package com.snhu.weighttracker.Database;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

/*
* Fragments do not seem to call onResume when changing between them
* As a result this class is used to allow various fragments to listen
* for data updates, and call the appropriate methods the fragment needs to
* reflect updates
* */
public class SharedViewModel extends ViewModel {



    private final MutableLiveData<Boolean> dataChanged;

    public SharedViewModel() {
        dataChanged = new MutableLiveData<>();
    }

    public LiveData<Boolean> getDataChanged() {

        return dataChanged;
    }

    public void notifyDataChanged() {
        dataChanged.setValue(true);
    }

    public void resetDataChanged() {
        dataChanged.setValue(false);
    }

}

