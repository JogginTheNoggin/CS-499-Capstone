package com.snhu.weighttracker.Database;

import android.content.Context;
import android.content.SharedPreferences;

/* This class is used to manage Shared preferences between all classes
   this is done to prevent need to centralize the edits of SharedPreferences file */
public class SharedPreferenceManager {
    public static final String PREFS_NAME = "user_prefs";
    public static final String PREF_SIGNED_IN = "signed_in";
    public static final String PREF_USER_ID = "user_id";

    public static final String PREF_PASS = "password";
    public static final String PREF_SMS_ENABLED = "sms_enabled";

    private static boolean initialized = false;

    private static SharedPreferences sharedPreferences;
    private SharedPreferenceManager(){
    }

    public static void init(Context context){
        if(initialized){
            return;
        }
        else{
            sharedPreferences = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
            initialized = true;
        }

    }

    public static void saveSignIn(String userId, String password){
        if(!initialized){
            return;
        }
        if(userId == null){
            return;
        }
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREF_SIGNED_IN, true);
        editor.putString(PREF_USER_ID, userId);
        editor.putString(PREF_PASS, password);
        editor.putBoolean(PREF_SMS_ENABLED, isSmsEnabled());
        editor.apply();
    }


    public static boolean isSignedIn(){
        if(!initialized){
            return false;
        }
        return sharedPreferences.getBoolean(PREF_SIGNED_IN, false);
    }

    public static String getSignedInID(){
        if(!initialized){
            return null;
        }

        return sharedPreferences.getString(PREF_USER_ID, null);
    }

    public static String getPassword(){
        if(!initialized){
            return null;

        }return sharedPreferences.getString(PREF_PASS, null);
    }

    public static boolean isSmsEnabled() {
        if(!initialized){
            return false;
        }
        return sharedPreferences.getBoolean(PREF_SMS_ENABLED, false);
    }

    public static void setSmsEnabled(boolean enabled) {
        if(!initialized){
            return;
        }
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREF_SMS_ENABLED, enabled);
        editor.apply();
    }

    public static void signOut(){
        if(!initialized){
            return;
        }
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putBoolean(PREF_SIGNED_IN, false);
        editor.putString(PREF_USER_ID, null);
        editor.putString(PREF_PASS, null);
        editor.putBoolean(PREF_SMS_ENABLED, false);
        editor.apply();
    }
}
