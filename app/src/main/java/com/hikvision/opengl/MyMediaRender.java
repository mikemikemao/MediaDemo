package com.hikvision.opengl;

import android.opengl.GLSurfaceView;
import android.util.Log;

import com.hikvision.jni.MyCam;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyMediaRender extends MyCam implements GLSurfaceView.Renderer {
    private static final String TAG = "MyMediaRender";
    private GLSurfaceView mGLSurfaceView;

    public static final int RECORDER_TYPE_SINGLE_VIDEO   = 0; //仅录制视频
    public static final int RECORDER_TYPE_SINGLE_AUDIO   = 1; //仅录制音频
    public static final int RECORDER_TYPE_AV             = 2; //同时录制音频和视频,打包成 MP4 文件

    public void init(GLSurfaceView surfaceView,int renderType) { //for Video
        mGLSurfaceView = surfaceView;
        mGLSurfaceView.setEGLContextClientVersion(2);
        mGLSurfaceView.setRenderer(this);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        native_CreateContext(renderType);
        native_Init();
    }
    public void unInit() {
        native_UnInit();
        native_DestroyContext();
    }


    public void requestRender() {
        if (mGLSurfaceView != null) {
            mGLSurfaceView.requestRender();
        }
    }

    public void onPreviewFrame(int format, byte[] data, int width, int height) {
        //Log.d(TAG, "onPreviewFrame() called with: data = [" + data + "], width = [" + width + "], height = [" + height + "]");
        native_OnPreviewFrame(format, data, width, height);
    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d(TAG, "onSurfaceCreated() called with: gl = [" + gl + "], config = [" + config + "]");
        native_OnSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.d(TAG, "onSurfaceChanged() called with: gl = [" + gl + "], width = [" + width + "], height = [" + height + "]");
        native_OnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        Log.d(TAG, "onDrawFrame() called with: gl = [" + gl + "]");
        native_OnDrawFrame();
    }

    //编码
    public void startRecord(int recorderType, String outUrl, int frameWidth, int frameHeight, long videoBitRate, int fps) {
        Log.d(TAG, "startRecord() called with: recorderType = [" + recorderType + "], outUrl = [" + outUrl + "], frameWidth = [" + frameWidth + "], frameHeight = [" + frameHeight + "], videoBitRate = [" + videoBitRate + "], fps = [" + fps + "]");
        native_StartRecord(recorderType, outUrl, frameWidth, frameHeight, videoBitRate, fps);
    }


}
