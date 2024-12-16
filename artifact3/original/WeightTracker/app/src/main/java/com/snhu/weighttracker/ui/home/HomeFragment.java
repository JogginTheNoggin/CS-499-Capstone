package com.snhu.weighttracker.ui.home;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.snhu.weighttracker.Database.DailyWeight;
import com.snhu.weighttracker.Database.SharedViewModel;
import com.snhu.weighttracker.R;
import com.snhu.weighttracker.databinding.FragmentHomeBinding;
import com.snhu.weighttracker.ui.WeightDialogFragment;

import java.util.ArrayList;
import java.util.List;

public class HomeFragment extends Fragment {

    private TextView goalWeightTextView;
    private LineChart lineChart;
    private FragmentHomeBinding binding;

    private HomeViewModel homeViewModel;

    private SharedViewModel sharedViewModel;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        sharedViewModel = new ViewModelProvider(requireActivity()).get(SharedViewModel.class);
        HomeViewModel.Factory factory = new HomeViewModel.Factory(requireContext(), sharedViewModel); // used to pass the context and other values to homeviewmodel
        homeViewModel = new ViewModelProvider(this, factory).get(HomeViewModel.class); // the V of MVC pattern

        binding = FragmentHomeBinding.inflate(inflater, container, false);
        View root = binding.getRoot();


        // final TextView textView = binding.textHome;
        // homeViewModel.getText().observe(getViewLifecycleOwner(), textView::setText);

        return root;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        goalWeightTextView = view.findViewById(R.id.goal_weight);
        final TableLayout tableLayout = view.findViewById(R.id.tableLayout);
        lineChart = view.findViewById(R.id.lineChart);

        //tempInit();
        // check for updates to data, like listener
        homeViewModel.getGoalWeightText().observe(getViewLifecycleOwner(), goalWeightTextView::setText);

        // check for updates to weight values. like listener
        homeViewModel.getDailyWeights().observe(getViewLifecycleOwner(), weights -> {
            updateChart(weights);
            updateTable(weights);

        });

    }



    private void updateChart(List<DailyWeight> weights) {


        List<Entry> entries = new ArrayList<>();
        if(weights != null) {

            // Create line data entry for each weight
            for (int i = 0; i < weights.size(); i++) {
                DailyWeight weight = weights.get(i);
                entries.add(new Entry(i, (float) weight.getWeight()));
            }
        }
        LineDataSet dataSet = new LineDataSet(entries, "Weight over Time");
        dataSet.setAxisDependency(YAxis.AxisDependency.LEFT);
        LineData lineData = new LineData(dataSet);
        lineChart.setData(lineData);
        lineChart.getXAxis().setPosition(XAxis.XAxisPosition.BOTTOM);
        lineChart.getAxisRight().setEnabled(false);
        lineChart.invalidate(); // Refresh the chart
    }


    private void updateTable(List<DailyWeight> weights) {
        TableLayout tableLayout = getView().findViewById(R.id.tableLayout);
        tableLayout.removeViews(1, tableLayout.getChildCount() - 1); // Clear existing rows except the header
        if(!homeViewModel.isSignedIn()){
            return; // do not display data
        }
        // for each weight create table row for entry
        for (DailyWeight weight : weights) {
            TableRow row = new TableRow(getContext());

            TextView timeTextView = new TextView(getContext());
            timeTextView.setText(weight.getDate());
            timeTextView.setPadding(8, 8, 8, 8);

            TextView weightTextView = new TextView(getContext());
            weightTextView.setText(String.valueOf(weight.getWeight()) + " lbs");
            weightTextView.setPadding(8, 8, 8, 8);

            ImageButton deleteButton = new ImageButton(getContext());
            deleteButton.setImageResource(R.drawable.clear_24);
            deleteButton.setBackground(null);
            deleteButton.setOnClickListener(v -> {
                // remove the weight entry from the database
                homeViewModel.deleteDailyWeight(weight.getId());
            });

            row.addView(timeTextView);
            row.addView(weightTextView);
            row.addView(deleteButton);

            tableLayout.addView(row);
        }
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }


}