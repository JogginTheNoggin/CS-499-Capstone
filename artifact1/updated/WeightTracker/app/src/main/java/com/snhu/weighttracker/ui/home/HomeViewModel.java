package com.snhu.weighttracker.ui.home;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import com.snhu.weighttracker.Database.DailyWeight;
import com.snhu.weighttracker.Database.DailyWeightDatabase;
import com.snhu.weighttracker.Database.GoalWeightDatabase;
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
    private final MutableLiveData<List<DailyWeight>> dailyWeights;
    private SharedPreferenceManager sharedPreferenceManager;
    private GoalWeightDatabase goalWeightDatabase;
    private DailyWeightDatabase dailyWeightDatabase;


    public HomeViewModel(Context context, SharedViewModel sharedViewModel) {
        goalWeightText = new MutableLiveData<>();
        dailyWeights = new MutableLiveData<>();
        sharedPreferenceManager = new SharedPreferenceManager(context);
        goalWeightDatabase = new GoalWeightDatabase(context);
        dailyWeightDatabase = new DailyWeightDatabase(context);

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

    public LiveData<List<DailyWeight>> getDailyWeights() {
        return dailyWeights;
    }

    private void loadUserData() {
        // if signed in get the goal weight user has saved in database and set it to textView
        if (sharedPreferenceManager.isSignedIn()) {
            int userId = sharedPreferenceManager.getSignedInID();
            Double goalWeight = goalWeightDatabase.getGoalWeight(userId);
            if (goalWeight != null ) {
                goalWeightText.setValue("Goal Weight: " + goalWeight + " lbs");
            } else {
                goalWeightText.setValue("Goal Weight: Not Set");
            }

            List<DailyWeight> weights = dailyWeightDatabase.getAllWeightsForUser(userId);
            dailyWeights.setValue(weights);
        } else {
            goalWeightText.setValue("Not Signed In");
            dailyWeights.setValue(null);
        }
    }

    public void deleteDailyWeight(int id) {
        // remove the daily weight entry for the specific user
        if (dailyWeightDatabase.deleteDailyWeight(id)) {
            loadUserData();
        }
    }

    public boolean isSignedIn(){
        return sharedPreferenceManager.isSignedIn();
    }



}