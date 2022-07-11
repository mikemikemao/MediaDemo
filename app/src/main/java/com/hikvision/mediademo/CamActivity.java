package com.hikvision.mediademo;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.hikvision.cam2.Camera2FrameCallback;
import com.hikvision.cam2.Camera2Wrapper;
import com.hikvision.jni.MediaRendor;
import com.hikvision.jni.MyCam;
import com.hikvision.util.FileUtils;

import static com.hikvision.jni.MediaRendor.IMAGE_FORMAT_I420;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/7/7 15:06
 */
public class CamActivity extends AppCompatActivity implements Camera2FrameCallback {
    final static String TAG = "MainActivity";

    SurfaceView surfaceView;
    SurfaceHolder surfaceHolder;

    MediaRendor mediaRendor;
    private Camera2Wrapper mCamera2Wrapper;
    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 1;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_cam);
        mediaRendor = new MediaRendor();
        initViews();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            mCamera2Wrapper.startCamera();
        } else {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, CAMERA_PERMISSION_REQUEST_CODE);
        }
    }

    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        mCamera2Wrapper.stopCamera();
        super.onPause();
    }

    private void initViews() {
        mCamera2Wrapper = new Camera2Wrapper(CamActivity.this);
        mCamera2Wrapper.setDefaultPreviewSize(new Size(1600, 1200));
    }


    @Override
    public void onPreviewFrame(byte[] data, int width, int height) {
        Log.d(TAG, "onPreviewFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]"+ "size = "+ data.length);
        //FileUtils.saveData("/sdcard/video.yuv",data,true);
        //mediaRendor.onPreviewFrame(IMAGE_FORMAT_I420, data, width, height);
    }

    @Override
    public void onCaptureFrame(byte[] data, int width, int height) {
        Log.d(TAG, "onCaptureFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
    }
}