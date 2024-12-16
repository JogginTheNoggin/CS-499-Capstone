package com.snhu.weighttracker.ui.login;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.snhu.weighttracker.Database.SharedPreferenceManager;
import com.snhu.weighttracker.Database.UserDatabase;

public class LoginViewModel extends ViewModel {

    private UserDatabase userDatabase;
    private final SharedPreferenceManager sharedPreferenceManager;
    private final MutableLiveData<String> mText;
    private final MutableLiveData<Boolean> isRegistering;
    private final MutableLiveData<String> username;
    private final MutableLiveData<String> email;
    private final MutableLiveData<String> password;
    private final MutableLiveData<String> confirmPassword;
    private final MutableLiveData<String> toastMessage;

    public LoginViewModel(Context context) {
        userDatabase = new UserDatabase(context);

        sharedPreferenceManager = new SharedPreferenceManager(context);
        mText = new MutableLiveData<>();
        // mText.setValue("Login or Create an Account");

        isRegistering = new MutableLiveData<>(false);
        username = new MutableLiveData<>();
        email = new MutableLiveData<>();
        password = new MutableLiveData<>();
        confirmPassword = new MutableLiveData<>();
        toastMessage = new MutableLiveData<>();
    }

    public LiveData<String> getText() {
        return mText;
    }

    public LiveData<Boolean> getIsRegistering() {
        return isRegistering;
    }

    public LiveData<String> getUsername() {
        return username;
    }

    public LiveData<String> getEmail() {
        return email;
    }

    public LiveData<String> getPassword() {
        return password;
    }

    public LiveData<String> getConfirmPassword() {
        return confirmPassword;
    }

    public LiveData<String> getToastMessage() {
        return toastMessage;
    }

    public void setUsername(String username) {
        this.username.setValue(username);
    }

    public void setEmail(String email) {
        this.email.setValue(email);
    }

    public void setPassword(String password) {
        this.password.setValue(password);
    }

    public void setConfirmPassword(String confirmPassword) {
        this.confirmPassword.setValue(confirmPassword);
    }

    public void toggleRegistering() {
        isRegistering.setValue(!Boolean.TRUE.equals(isRegistering.getValue()));
    }




    public boolean register() {
        String usernameValue = username.getValue();
        String emailValue = email.getValue();
        String passwordValue = password.getValue();
        String confirmPasswordValue = confirmPassword.getValue();

        if (usernameValue == null || usernameValue.isEmpty() ||
                emailValue == null || emailValue.isEmpty() ||
                passwordValue == null || passwordValue.isEmpty() ||
                confirmPasswordValue == null || confirmPasswordValue.isEmpty()) {
            toastMessage.setValue("All entries required");
            return false;
        }

        if (!passwordValue.equals(confirmPasswordValue)) {
            toastMessage.setValue("Passwords do not match");
            return false;
        }

        if (userDatabase.isUsernameExists(usernameValue)) {
            toastMessage.setValue("Username already exists");
            return false;
        }


        if(userDatabase.addUser(usernameValue, emailValue, confirmPasswordValue)){
            saveUserStatus(usernameValue); // save the status as logged in
            toastMessage.setValue("Registration successful");
        }
        else{
            toastMessage.setValue("Registration Failed");
        }

        return true;
    }

    public boolean login() {
        String usernameValue = username.getValue();
        String passwordValue = password.getValue();

        if (usernameValue == null || usernameValue.isEmpty() ||
                passwordValue == null || passwordValue.isEmpty()) {
            toastMessage.setValue("Username and Password are required");
            return false;
        }

        if (!userDatabase.validateUser(usernameValue, passwordValue)) {
            toastMessage.setValue("Invalid username or password");
            return false;
        }

        saveUserStatus(usernameValue); // save the status as logged in
        toastMessage.setValue("Login successful");

        return true;
    }


    private void saveUserStatus(String username) {
        int userId = userDatabase.getUserIdByUsername(username);
        if(userId != -1) { // only change if user has valid id
            sharedPreferenceManager.saveSignIn(userId);
            boolean smsEnabled = userDatabase.getSmsStatus(userId);
            sharedPreferenceManager.setSmsEnabled(smsEnabled);
        }
    }

    public int getUserId() {
        return sharedPreferenceManager.getSignedInID();
    }
}