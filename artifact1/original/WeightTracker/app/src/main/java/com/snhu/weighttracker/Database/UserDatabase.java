package com.snhu.weighttracker.Database;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

public class UserDatabase extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "users.db";
    private static final int VERSION = 1;

    public UserDatabase(Context context) {
        super(context, DATABASE_NAME, null, VERSION);
    }

    private static final class UserTable {
        private static final String TABLE = "users";
        private static final String COL_ID = "_id";
        private static final String COL_USERNAME = "username";
        private static final String COL_EMAIL = "email";
        private static final String COL_PASSWORD = "password";
        private static final String COL_SMS_ENABLED = "sms_enabled";
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        // create the user database table
        db.execSQL("CREATE TABLE " + UserTable.TABLE + " (" +
                UserTable.COL_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                UserTable.COL_USERNAME + " TEXT, " +
                UserTable.COL_EMAIL + " TEXT, " +
                UserTable.COL_PASSWORD + " TEXT, " +
                UserTable.COL_SMS_ENABLED + " INTEGER DEFAULT 0" + // Default SMS status to false
                ")");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + UserTable.TABLE);
        onCreate(db);
    }

    public boolean addUser(String username, String email, String password) {
        if (isUsernameExists(username)) {
            return false;
        }
        SQLiteDatabase db = this.getWritableDatabase(); // writable database connection
        ContentValues values = new ContentValues(); // values we want to insert and the columns of the row
        values.put(UserTable.COL_USERNAME, username);
        values.put(UserTable.COL_EMAIL, email);
        values.put(UserTable.COL_PASSWORD, password);
        long result = db.insert(UserTable.TABLE, null, values); // execute
        // Log.i("Database", "" + result);
        db.close();
        return result != -1;
    }

    public boolean validateUser(String username, String password) {
        SQLiteDatabase db = this.getReadableDatabase(); // create connection
        String[] columns = {UserTable.COL_ID}; // specify the column we want out of the queries result
        String selection = UserTable.COL_USERNAME + " = ? AND " + UserTable.COL_PASSWORD + " = ?"; // the parameterized selection part of query,
        String[] selectionArgs = {username, password}; // values of selection parameters
        Cursor cursor = db.query(UserTable.TABLE, columns, selection, selectionArgs, null, null, null); // execute query
        boolean isValid = cursor.getCount() > 0; // if cursor has a row then we know the user exists
        cursor.close();
        db.close();
        return isValid;
    }

    public boolean isUsernameExists(String username) {
        SQLiteDatabase db = this.getReadableDatabase(); // db connect
        String[] columns = {UserTable.COL_ID}; // column we need from query result
        String selection = UserTable.COL_USERNAME + " = ?"; // parameterized selection part of query
        String[] selectionArgs = {username}; // value of parameter
        Cursor cursor = db.query(UserTable.TABLE, columns, selection, selectionArgs, null, null, null); // execute query
        boolean exists = cursor.getCount() > 0; // if cursor contains row, we know user exists with username
        cursor.close();
        db.close();
        return exists;
    }

    public int getUserIdByUsername(String username) {
        SQLiteDatabase db = this.getReadableDatabase(); // sql connection
        String[] columns = {UserTable.COL_ID}; // column of interest
        String selection = UserTable.COL_USERNAME + " = ?"; // parameterized selection part of query
        String[] selectionArgs = {username}; // query parameter value
        Cursor cursor = db.query(UserTable.TABLE, columns, selection, selectionArgs, null, null, null); // execute query

        int userId = -1;
        if (cursor.moveToFirst()) { // if contains row, has result
            int idColumnIndex = cursor.getColumnIndex(UserTable.COL_ID);
            if (idColumnIndex >= 0) {
                userId = cursor.getInt(idColumnIndex);
            }
        }
        cursor.close();
        db.close();
        return userId;
    }

    public boolean getSmsStatus(int userId) {
        SQLiteDatabase db = getReadableDatabase(); // connect
        // QUERY contains in order: column of interest, selection, parameter values
        Cursor cursor = db.query(UserTable.TABLE,
                new String[]{UserTable.COL_SMS_ENABLED},
                UserTable.COL_ID + " = ?",
                new String[]{String.valueOf(userId)},
                null, null, null);

        boolean smsEnabled = false;
        if (cursor.moveToFirst()) { // if result
            int idColumnIndex = cursor.getColumnIndex(UserTable.COL_SMS_ENABLED);

            if(idColumnIndex >= 0) {
                smsEnabled = cursor.getInt(idColumnIndex) == 1;
            }
        }
        cursor.close();
        return smsEnabled;
    }

    public boolean setSmsStatus(int userId, boolean enabled) {
        SQLiteDatabase db = getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(UserTable.COL_SMS_ENABLED, enabled ? 1 : 0); // set sms status
        int rows = db.update(UserTable.TABLE, values, UserTable.COL_ID + " = ?", new String[]{String.valueOf(userId)});
        return rows > 0;
    }
}
