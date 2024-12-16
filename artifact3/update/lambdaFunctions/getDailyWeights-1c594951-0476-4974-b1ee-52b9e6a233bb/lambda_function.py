import boto3
import hashlib
import json
from boto3.dynamodb.conditions import Key
from decimal import Decimal

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

# Utility to convert Decimal to float
def convert_decimal(obj):
    if isinstance(obj, list):
        return [convert_decimal(i) for i in obj]
    elif isinstance(obj, dict):
        return {k: convert_decimal(v) for k, v in obj.items()}
    elif isinstance(obj, Decimal):
        return float(obj)
    else:
        return obj

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
                "body": json.dumps({"message": "user_id and password  are required"})
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

        # Query all weight entries for the user from dailyWeights table
        response = weights_table.query(
            KeyConditionExpression=Key("user_id").eq(user_id)
        )

        # Convert Decimal objects to float
        weights = convert_decimal(response.get("Items", []))

        # Return the results
        return {
            "statusCode": 200,
            "body": json.dumps({"weights": weights})
        }

    except Exception as e:
        print(f"Error: {e}")
        return {
            "statusCode": 500,
            "body": json.dumps({"message": "Internal server error"})
        }
