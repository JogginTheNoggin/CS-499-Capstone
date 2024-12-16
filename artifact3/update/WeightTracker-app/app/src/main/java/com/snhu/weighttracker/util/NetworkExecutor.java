package com.snhu.weighttracker.util;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class NetworkExecutor {
    private static final ExecutorService executor = Executors.newFixedThreadPool(4);

    public static ExecutorService getExecutor() {
        return executor;
    }
}
