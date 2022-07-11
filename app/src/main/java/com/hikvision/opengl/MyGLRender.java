package com.hikvision.opengl;

import android.opengl.GLSurfaceView;
import android.util.Log;
import com.hikvision.jni.MyCam;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;



/**
 * description ： TODO:类的作用
 * author : pirate
 * param
 * date : 2022/3/12 13:50
 */
public class MyGLRender implements GLSurfaceView.Renderer {
    private static final String TAG = "MyGLRender";
    private MyCam myCam;
    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;
    public static final int IMAGE_FORMAT_YUYV = 0x05;
    public static final int IMAGE_FORMAT_GARY = 0x06;

    public MyGLRender(){
        myCam = new MyCam();
    }
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        myCam.native_surfaceCreated();
        Log.e(TAG, "onSurfaceCreated() called with: GL_VERSION = [" + gl.glGetString(GL10.GL_VERSION) + "]");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        myCam.native_surfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        myCam.native_drawFrame();
    }


    public void setImageData(int format, int width, int height, byte[] bytes) {
        myCam.native_setImageData(format,bytes, width, height);
    }

}
