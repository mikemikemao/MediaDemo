package com.hikvision.mediademo;

import android.Manifest;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.hikvision.cam2.Camera2FrameCallback;
import com.hikvision.cam2.Camera2Wrapper;
import com.hikvision.opengl.MyMediaRender;

import static com.hikvision.jni.MyCam.IMAGE_FORMAT_I420;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/7/7 15:06
 */
public class CamActivity extends AppCompatActivity implements Camera2FrameCallback{
    final static String TAG = "MainActivity";

    protected GLSurfaceView mGLSurfaceView;
    private MyMediaRender myMediaRender;
    private Camera2Wrapper mCamera2Wrapper;
    private RelativeLayout mSurfaceViewRoot;
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

        mGLSurfaceView = new GLSurfaceView(this);
        myMediaRender = new MyMediaRender();
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
    @Override
    protected void onPause() {
        if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            mCamera2Wrapper.stopCamera();
        }
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        myMediaRender.unInit();
    }

    private void initViews() {
        mSurfaceViewRoot = (RelativeLayout) findViewById(R.id.surface_root);
        RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        mSurfaceViewRoot.addView(mGLSurfaceView, p);
        myMediaRender.init(mGLSurfaceView);
        mCamera2Wrapper = new Camera2Wrapper(CamActivity.this);
        mCamera2Wrapper.setDefaultPreviewSize(new Size(1600, 1200));
    }


    @Override
    public void onPreviewFrame(byte[] data, int width, int height) {
        Log.d(TAG, "onPreviewFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]"+ "size = "+ data.length);
        //FileUtils.saveData("/sdcard/video.yuv",data,true);
        myMediaRender.onPreviewFrame(IMAGE_FORMAT_I420, data, width, height);
        myMediaRender.requestRender();
    }

    @Override
    public void onCaptureFrame(byte[] data, int width, int height) {
        Log.d(TAG, "onCaptureFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
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
}
