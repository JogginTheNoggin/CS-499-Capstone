package com.snhu.weighttracker.Database;

import android.content.Context;
import android.content.SharedPreferences;

/* This class is used to manage Shared preferences between all classes
   this is done to prevent need to centralize the edits of SharedPreferences file */
public class SharedPreferenceManager {
    public static final String PREFS_NAME = "user_prefs";
    public static final String PREF_SIGNED_IN = "signed_in";
    public static final String PREF_USER_ID = "user_id";
    public static final String PREF_SMS_ENABLED = "sms_enabled";

    private final SharedPreferences sharedPreferences;
    public SharedPreferenceManager(Context context){
        sharedPreferences = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
    }

    public void saveSignIn(int userId){
        if(userId < 0){
            return;
        }
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREF_SIGNED_IN, true);
        editor.putInt(PREF_USER_ID, userId);
        editor.putBoolean(PREF_SMS_ENABLED, isSmsEnabled());
        editor.apply();
    }

    public boolean isSignedIn(){
        return sharedPreferences.getBoolean(PREF_SIGNED_IN, false);
    }

    public int getSignedInID(){
        return sharedPreferences.getInt(PREF_USER_ID, -1);
    }

    public boolean isSmsEnabled() {
        return sharedPreferences.getBoolean(PREF_SMS_ENABLED, false);
    }

    public void setSmsEnabled(boolean enabled) {
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREF_SMS_ENABLED, enabled);
        editor.apply();
    }

    public void signOut(){
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREF_SIGNED_IN, false);
        editor.putInt(PREF_USER_ID, -1);
        editor.putBoolean(PREF_SMS_ENABLED, false);
        editor.apply();
    }
}
