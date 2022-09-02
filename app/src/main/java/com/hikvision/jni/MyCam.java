package com.hikvision.jni;

import android.content.Context;
import android.content.res.AssetManager;
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

    public static final int IMAGE_FORMAT_RGBA = 0x01;
    public static final int IMAGE_FORMAT_NV21 = 0x02;
    public static final int IMAGE_FORMAT_NV12 = 0x03;
    public static final int IMAGE_FORMAT_I420 = 0x04;
    private static String TAG = "MyCam";

    private long mNativeContextHandle;

    /**
     * description startPreview
     * param surface
     * @return
     */
    public native void native_startPreview(Surface surface);

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_stopPreview();

    /**
     * description native_CreateContext
     * param  renderType  0 应用  1  测试
     * @return
     */
    public native void native_CreateContext(int renderType);

    /**
     * description native_DestroyContext
     * param
     * @return
     */
    public native void native_DestroyContext();


    /**
     * description native_Init
     * param
     * @return
     */
    public native int native_Init();

    /**
     * description native_Init
     * param
     * @return
     */
    public native int native_UnInit();


    /**
     * description native_OnPreviewFrame
     * param
     * @return
     */
    protected native void native_OnPreviewFrame(int format, byte[] data, int width, int height);

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_OnSurfaceCreated();

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_OnSurfaceChanged(int width, int height);

    /**
     * description stopPreview
     * param
     * @return
     */
    public native void native_OnDrawFrame();

    /**
     * description native_StartRecord
     * param
     * @return
     */
    protected native int native_StartRecord(int recorderType, String outUrl, int frameWidth, int frameHeight, long videoBitRate, int fps);

    /**
     * description native_SetSurface  将应用层获取到的surface传递到NDK层
     * param
     * @return
     */
    public native void native_SetSurface(Surface surface);

    /**
     * description createStreamingMediaPlayer  创建播放资源 主要是将assets视频数据传传到NDK
     * param
     * @return
     */
    public static native int native_CreateStreamingMediaPlayer(AssetManager assetMgr, String filename);

}
