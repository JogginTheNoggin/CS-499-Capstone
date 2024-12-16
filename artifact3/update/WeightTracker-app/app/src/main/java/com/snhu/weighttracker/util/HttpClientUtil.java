package com.snhu.weighttracker.util;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class HttpClientUtil {

    public static HttpResponse sendPostRequest(String urlString, String jsonInputString) throws Exception {
        URL url = new URL(urlString);
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();

        connection.setRequestMethod("POST");
        connection.setRequestProperty("Content-Type", "application/json");
        connection.setDoOutput(true);

        try (OutputStream os = connection.getOutputStream()) {
            byte[] input = jsonInputString.getBytes("utf-8");
            os.write(input, 0, input.length);
        }

        int statusCode = connection.getResponseCode();
        String response = readResponse(connection);
        return new HttpResponse(statusCode, response);
    }

    public static HttpResponse sendGetRequest(String urlString) throws Exception {
        URL url = new URL(urlString);
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();

        connection.setRequestMethod("GET");

        int statusCode = connection.getResponseCode();
        String response = readResponse(connection);
        return new HttpResponse(statusCode, response);
    }

    public static HttpResponse sendGetRequestWithBody(String urlString, String jsonInputString) throws Exception {
        URL url = new URL(urlString);
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();

        connection.setRequestMethod("GET");
        connection.setRequestProperty("Content-Type", "application/json");
        connection.setDoOutput(true);

        if (jsonInputString != null && !jsonInputString.isEmpty()) {
            try (OutputStream os = connection.getOutputStream()) {
                byte[] input = jsonInputString.getBytes("utf-8");
                os.write(input, 0, input.length);
            }
        }

        int statusCode = connection.getResponseCode();
        String response = readResponse(connection);
        return new HttpResponse(statusCode, response);
    }


    public static HttpResponse sendPutRequest(String urlString, String jsonInputString) throws Exception {
        URL url = new URL(urlString);
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();

        connection.setRequestMethod("PUT");
        connection.setRequestProperty("Content-Type", "application/json");
        connection.setDoOutput(true);

        if (jsonInputString != null && !jsonInputString.isEmpty()) {
            try (OutputStream os = connection.getOutputStream()) {
                byte[] input = jsonInputString.getBytes("utf-8");
                os.write(input, 0, input.length);
            }
        }

        int statusCode = connection.getResponseCode();
        String response = readResponse(connection);
        return new HttpResponse(statusCode, response);
    }



    private static String readResponse(HttpURLConnection connection) throws Exception {
        BufferedReader br = new BufferedReader(new InputStreamReader(connection.getInputStream(), "utf-8"));
        StringBuilder response = new StringBuilder();
        String responseLine;
        while ((responseLine = br.readLine()) != null) {
            response.append(responseLine.trim());
        }
        br.close();
        return response.toString();
    }

    public static HttpResponse sendDeleteRequest(String urlString, String jsonInputString) throws Exception {
        URL url = new URL(urlString);
        HttpURLConnection connection = (HttpURLConnection) url.openConnection();

        connection.setRequestMethod("DELETE");
        connection.setRequestProperty("Content-Type", "application/json");
        connection.setDoOutput(true);

        if (jsonInputString != null && !jsonInputString.isEmpty()) {
            try (OutputStream os = connection.getOutputStream()) {
                byte[] input = jsonInputString.getBytes("utf-8");
                os.write(input, 0, input.length);
            }
        }

        int statusCode = connection.getResponseCode();
        String response = readResponse(connection);
        return new HttpResponse(statusCode, response);
    }

}
