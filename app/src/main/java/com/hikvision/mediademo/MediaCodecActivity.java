package com.hikvision.mediademo;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.hikvision.jni.MyCam;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/7/26 17:07
 */
public class MediaCodecActivity extends AppCompatActivity {
    static final String TAG = "MyMediaCodec";
    SurfaceView mySurfaceView;
    SurfaceHolder mySurfaceHolder;
    MyCam myCam;
    String fileName = "testfile.mp4";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mediacodec);
        myCam = new MyCam();
        mySurfaceView = (SurfaceView) findViewById(R.id.mySurfaceView);
        mySurfaceHolder = mySurfaceView.getHolder();
        mySurfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.v(TAG, "surfaceCreated");
                myCam.native_SetSurface(holder.getSurface());
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, "surfaceChanged format=" + format + ", width=" + width + ", height="
                        + height);
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                Log.v(TAG, "surfaceDestroyed");
            }
        });


    }

    public void onClickCreateResource(View view) {
        //编解码做好准备
        myCam.native_CreateStreamingMediaPlayer(getResources().getAssets(),fileName);
    }
}
