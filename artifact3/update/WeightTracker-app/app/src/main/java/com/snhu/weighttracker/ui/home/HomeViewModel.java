package com.snhu.weighttracker.ui.home;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import com.snhu.weighttracker.Database.AWS.AwsDailyWeight;
import com.snhu.weighttracker.Database.AWS.AwsDailyWeightDatabase;
import com.snhu.weighttracker.Database.AWS.AwsGoalWeightDatabase;

import com.snhu.weighttracker.Database.SharedPreferenceManager;
import com.snhu.weighttracker.Database.SharedViewModel;

import java.util.List;

public class HomeViewModel extends ViewModel {


    // used to pass context to viewmodel
    public static class Factory implements ViewModelProvider.Factory {
        private final Context mContext;
        private final SharedViewModel sharedViewModel;

        public Factory(Context context, SharedViewModel sharedViewModel) {
            mContext = context;
            this.sharedViewModel = sharedViewModel;
        }

        @NonNull
        @Override
        public <T extends ViewModel> T create(@NonNull Class<T> modelClass) {
            if (modelClass.isAssignableFrom(HomeViewModel.class)) {
                return (T) new HomeViewModel(mContext, sharedViewModel);
            }
            throw new IllegalArgumentException("Unknown ViewModel class");
        }
    }

    private final MutableLiveData<String> goalWeightText;
    private final MutableLiveData<List<AwsDailyWeight>> dailyWeights;
    private AwsGoalWeightDatabase goalWeightDatabase;
    private AwsDailyWeightDatabase dailyWeightDatabase;


    public HomeViewModel(Context context, SharedViewModel sharedViewModel) {
        goalWeightText = new MutableLiveData<>();
        dailyWeights = new MutableLiveData<>();
        goalWeightDatabase = new AwsGoalWeightDatabase();
        dailyWeightDatabase = new AwsDailyWeightDatabase();

        // use shared view model to check for changes to data, like sign in sign out or new weights
        sharedViewModel.getDataChanged().observeForever(new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean dataChanged) {
                if (dataChanged) {
                    loadUserData();
                    sharedViewModel.resetDataChanged();
                }
            }
        });

        loadUserData();
    }

    public LiveData<String> getGoalWeightText() {
        return goalWeightText;
    }

    public LiveData<List<AwsDailyWeight>> getDailyWeights() {
        return dailyWeights;
    }

    private void loadUserData() {
        // if signed in get the goal weight user has saved in database and set it to textView
        if (SharedPreferenceManager.isSignedIn()) {
            String userId = SharedPreferenceManager.getSignedInID();
            String password = SharedPreferenceManager.getPassword();
            Double goalWeight = goalWeightDatabase.getGoalWeight(userId, password);
            if (goalWeight != null ) {
                goalWeightText.setValue("Goal Weight: " + goalWeight + " lbs");
            } else {
                goalWeightText.setValue("Goal Weight: Not Set");
            }

            List<AwsDailyWeight> weights = dailyWeightDatabase.getAllWeightsForUser(userId, password);
            dailyWeights.setValue(weights);
        } else {
            goalWeightText.setValue("Not Signed In");
            dailyWeights.setValue(null);
        }
    }

    public void deleteDailyWeight(String username, String password,  String id) {
        // remove the daily weight entry for the specific user
        if (dailyWeightDatabase.deleteDailyWeight(username, password, id)) {
            loadUserData();
        }
    }

    public boolean isSignedIn(){
        return SharedPreferenceManager.isSignedIn();
    }



}