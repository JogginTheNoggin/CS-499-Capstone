package com.snhu.weighttracker.ui.notifications;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.SwitchCompat;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;


import com.google.android.material.snackbar.Snackbar;
import com.snhu.weighttracker.Database.SharedViewModel;
import com.snhu.weighttracker.databinding.FragmentNotificationsBinding;

public class NotificationFragment extends Fragment {

    private FragmentNotificationsBinding binding;
    private NotificationViewModel notificationViewModel;
    private SharedViewModel sharedViewModel;
    private static final int REQUEST_SEND_SMS = 1;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        NotificationViewModel.Factory factory = new NotificationViewModel.Factory(requireContext());
        notificationViewModel = new ViewModelProvider(this, factory).get(NotificationViewModel.class);
        sharedViewModel = new ViewModelProvider(requireActivity()).get(SharedViewModel.class);

        binding = FragmentNotificationsBinding.inflate(inflater, container, false);
        View root = binding.getRoot();

        final TextView textView = binding.textNotifications;
        notificationViewModel.getText().observe(getViewLifecycleOwner(), textView::setText);

        // Initialize the switches based on shared preferences
        initSwitches();

        return root;
    }

    private void initSwitches() {
        SwitchCompat goalWeightSwitch = binding.switchGoalWeightReached;

        // Set initial state based on shared preferences
        goalWeightSwitch.setChecked(notificationViewModel.isSmsEnabled());

        // Set listener for the goal weight switch
        goalWeightSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                Log.d("SMS CHECK" , "Checked selected, sms to be enabled");
                checkForSmsPermission();
            } else {
                notificationViewModel.updateSmsStatus(false);
                sharedViewModel.notifyDataChanged();
            }
        });

        // Observe SMS permission changes
        notificationViewModel.getSmsPermissionGranted().observe(getViewLifecycleOwner(), granted -> {
            if (granted != null && granted) {
                notificationViewModel.updateSmsStatus(true);
                sharedViewModel.notifyDataChanged();
            } else {
                goalWeightSwitch.setChecked(false);
                Snackbar.make(binding.getRoot(),
                        "Permission denied. Unable to send SMS notifications.",
                        Snackbar.LENGTH_LONG).show();
            }
        });

        // Observe SMS status updates
        notificationViewModel.getSmsStatusUpdated().observe(getViewLifecycleOwner(), updated -> {
            if (updated != null && updated) {
                Toast.makeText(getContext(), "SMS notification status updated", Toast.LENGTH_SHORT).show();
            } else if (updated != null) {
                Toast.makeText(getContext(), "Failed to update SMS status", Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void checkForSmsPermission() {
        if (ContextCompat.checkSelfPermission(requireContext(), Manifest.permission.SEND_SMS)
                != PackageManager.PERMISSION_GRANTED) {

            if (ActivityCompat.shouldShowRequestPermissionRationale(requireActivity(),
                    Manifest.permission.SEND_SMS)) {
                // Show an explanation to the user *asynchronously* -- don't block
                // this thread waiting for the user's response! After the user
                // sees the explanation, try again to request the permission.
                Snackbar.make(binding.getRoot(),
                                "SMS permission is needed to send notifications for upcoming events and goal weight reached.",
                                Snackbar.LENGTH_INDEFINITE)
                        .setAction("OK", new View.OnClickListener() {
                            @Override
                            public void onClick(View view) {
                                // Request the permission
                                requestSmsPermission();
                            }
                        }).show();
            } else {
                // No explanation needed; request the permission
                requestSmsPermission();
            }
        } else {
            // Permission has already been granted
            // proceed with SMS sending logic here if needed
            notificationViewModel.updateSmsStatus(true);
            sharedViewModel.notifyDataChanged();
        }
    }

    private void requestSmsPermission() {
        ActivityCompat.requestPermissions(requireActivity(),
                new String[]{Manifest.permission.SEND_SMS},
                REQUEST_SEND_SMS);
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_SEND_SMS) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // Permission granted
                notificationViewModel.updateSmsStatus(true);
                sharedViewModel.notifyDataChanged();
            } else {
                // Permission denied
                // Handle the case where permission is not granted
                binding.switchGoalWeightReached.setChecked(false);
                Snackbar.make(binding.getRoot(),
                                "Permission denied. Unable to send SMS notifications.",
                                Snackbar.LENGTH_LONG)
                        .show();
            }
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}