package com.snhu.weighttracker.Database;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

//Used to access the DailyWeights table
public class DailyWeightDatabase extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "dailyWeights.db";
    private static final int VERSION = 1;


    public DailyWeightDatabase(Context context) {
        super(context, DATABASE_NAME, null, VERSION);
    }

    private static final class DailyWeightTable {
        private static final String TABLE = "dailyWeights";
        private static final String COL_ID = "_id";
        private static final String COL_USER_ID = "user_id"; // user id will be a foreign key, from the userDatabase
        private static final String COL_WEIGHT = "weight";
        private static final String COL_DATE = "date";
    }

    // create the table
    @Override
    public void onCreate(SQLiteDatabase db) {
        // defines the table that DailyWeights will be in
        db.execSQL("CREATE TABLE " + DailyWeightTable.TABLE + " (" +
                DailyWeightTable.COL_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                DailyWeightTable.COL_USER_ID + " INTEGER, " +
                DailyWeightTable.COL_WEIGHT + " REAL, " +
                DailyWeightTable.COL_DATE + " TEXT, " +
                "FOREIGN KEY (" + DailyWeightTable.COL_USER_ID + ") REFERENCES users(_id))");
    }


    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + DailyWeightTable.TABLE);
        onCreate(db);
    }

    // insert the entry into the table, and return the result as a boolean
    public boolean addDailyWeight(int userId, double weight, String date) {
        SQLiteDatabase db = this.getWritableDatabase();
        // create values to insert using the parameter values
        ContentValues values = new ContentValues();
        values.put(DailyWeightTable.COL_USER_ID, userId);
        values.put(DailyWeightTable.COL_WEIGHT, weight);
        values.put(DailyWeightTable.COL_DATE, date);

        // attempt to insert new row
        long result = db.insert(DailyWeightTable.TABLE, null, values);
        db.close();
        return result != -1;
    }

    public List<DailyWeight> getAllWeightsForUser(int userId) {
        SQLiteDatabase db = this.getReadableDatabase(); // open database
        //create the query to get all entries in daily weight where, it contains the usersId
        String[] columns = {DailyWeightTable.COL_ID, DailyWeightTable.COL_WEIGHT, DailyWeightTable.COL_DATE};
        String selection = DailyWeightTable.COL_USER_ID + " = ?";
        String[] selectionArgs = {String.valueOf(userId)};
        Cursor cursor = db.query(DailyWeightTable.TABLE, columns, selection, selectionArgs, null, null, DailyWeightTable.COL_DATE + " DESC");

        List<DailyWeight> dailyWeights = new ArrayList<>();
        if (cursor != null) { // contains rows
            // check if contains more rows if so move to next
            while (cursor.moveToNext()) {
                int id = cursor.getInt(cursor.getColumnIndexOrThrow(DailyWeightTable.COL_ID));
                double weight = cursor.getDouble(cursor.getColumnIndexOrThrow(DailyWeightTable.COL_WEIGHT));
                String date = cursor.getString(cursor.getColumnIndexOrThrow(DailyWeightTable.COL_DATE));
                dailyWeights.add(new DailyWeight(id, userId, weight, date));
            }
            cursor.close();
        }
        db.close(); // close the database connection before exiting
        return dailyWeights;
    }

    public boolean deleteDailyWeight(int id) {
        SQLiteDatabase db = this.getWritableDatabase();
        // use primary index of row to delete an entry if it exist
        int rowsDeleted = db.delete(DailyWeightTable.TABLE, DailyWeightTable.COL_ID + " = ?", new String[]{String.valueOf(id)});
        db.close();
        return rowsDeleted > 0; // return if successful
    }
}
