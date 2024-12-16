import boto3
import hashlib
import json
from boto3.dynamodb.conditions import Key

# Initialize the DynamoDB client
dynamodb = boto3.resource("dynamodb")
users_table_name = "weightUsers"  
weights_table_name = "dailyWeights"  
users_table = dynamodb.Table(users_table_name)
weights_table = dynamodb.Table(weights_table_name)

# Hashing helper function
def hash_password(password, salt):
    return hashlib.pbkdf2_hmac("sha256", password.encode(), salt, 100000)

# Validate the user's password
def validate_password(provided_password, stored_hash, stored_salt):
    derived_hash = hash_password(provided_password, stored_salt)
    return derived_hash == stored_hash

def lambda_handler(event, context):
    try:
        # Parse the request body
        body = json.loads(event.get("body", "{}"))
        user_id = body.get("username")
        password = body.get("password")
        weight_id = body.get("weight_id")  # Equivalent to timeSubmitted

        # Validate input
        if not user_id or not password or not weight_id:
            return {
                "statusCode": 400,
                "body": json.dumps({"message": "username, password, and weight_id are required"})
            }

        # Fetch user data from Users table
        user_response = users_table.get_item(Key={"user_id": user_id})
        if "Item" not in user_response:
            return {
                "statusCode": 404,
                "body": json.dumps({"message": "User not found"})
            }

        # Extract stored password hash and salt
        user_data = user_response["Item"]
        stored_salt = bytes.fromhex(user_data["salt"])
        stored_hash = bytes.fromhex(user_data["password_hash"])

        # Validate the password
        if not validate_password(password, stored_hash, stored_salt):
            return {
                "statusCode": 403,
                "body": json.dumps({"message": "Invalid password"})
            }

        # Delete the weight entry from the dailyWeights table
        delete_response = weights_table.delete_item(
            Key={
                "user_id": user_id,
                "timeSubmitted": weight_id
            },
            ReturnValues="ALL_OLD"  # Returns the deleted item if it existed
        )

        # Check if the item was deleted
        if "Attributes" not in delete_response:
            return {
                "statusCode": 404,
                "body": json.dumps({"message": "Weight entry not found"})
            }

        return {
            "statusCode": 200,
            "body": json.dumps({"message": "Weight entry deleted successfully"})
        }

    except Exception as e:
        print(f"Error: {e}")
        return {
            "statusCode": 500,
            "body": json.dumps({"message": "Internal server error"})
        }

