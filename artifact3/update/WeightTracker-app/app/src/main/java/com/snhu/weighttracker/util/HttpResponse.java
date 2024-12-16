package com.snhu.weighttracker.util;

public class HttpResponse {
    public enum StatusCode {
        OK(200),
        CREATED(201),
        NO_CONTENT(204),
        BAD_REQUEST(400),
        UNAUTHORIZED(401),
        FORBIDDEN(403),
        NOT_FOUND(404),
        INTERNAL_SERVER_ERROR(500);

        private final int code;

        StatusCode(int code) {
            this.code = code;
        }

        public int getCode() {
            return code;
        }

        public static StatusCode fromCode(int code) {
            for (StatusCode status : values()) {
                if (status.getCode() == code) {
                    return status;
                }
            }
            return null; // Return null or throw an exception for unsupported codes
        }
    }

    private final int statusCode;
    private final String body;

    public HttpResponse(int statusCode, String body) {
        this.statusCode = statusCode;
        this.body = body;
    }

    public int getStatusCode() {
        return statusCode;
    }

    public StatusCode getStatus() {
        return StatusCode.fromCode(statusCode);
    }

    public String getBody() {
        return body;
    }

    public boolean isSuccessful() {
        return statusCode >= 200 && statusCode < 300;
    }

    @Override
    public String toString() {
        return "HttpResponse{" +
                "statusCode=" + statusCode +
                ", status=" + getStatus() +
                ", body='" + body + '\'' +
                '}';
    }
}

