import boto3
import hashlib
import os
import json

# Initialize the DynamoDB client
dynamodb = boto3.resource("dynamodb")
table_name = "weightUsers"  # Replace with your DynamoDB table name
table = dynamodb.Table(table_name)

# Hashing helper function
def hash_password(password):
    salt = os.urandom(16)  # Generate a random 16-byte salt
    hashed = hashlib.pbkdf2_hmac("sha256", password.encode(), salt, 100000)
    return salt.hex(), hashed.hex()

def lambda_handler(event, context):
    try:
        # Parse the request body
        body = json.loads(event.get("body", "{}"))
        user_id = body.get("username")
        password = body.get("password")
        email = body.get("email")

        # Input validation
        if not user_id or not password or not email:
            return {
                "statusCode": 400,
                "body": json.dumps({"message": "username, password, and email are required"})
            }

        # Check if user already exists
        response = table.get_item(Key={"user_id": user_id})
        if "Item" in response:
            return {
                "statusCode": 409,
                "body": json.dumps({"message": "User already exists"})
            }

        # Hash the password
        salt, hashed_password = hash_password(password)

        # Save the new user to the database
        table.put_item(
            Item={
                "user_id": user_id,
                "email": email,
                "password_hash": hashed_password,
                "salt": salt,
                "sms_enabled": False  # Default value
            }
        )

        return {
            "statusCode": 201,
            "body": json.dumps({"message": "User created successfully"})
        }

    except Exception as e:
        print(f"Error: {e}")
        return {
            "statusCode": 500,
            "body": json.dumps({"message": "Internal server error"})
        }
