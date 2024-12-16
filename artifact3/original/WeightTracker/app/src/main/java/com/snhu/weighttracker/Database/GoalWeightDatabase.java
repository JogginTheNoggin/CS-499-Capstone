package com.snhu.weighttracker.Database;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

//Used to access the GoalWeights table
public class GoalWeightDatabase extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "goalWeight.db";
    private static final int VERSION = 1;

    public GoalWeightDatabase(Context context) {
        super(context, DATABASE_NAME, null, VERSION);
    }

    private static final class GoalWeightTable {
        private static final String TABLE = "goalWeights";
        private static final String COL_USER_ID = "user_id";
        private static final String COL_GOAL_WEIGHT = "goal_weight";
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        // defines the table that GoalWeights will be in
        db.execSQL("CREATE TABLE " + GoalWeightTable.TABLE + " (" +
                GoalWeightTable.COL_USER_ID + " INTEGER PRIMARY KEY, " +
                GoalWeightTable.COL_GOAL_WEIGHT + " REAL, " +
                "FOREIGN KEY (" + GoalWeightTable.COL_USER_ID + ") REFERENCES users(_id))");
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + GoalWeightTable.TABLE);
        onCreate(db);
    }

    public boolean setGoalWeight(int userId, double goalWeight) {
        SQLiteDatabase db = this.getWritableDatabase(); // create db connection
        ContentValues values = new ContentValues(); // store values to be entered
        values.put(GoalWeightTable.COL_USER_ID, userId);
        values.put(GoalWeightTable.COL_GOAL_WEIGHT, goalWeight);

        //insert row if one does not exist, overwrites otherwise
        long result = db.insertWithOnConflict(GoalWeightTable.TABLE, null, values, SQLiteDatabase.CONFLICT_REPLACE);
        db.close();
        return result != -1;
    }

    public Double getGoalWeight(int userId) {
        SQLiteDatabase db = this.getReadableDatabase();
        //create the query to get the specific goal weight for the user with passed userID
        String[] columns = {GoalWeightTable.COL_GOAL_WEIGHT}; // specific column we want to get from query
        String selection = GoalWeightTable.COL_USER_ID + " = ?"; // the selection query
        String[] selectionArgs = {String.valueOf(userId)}; // value of query parameter
        Cursor cursor = db.query(GoalWeightTable.TABLE, columns, selection, selectionArgs, null, null, null);

        Double goalWeight = null;
        // check if any entries found, should only be one per userId
        if (cursor.moveToFirst()) {
            int goalWeightColumnIndex = cursor.getColumnIndex(GoalWeightTable.COL_GOAL_WEIGHT);
            if (goalWeightColumnIndex >= 0) {
                goalWeight = cursor.getDouble(goalWeightColumnIndex);
            }
        }
        cursor.close();
        db.close();
        return goalWeight;
    }
}
