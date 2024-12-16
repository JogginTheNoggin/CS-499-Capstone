package com.snhu.weighttracker.ui.notifications;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import com.snhu.weighttracker.Database.AWS.AwsUserDatabase;
import com.snhu.weighttracker.Database.SharedPreferenceManager;
import com.snhu.weighttracker.Database.UserDatabase;

public class NotificationViewModel extends ViewModel {

    // used to construct viewmodel and pass context
    public static class Factory implements ViewModelProvider.Factory {
        private final Context context;

        public Factory(Context context) {
            this.context = context;
        }

        @NonNull
        @Override
        public <T extends ViewModel> T create(@NonNull Class<T> modelClass) {
            if (modelClass.isAssignableFrom(NotificationViewModel.class)) {
                return (T) new NotificationViewModel(context);
            }
            throw new IllegalArgumentException("Unknown ViewModel class");
        }
    }
    private final MutableLiveData<String> mText;
    private final MutableLiveData<Boolean> smsPermissionGranted;
    private final MutableLiveData<Boolean> smsStatusUpdated;


    private AwsUserDatabase userDatabase;

    public NotificationViewModel(Context context) {
        mText = new MutableLiveData<>();
        mText.setValue("SMS Notification Settings");
        smsPermissionGranted = new MutableLiveData<>();
        smsStatusUpdated = new MutableLiveData<>();

        userDatabase = new AwsUserDatabase();
    }

    public LiveData<String> getText() {
        return mText;
    }

    public LiveData<Boolean> getSmsPermissionGranted() {
        return smsPermissionGranted;
    }

    public LiveData<Boolean> getSmsStatusUpdated() {
        return smsStatusUpdated;
    }

    public boolean isSmsEnabled() {
        return SharedPreferenceManager.isSmsEnabled();
    }

    public void updateSmsStatus(boolean enabled) {
        // change the users sms status based on the passed parameter
        String userId = SharedPreferenceManager.getSignedInID();
        String password = SharedPreferenceManager.getPassword();
        if (userId != null) {
            if (userDatabase.setSmsStatus(userId, password, enabled)) {
                SharedPreferenceManager.setSmsEnabled(enabled);
                smsStatusUpdated.setValue(true);
            } else {
                smsStatusUpdated.setValue(false);
            }
        }
    }
}