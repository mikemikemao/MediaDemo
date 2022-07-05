package com.hikvision.jni;

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
     * native
     * @return
     */
    public static native void native_Init();

}
