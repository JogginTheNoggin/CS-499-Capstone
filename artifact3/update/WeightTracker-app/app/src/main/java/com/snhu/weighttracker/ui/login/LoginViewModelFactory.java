package com.snhu.weighttracker.ui.login;

import android.content.Context;
import androidx.annotation.NonNull;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;


// used to pass context to view model of login fragment
public class LoginViewModelFactory implements ViewModelProvider.Factory {
    private final Context context;

    public LoginViewModelFactory(Context context) {
        this.context = context.getApplicationContext(); // Use application context to avoid memory leaks
    }

    @NonNull
    @Override
    public <T extends ViewModel> T create(@NonNull Class<T> modelClass) {
        if (modelClass.isAssignableFrom(LoginViewModel.class)) {
            return (T) new LoginViewModel(context);
        }
        throw new IllegalArgumentException("Unknown ViewModel class");
    }
}
