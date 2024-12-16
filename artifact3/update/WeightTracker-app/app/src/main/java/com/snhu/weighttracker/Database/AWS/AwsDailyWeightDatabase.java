package com.snhu.weighttracker.Database.AWS;

import com.snhu.weighttracker.util.HttpClientUtil;
import com.snhu.weighttracker.util.HttpResponse;

import org.json.JSONArray;
import org.json.JSONObject;
import java.util.ArrayList;
import java.util.List;

public class AwsDailyWeightDatabase {
    private final String END_POINT = "https://o7vdan0ojl.execute-api.us-east-1.amazonaws.com/test/users/dailyWeights";



    public boolean addDailyWeight(String userId, String password, double weight) {
        try {
            // Create JSON body
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("username", userId);
            jsonBody.put("password", password); // Replace with actual password
            jsonBody.put("weight", weight);

            // Send POST request
            HttpResponse response = HttpClientUtil.sendPostRequest(END_POINT, jsonBody.toString());

            // Check if the operation was successful
            return response.getStatusCode() == HttpResponse.StatusCode.CREATED.getCode();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }



    public List<AwsDailyWeight> getAllWeightsForUser(String userId, String password) {
        List<AwsDailyWeight> dailyWeights = new ArrayList<>();
        try {
            // Create JSON body for authentication
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("password", password);

            // Construct URL with userId as a query parameter
            String urlWithQuery = END_POINT + "?user_id=" + userId;

            // Send GET request with body
            HttpResponse response = HttpClientUtil.sendGetRequestWithBody(urlWithQuery, jsonBody.toString());

            // Parse the JSON response
            if (response.getStatusCode() == HttpResponse.StatusCode.OK.getCode()) {
                JSONArray jsonArray = new JSONArray(response.getBody());
                for (int i = 0; i < jsonArray.length(); i++) {
                    JSONObject weightEntry = jsonArray.getJSONObject(i);
                    String time = weightEntry.getString("timeSubmitted");
                    double weight = weightEntry.getDouble("weight");

                    // Add to the list
                    dailyWeights.add(new AwsDailyWeight(userId, weight, time));
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return dailyWeights;
    }



    public boolean deleteDailyWeight(String userId, String password, String weightId) {
        try {
            // Create JSON body with password
            JSONObject jsonBody = new JSONObject();
            jsonBody.put("username", userId);
            jsonBody.put("password", password);
            jsonBody.put("weight_id", weightId);

            // Construct URL
            String url = END_POINT + "/delete";

            // Send DELETE request with body
            HttpResponse response = HttpClientUtil.sendDeleteRequest(url, jsonBody.toString());

            // Check if the operation was successful
            return response.getStatusCode() == HttpResponse.StatusCode.OK.getCode();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

}
