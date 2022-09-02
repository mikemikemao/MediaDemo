package com.hikvision.mediademo;

import android.Manifest;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.util.Size;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.hikvision.cam2.Camera2FrameCallback;
import com.hikvision.cam2.Camera2Wrapper;
import com.hikvision.opengl.MyMediaRender;
import com.hikvision.util.FileUtils;

import static com.hikvision.jni.MyCam.IMAGE_FORMAT_I420;
import static com.hikvision.opengl.MyMediaRender.RECORDER_TYPE_SINGLE_VIDEO;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.GregorianCalendar;
import java.util.Locale;

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
    private String mOutUrl;
    private static final String RESULT_IMG_DIR = "mediaDemo";
    private static final SimpleDateFormat DateTime_FORMAT = new SimpleDateFormat("yyyyMMddHHmmss", Locale.US);

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
        mGLSurfaceView = (GLSurfaceView)findViewById(R.id.glSurfaceView);
        myMediaRender.init(mGLSurfaceView,0);
        mCamera2Wrapper = new Camera2Wrapper(CamActivity.this);
        mCamera2Wrapper.setDefaultPreviewSize(new Size(1600, 1200));
    }


    @Override
    public void onPreviewFrame(byte[] data, int width, int height) {
        //Log.d(TAG, "onPreviewFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]"+ "size = "+ data.length);
        //FileUtils.saveData("/sdcard/video.yuv",data,false);
        myMediaRender.onPreviewFrame(IMAGE_FORMAT_I420, data, width, height);
        myMediaRender.requestRender();
    }

    public static final File getOutFile(final String ext) {
        final File dir = new File(Environment.getExternalStorageDirectory(), RESULT_IMG_DIR);
        Log.d(TAG, "path=" + dir.toString());
        dir.mkdirs();
        if (dir.canWrite()) {
            return new File(dir, "video_" + getDateTimeString() + ext);
        }
        return null;
    }

    private static final String getDateTimeString() {
        final GregorianCalendar now = new GregorianCalendar();
        return DateTime_FORMAT.format(now.getTime());
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

    public void onClickStartRecord(View view) {
        //进行编码
        int frameWidth = mCamera2Wrapper.getPreviewSize().getWidth();
        int frameHeight = mCamera2Wrapper.getPreviewSize().getHeight();
        int fps = 25;
        int bitRate = (int) (frameWidth * frameHeight * fps * 0.25);
        mOutUrl = getOutFile(".mp4").getAbsolutePath();
        myMediaRender.startRecord(RECORDER_TYPE_SINGLE_VIDEO, mOutUrl, frameWidth, frameHeight, bitRate, fps);
    }
}
