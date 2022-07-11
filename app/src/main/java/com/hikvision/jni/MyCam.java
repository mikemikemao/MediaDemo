package com.hikvision.jni;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.Surface;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * description ： camera operation
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/7/5 19:06
 */
public class MyCam {
    static {
        System.loadLibrary("learn-dsp");
    }
    public static final int IMAGE_FORMAT_I420 = 0x04;
    private static String TAG = "MyCam";

    /**
     * description startPreview
     * param surface
     * @return
     */
    public static native void native_startPreview(Surface surface);

    /**
     * description stopPreview
     * param
     * @return
     */
    public static native void native_stopPreview();

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_setImageData(int format, byte[] data, int width, int height);

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_surfaceCreated();

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_surfaceChanged(int width, int height);

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_drawFrame();
}
