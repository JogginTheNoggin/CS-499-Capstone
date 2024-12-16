package com.snhu.weighttracker.ui;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.DialogFragment;
import androidx.lifecycle.ViewModelProvider;

import com.snhu.weighttracker.Database.DailyWeightDatabase;
import com.snhu.weighttracker.Database.GoalWeightDatabase;
import com.snhu.weighttracker.Database.SharedPreferenceManager;
import com.snhu.weighttracker.Database.SharedViewModel;
import com.snhu.weighttracker.R;

import java.util.Date;
import java.util.Locale;
import java.text.SimpleDateFormat;

public class WeightDialogFragment extends DialogFragment {
    private static final String phoneNumber = "15551234567"; // phone number for device, found in: settings, about emulator, if need to change
    private EditText editTextWeight;
    private EditText goalWeightField;
    private SharedPreferenceManager sharedPreferenceManager;
    private DailyWeightDatabase dailyWeightDatabase;
    private GoalWeightDatabase goalWeightDatabase;
    private SharedViewModel sharedViewModel;
    private static final double MINIMUM_WEIGHT = 60.0;
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_add_weight, container, false);

        editTextWeight = view.findViewById(R.id.editTextText);
        goalWeightField = view.findViewById(R.id.goalWeightField);
        Button submitButton = view.findViewById(R.id.submitButton);

        Context context = getContext();
        if (context != null) {
            sharedPreferenceManager = new SharedPreferenceManager(context);
            dailyWeightDatabase = new DailyWeightDatabase(context);
            goalWeightDatabase = new GoalWeightDatabase(context);
            sharedViewModel = new ViewModelProvider(requireActivity()).get(SharedViewModel.class);
        }

        submitButton.setOnClickListener(v -> submitWeight());
        return view;
    }

    @Override
    public void onStart() {
        super.onStart();
        if (getDialog() != null && getDialog().getWindow() != null) {
            getDialog().getWindow().setLayout(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
            getDialog().getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        }
    }

    private void submitWeight() {
        String weightInput = editTextWeight.getText().toString();
        String goalWeightInput = goalWeightField.getText().toString();

        if(!sharedPreferenceManager.isSignedIn() || sharedPreferenceManager.getSignedInID() < 0){ // check if signed in, if not , do not process
            return;
        }

        // see if the text entry contains any values
        if (!TextUtils.isEmpty(weightInput)) {
            double weight = Double.parseDouble(weightInput);
            if (weight > MINIMUM_WEIGHT ) { // make sure a valid weight entered
                int userId = sharedPreferenceManager.getSignedInID();

                String date = new SimpleDateFormat("yyyy-MM-dd", Locale.getDefault()).format(new Date()); // get current date

                boolean success = dailyWeightDatabase.addDailyWeight(userId, weight, date); // attempt to submit new daily weight to database
                if (success) {
                    sharedViewModel.notifyDataChanged(); // notify other fragments to update views
                    Toast.makeText(getContext(), "Weight added successfully", Toast.LENGTH_SHORT).show();

                    // Check if SMS is enabled and weight equals goal weight
                    if (sharedPreferenceManager.isSmsEnabled()) {
                        double goalWeight = goalWeightDatabase.getGoalWeight(userId);
                        //Log.d("SMS STATUS:", "Enabled weight: " + weight + " goal weight: " + goalWeight);
                        if (goalWeight == weight) {
                            sendSmsNotification(); // send of notification
                        }
                    }
                } else {
                    Toast.makeText(getContext(), "Failed to add weight", Toast.LENGTH_SHORT).show();


                }

                dismiss();
            } else {
                Toast.makeText(getContext(), "Please enter a weight greater than 0", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(getContext(), "Please enter a weight", Toast.LENGTH_SHORT).show();
        }

        // process goal weight text field if contains values
        if (!TextUtils.isEmpty(goalWeightInput)) {
            double goalWeight = Double.parseDouble(goalWeightInput);
            if(goalWeight >= MINIMUM_WEIGHT) { // make sure valid value
                int userId = sharedPreferenceManager.getSignedInID();

                // attempt to update exist or create new goal weight for signed in user
                boolean goalSuccess = goalWeightDatabase.setGoalWeight(userId, goalWeight);
                if (goalSuccess) {
                    sharedViewModel.notifyDataChanged(); // notify other fragments to update their views
                    Toast.makeText(getContext(), "Goal weight set successfully", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(getContext(), "Failed to set goal weight", Toast.LENGTH_SHORT).show();
                }
            }
        } else {
            Toast.makeText(getContext(), "Please enter a goal weight", Toast.LENGTH_SHORT).show();
        }
    }



    private void sendSmsNotification() {

        String message = "Congratulations! You've reached your goal weight.";

        SmsManager smsManager = SmsManager.getDefault();
        smsManager.sendTextMessage(phoneNumber, null, message, null, null);

        Toast.makeText(getContext(), "Goal weight reached! SMS sent.", Toast.LENGTH_SHORT).show();

    }

}
