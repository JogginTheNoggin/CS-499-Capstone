import boto3
import hashlib
import json

# Initialize the DynamoDB client
dynamodb = boto3.resource("dynamodb")
table_name = "weightUsers"  # Replace with your DynamoDB table name
table = dynamodb.Table(table_name)

# Hashing helper function
def hash_password(password, salt):
    return hashlib.pbkdf2_hmac("sha256", password.encode(), salt, 100000)

# Validate the user's password
def validate_password(provided_password, stored_hash, stored_salt):
    derived_hash = hash_password(provided_password, stored_salt)
    return derived_hash == stored_hash

def lambda_handler(event, context):
    # Extract query parameters and request body
    query_params = event.get("queryStringParameters", {})
    user_id = query_params.get("user_id")
    
    try:
        body = json.loads(event.get("body", "{}"))
        password = body.get("password")
        sms_enabled = body.get("sms_enabled")
    except json.JSONDecodeError:
        return {
            "statusCode": 400,
            "body": json.dumps({"message": "Invalid request body"})
        }

    # Validate input
    if not user_id or not password or sms_enabled is None:
        return {
            "statusCode": 400,
            "body": json.dumps({"message": "user_id (query parameter), password, and sms_enabled (in body) are required"})
        }

    try:
        # Fetch user data from DynamoDB
        response = table.get_item(Key={"user_id": user_id})
        if "Item" not in response:
            return {
                "statusCode": 404,
                "body": json.dumps({"message": "User not found"})
            }

        # Extract stored password hash and salt
        user_data = response["Item"]
        stored_salt = bytes.fromhex(user_data["salt"])
        stored_hash = bytes.fromhex(user_data["password_hash"])

        # Validate the password
        if not validate_password(password, stored_hash, stored_salt):
            return {
                "statusCode": 403,
                "body": json.dumps({"message": "Invalid password"})
            }

        # Update the sms_enabled field in DynamoDB
        table.update_item(
            Key={"user_id": user_id},
            UpdateExpression="SET sms_enabled = :sms",
            ExpressionAttributeValues={":sms": sms_enabled},
            ReturnValues="UPDATED_NEW"
        )

        return {
            "statusCode": 200,
            "body": json.dumps({"message": "sms_enabled updated successfully"})
        }

    except Exception as e:
        print(f"Error: {e}")
        return {
            "statusCode": 500,
            "body": json.dumps({"message": "Internal server error"})
        }
