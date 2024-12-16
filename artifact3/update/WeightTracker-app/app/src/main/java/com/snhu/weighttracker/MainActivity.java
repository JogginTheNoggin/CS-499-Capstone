package com.snhu.weighttracker;

import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.view.Menu;

import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.navigation.NavigationView;

import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.appcompat.app.AppCompatActivity;

import com.snhu.weighttracker.Database.SharedPreferenceManager;
import com.snhu.weighttracker.Database.SharedViewModel;
import com.snhu.weighttracker.databinding.ActivityMainBinding;
import com.snhu.weighttracker.ui.WeightDialogFragment;

public class MainActivity extends AppCompatActivity {

    private AppBarConfiguration mAppBarConfiguration;
    private ActivityMainBinding binding;

    NavigationView navigationView;


    private SharedViewModel sharedViewModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SharedPreferenceManager.init(this.getBaseContext());
        sharedViewModel = new ViewModelProvider(this).get(SharedViewModel.class); // for tracking changes between fragments


        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // set the top menu bar
        setSupportActionBar(binding.appBarMain.toolbar);
        binding.appBarMain.fab.setOnClickListener(view -> showDialog());

        DrawerLayout drawer = binding.drawerLayout;
        navigationView = binding.navView;

        // Passing each menu ID as a set of Ids because each menu should be considered as top level destinations.
        mAppBarConfiguration = new AppBarConfiguration.Builder(
                R.id.nav_home, R.id.nav_login, R.id.nav_logout, R.id.nav_notification)
                .setOpenableLayout(drawer)
                .build();
        // assign how fragments to be navigated via side menu
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        NavigationUI.setupActionBarWithNavController(this, navController, mAppBarConfiguration);
        NavigationUI.setupWithNavController(navigationView, navController); //

        // set listener for side menu
        navigationView.setNavigationItemSelectedListener(item -> {
            int id = item.getItemId();
            if (id == R.id.nav_logout) {
                SharedPreferenceManager.signOut();
                sharedViewModel.notifyDataChanged(); // no longer signed in, so chart should reflect this
                updateNavigationViewMenu(navigationView);
                return true;
            }
            return NavigationUI.onNavDestinationSelected(item, navController) || super.onOptionsItemSelected(item);
        });

        // Observe the SharedViewModel for changes, so views can be updated
        // doesn't quite work for login -> logout
        sharedViewModel.getDataChanged().observe(this, dataChanged -> {
            if (dataChanged) {
                updateNavigationViewMenu(navigationView);
                sharedViewModel.resetDataChanged();
            }
        });

    }


    // does not get called
    @Override
    protected void onResume(){
        super.onResume();
        Log.d("LIFECYCLE", "onResume: Updating navigation view menu");
        updateNavigationViewMenu(binding.navView);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        return NavigationUI.navigateUp(navController, mAppBarConfiguration)
                || super.onSupportNavigateUp();
    }

    private void showDialog() {
        // open dialog for adding daily weight and goal weight
        WeightDialogFragment dialogFragment = new WeightDialogFragment();
        dialogFragment.show(getSupportFragmentManager(), "WeightDialogFragment");
    }

    private void updateNavigationViewMenu(NavigationView navigationView) {
        MenuItem loginMenuItem = navigationView.getMenu().findItem(R.id.nav_login);
        MenuItem logoutMenuItem = navigationView.getMenu().findItem(R.id.nav_logout);

        // set view of side navigation if signed in or not
        if (SharedPreferenceManager.isSignedIn()) {
            Log.w("Sign In: ", "true");
            loginMenuItem.setVisible(false);
            logoutMenuItem.setVisible(true);
        } else {
            Log.w("Sign In: ", "false");
            loginMenuItem.setVisible(true);
            logoutMenuItem.setVisible(false);
        }
    }
}