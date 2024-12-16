package com.snhu.weighttracker.Database;

public class DailyWeight {
    private int id;
    private int userId;
    private double weight;
    private String date;

    /*
    *
    * Used to encapsulate database entries for weights
    * */
    public DailyWeight(int id, int userId, double weight, String date) {
        this.id = id;
        this.userId = userId;
        this.weight = weight;
        this.date = date;
    }

    public int getId() {
        return id;
    }

    public int getUserId() {
        return userId;
    }

    public double getWeight() {
        return weight;
    }

    public String getDate() {
        return date;
    }
}
