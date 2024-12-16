import boto3
import hashlib
import json
from boto3.dynamodb.conditions import Key

# Initialize the DynamoDB client
dynamodb = boto3.resource("dynamodb")
users_table_name = "weightUsers"  
goal_weight_table_name = "goalWeights"  
users_table = dynamodb.Table(users_table_name)
goal_weight_table = dynamodb.Table(goal_weight_table_name)

# Hashing helper function
def hash_password(password, salt):
    return hashlib.pbkdf2_hmac("sha256", password.encode(), salt, 100000)

# Validate the user's password
def validate_password(provided_password, stored_hash, stored_salt):
    derived_hash = hash_password(provided_password, stored_salt)
    return derived_hash == stored_hash

def lambda_handler(event, context):
    try:
        # Extract query parameters and request body
        query_params = event.get("queryStringParameters", {})
        user_id = query_params.get("user_id")

        body = json.loads(event.get("body", "{}"))
        password = body.get("password")

        # Validate input
        if not user_id or not password:
            return {
                "statusCode": 400,
                "body": json.dumps({"message": "user_id and password are required"})
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

        # Fetch the goal weight from the GoalWeights table
        goal_weight_response = goal_weight_table.get_item(Key={"user_id": user_id})

        # Check if the goal weight exists
        if "Item" not in goal_weight_response:
            return {
                "statusCode": 404,
                "body": json.dumps({"message": "Goal weight not set for this user"})
            }

        # Extract and return the goal weight
        goal_weight = float(goal_weight_response["Item"]["goal_weight"])
        return {
            "statusCode": 200,
            "body": json.dumps({"goal_weight": goal_weight})
        }

    except Exception as e:
        print(f"Error: {e}")
        return {
            "statusCode": 500,
            "body": json.dumps({"message": "Internal server error"})
        }
