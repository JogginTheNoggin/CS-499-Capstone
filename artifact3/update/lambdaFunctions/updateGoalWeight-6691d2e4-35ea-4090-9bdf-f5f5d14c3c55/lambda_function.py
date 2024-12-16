import boto3
import hashlib
import json
from boto3.dynamodb.conditions import Key
from decimal import Decimal

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
        # Parse the request body
        body = json.loads(event.get("body", "{}"))
        user_id = body.get("username")
        password = body.get("password")
        goal_weight = body.get("goal_weight")

        # Validate input
        if not user_id or not password or goal_weight is None:
            return {
                "statusCode": 400,
                "body": json.dumps({"message": "username, password, and goal_weight are required"})
            }

        # Validate goal weight type
        try:
            goal_weight = float(goal_weight)
            if goal_weight <= 0:
                raise ValueError("Goal weight must be positive")
        except ValueError:
            return {
                "statusCode": 400,
                "body": json.dumps({"message": "Goal weight must be a valid positive number"})
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

        # Create or update the goal weight in the GoalWeights table
        goal_weight_table.put_item(
            Item={
                "user_id": user_id,  # Partition key
                "goal_weight": Decimal(str(goal_weight))  # Convert to Decimal for DynamoDB
            }
        )

        return {
            "statusCode": 200,
            "body": json.dumps({"message": "Goal weight saved successfully"})
        }

    except Exception as e:
        print(f"Error: {e}")
        return {
            "statusCode": 500,
            "body": json.dumps({"message": "Internal server error"})
        }
