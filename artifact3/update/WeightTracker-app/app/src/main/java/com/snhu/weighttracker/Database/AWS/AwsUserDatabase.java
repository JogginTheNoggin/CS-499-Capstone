package com.snhu.weighttracker.Database.AWS;

import android.content.Context;
import com.snhu.weighttracker.Database.UserDatabase;
import com.snhu.weighttracker.util.HttpClientUtil;
import com.snhu.weighttracker.util.HttpResponse;

import org.json.JSONObject;

public class AwsUserDatabase{
    private final String END_POINT = "https://o7vdan0ojl.execute-api.us-east-1.amazonaws.com/test/users";



    public boolean addUser(String username, String email, String password) {
        try {
            // Create JSON body
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("username", username);
            jsonBody.put("email", email);
            jsonBody.put("password", password);

            // Send POST request
            HttpResponse response = HttpClientUtil.sendPostRequest(END_POINT, jsonBody.toString());


            return response.getStatusCode() == HttpResponse.StatusCode.CREATED.getCode();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    public boolean validateUser(String username, String password) {
        try {
            // Create JSON body for the password
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", password);

            // Construct URL with username as a query parameter
            String urlWithQuery = END_POINT + "?user_id=" + username;

            // Send POST request with password in the body
            HttpResponse response = HttpClientUtil.sendGetRequestWithBody(urlWithQuery, jsonBody.toString());
            
            // Check if the status code is 200 (OK)
            return response.getStatusCode() == HttpResponse.StatusCode.OK.getCode();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }



    public boolean isUsernameExists(String username) {
        try {
            // Send GET request
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", "anything");
            HttpResponse response = HttpClientUtil.sendGetRequestWithBody(END_POINT + "?user_id=" + username,
                    jsonBody.toString());

            return response.getStatusCode() != HttpResponse.StatusCode.NOT_FOUND.getCode(); // just default, not perfect
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }




    public boolean getSmsStatus(String username, String password) {
        try {
            // Send GET request
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", "anything");
            HttpResponse response = HttpClientUtil.sendGetRequestWithBody(END_POINT + "?user_id=" + username,
                    jsonBody.toString());

            // Parse response
            JSONObject jsonResponse = new JSONObject(response.getBody());
            return jsonResponse.getBoolean("smsEnabled");
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }


    public boolean setSmsStatus(String username, String password, boolean enabled) {
        try {
            // Create JSON body with the password
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", password);
            jsonBody.put("smsEnabled", enabled);

            // Construct URL with userId as a query parameter
            String urlWithQuery = END_POINT + "?user_id=" + username;

            // Send PUT request
            HttpResponse response = HttpClientUtil.sendPutRequest(urlWithQuery, jsonBody.toString());

            // Check if the operation was successful
            return response.getStatusCode() == HttpResponse.StatusCode.OK.getCode();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
}
