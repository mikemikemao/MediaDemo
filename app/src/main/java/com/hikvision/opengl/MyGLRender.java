package com.hikvision.ffmpegdemo.opengl;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.util.Log;

import com.hikvision.ffmpegdemo.ZZFFmpeg;
import com.hikvision.ffmpegdemo.ZZOpenglNative;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static com.hikvision.ffmpegdemo.ZZOpenglNative.SAMPLE_TYPE;

/**
 * description ： TODO:类的作用
 * author : pirate
 * param
 * date : 2022/3/12 13:50
 */
public class MyGLRender implements GLSurfaceView.Renderer {
    private static final String TAG = "MyGLRender";
    private ZZOpenglNative mNativeRender;
    private int mSampleType;
    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;
    public static final int IMAGE_FORMAT_YUYV = 0x05;
    public static final int IMAGE_FORMAT_GARY = 0x06;
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mNativeRender.native_OnSurfaceCreated();
        Log.e(TAG, "onSurfaceCreated() called with: GL_VERSION = [" + gl.glGetString(GL10.GL_VERSION) + "]");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mNativeRender.native_OnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        mNativeRender.native_OnDrawFrame();
    }

    public void setParamsInt(int paramType, int value0, int value1) {
        if (paramType == SAMPLE_TYPE) {
            mSampleType = value0;
        }
        mNativeRender.native_SetParamsInt(paramType, value0, value1);
    }
    public int getSampleType() {
        return mSampleType;
    }

    public void setImageData(int format, int width, int height, byte[] bytes) {
        mNativeRender.native_SetImageData(format, width, height, bytes);
    }

}
