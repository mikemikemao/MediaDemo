package com.hikvision.jni;

import android.view.Surface;

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
    /**
     * description startPreview
     * param surface
     * @return
     */
    public static native void startPreview(Surface surface);

    /**
     * description stopPreview
     * param
     * @return
     */
    public static native void stopPreview();

}
