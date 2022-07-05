package com.hikvision.mediademo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.hikvision.jni.MyCam;

public class MainActivity extends AppCompatActivity {
    final static String TAG = "MainActivity";

    SurfaceView surfaceView;
    SurfaceHolder surfaceHolder;
    MyCam myCam;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        myCam = new MyCam();
        surfaceView = (SurfaceView) findViewById(R.id.surfaceview);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.v(TAG, "surface created.");
                myCam.startPreview(holder.getSurface());
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, "format=" + format + " w/h : (" + width + ", " + height + ")");
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                myCam.stopPreview();
            }
        });
    }

    @Override
    protected void onDestroy() {
        myCam.stopPreview();
        super.onDestroy();
    }


}