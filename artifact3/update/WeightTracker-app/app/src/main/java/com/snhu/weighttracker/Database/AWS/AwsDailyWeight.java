package com.snhu.weighttracker.Database.AWS;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class AwsDailyWeight {

    private String userId;
    private String time; // used as id
    private String date; // human-readable date
    private double weight;

    /*
     * Encapsulates database entries for weights
     */
    public AwsDailyWeight(String userId, double weight, String time) {
        this.userId = userId;
        this.weight = weight;
        this.time = time;
        this.date = convertToHumanReadableDate(time); // Convert to human-readable format
    }

    public String getUserId() {
        return userId;
    }

    public double getWeight() {
        return weight;
    }

    public String getWeightId() {
        return time;
    }

    public String getDate() {
        return date;
    }

    private String convertToHumanReadableDate(String time) {
        try {
            long timestamp = Long.parseLong(time) * 1000; // Convert to milliseconds
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            return sdf.format(new Date(timestamp));
        } catch (NumberFormatException e) {
            return "Invalid timestamp";
        }
    }
}
