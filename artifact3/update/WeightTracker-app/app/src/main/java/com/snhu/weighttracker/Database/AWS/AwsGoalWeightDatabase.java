package com.snhu.weighttracker.Database.AWS;

import com.snhu.weighttracker.util.HttpClientUtil;
import com.snhu.weighttracker.util.HttpResponse;
import org.json.JSONObject;

public class AwsGoalWeightDatabase {
    private final String END_POINT = "https://o7vdan0ojl.execute-api.us-east-1.amazonaws.com/test/users";

    public boolean setGoalWeight(String username, String password, double goalWeight) {
        try {
            // Create JSON body with password
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", password);

            // Construct URL with query parameters
            String urlWithQuery = END_POINT + "/goalWeight?user_id=" + username + "&goal_weight=" + goalWeight;

            // Send PUT request
            HttpResponse response = HttpClientUtil.sendPutRequest(urlWithQuery, jsonBody.toString());

            // Check if the operation was successful
            return response.getStatusCode() == HttpResponse.StatusCode.OK.getCode();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }


    public Double getGoalWeight(String username, String password) {
        try {
            // Create JSON body with password
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", password);

            // Construct URL with query parameters
            String urlWithQuery = END_POINT + "/goalWeight?user_id=" + username;

            // Send GET request with body
            HttpResponse response = HttpClientUtil.sendGetRequestWithBody(urlWithQuery, jsonBody.toString());

            // Check if the response is successful and parse the goal weight
            if (response.getStatusCode() == HttpResponse.StatusCode.OK.getCode()) {
                JSONObject jsonResponse = new JSONObject(response.getBody());
                return jsonResponse.getDouble("goal_weight");
            } else if (response.getStatusCode() == HttpResponse.StatusCode.NOT_FOUND.getCode()) {
                System.out.println("Goal weight not found for the user.");
            }
            return null;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

}
