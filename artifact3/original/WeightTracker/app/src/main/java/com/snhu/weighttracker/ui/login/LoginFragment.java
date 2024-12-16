package com.snhu.weighttracker.ui.login;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;

import com.snhu.weighttracker.Database.SharedViewModel;
import com.snhu.weighttracker.Database.UserDatabase;
import com.snhu.weighttracker.R;
import com.snhu.weighttracker.databinding.FragmentLoginBinding;

public class LoginFragment extends Fragment {


    private LoginViewModel loginViewModel;
    private SharedViewModel sharedViewModel;
    private FragmentLoginBinding binding;

    private EditText emailEditText;
    private EditText usernameEditText;
    private EditText passwordEditText;
    private EditText confirmPasswordEditText;
    private Button loginButton;
    private Button registerButton;
    private Button cancelButton;
    private TableRow emailRow;
    private TableRow confirmPasswordRow;




    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {

        LoginViewModelFactory factory = new LoginViewModelFactory(requireContext()); // used to provide context to loginViewModel
        loginViewModel = new ViewModelProvider(this, factory).get(LoginViewModel.class);
        sharedViewModel = new ViewModelProvider(requireActivity()).get(SharedViewModel.class);

        binding = FragmentLoginBinding.inflate(inflater, container, false);
        View root = binding.getRoot();



        final TextView textView = binding.textLogin;
        loginViewModel.getText().observe(getViewLifecycleOwner(), textView::setText);

        // update view based on if logging in or registering
        loginViewModel.getIsRegistering().observe(getViewLifecycleOwner(), isRegistering -> updateRegisterViewVisibility());
        loginViewModel.getToastMessage().observe(getViewLifecycleOwner(), message -> Toast.makeText(getContext(), message, Toast.LENGTH_SHORT).show());

        init(root);

        return root;
    }


    public void init(View root){
        usernameEditText = root.findViewById(R.id.editTextText2);
        emailEditText = root.findViewById(R.id.editTextText5);
        passwordEditText = root.findViewById(R.id.editTextText3);
        confirmPasswordEditText = root.findViewById(R.id.editTextText4);
        loginButton = root.findViewById(R.id.button3);
        registerButton = root.findViewById(R.id.button4);
        cancelButton = root.findViewById(R.id.button5);
        emailRow = (TableRow) root.findViewById(R.id.textView7).getParent();
        confirmPasswordRow = (TableRow) root.findViewById(R.id.textView6).getParent();


        loginButton.setOnClickListener(v -> {
            // Update ViewModel with current field values
            loginViewModel.setUsername(usernameEditText.getText().toString().trim());
            loginViewModel.setPassword(passwordEditText.getText().toString().trim());

            // use login model to attempt to login
            if (loginViewModel.login()) {
                sharedViewModel.notifyDataChanged(); // Notify data change
                navigateToHome(); // go back to home fragment
            }
        });

        registerButton.setOnClickListener(v -> {
            if (Boolean.TRUE.equals(loginViewModel.getIsRegistering().getValue())) {
                // assign values for login model in order to register user
                loginViewModel.setUsername(usernameEditText.getText().toString().trim());
                loginViewModel.setEmail(emailEditText.getText().toString().trim());
                loginViewModel.setPassword(passwordEditText.getText().toString().trim());
                loginViewModel.setConfirmPassword(confirmPasswordEditText.getText().toString().trim());
                // attempt to register user
                if (loginViewModel.register()) {
                    sharedViewModel.notifyDataChanged(); // Notify data change
                    navigateToHome(); // go back to home fragment
                }
            } else {
                loginViewModel.toggleRegistering();
                updateRegisterViewVisibility();
            }
        });

        cancelButton.setOnClickListener(v -> {
            loginViewModel.toggleRegistering();
            updateRegisterViewVisibility();
        });

    }

    private void updateRegisterViewVisibility() {
        boolean isRegistering = Boolean.TRUE.equals(loginViewModel.getIsRegistering().getValue());
        emailRow.setVisibility(isRegistering ? View.VISIBLE : View.GONE);
        confirmPasswordRow.setVisibility(isRegistering ? View.VISIBLE : View.GONE);
        loginButton.setVisibility(isRegistering ? View.GONE : View.VISIBLE);
        cancelButton.setVisibility(isRegistering ? View.VISIBLE : View.GONE);
    }


    private void navigateToHome() {
        NavController navController = Navigation.findNavController(requireActivity(), R.id.nav_host_fragment_content_main);
        navController.navigateUp(); // This will navigate back to the previous fragment
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}