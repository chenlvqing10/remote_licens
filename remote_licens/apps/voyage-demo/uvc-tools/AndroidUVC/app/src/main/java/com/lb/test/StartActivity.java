package com.lb.test;

import android.content.Intent;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class StartActivity extends AppCompatActivity {
    private static final String TAG = StartActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        int width = metrics.widthPixels;
        int height = metrics.heightPixels;
        Log.e(TAG, "onCreate: width: " + width + ", height: " + height);
    }

    public void uvcopen(View view){
        Intent intent = new Intent(this, UVCCameraActivity.class);
        startActivity(intent);
    }

    public void androidopen(View view){
        Intent intent = new Intent(this, AndroidCameraActivity.class);
        startActivity(intent);
    }
}
