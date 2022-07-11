package com.hikvision.jni;

import android.util.Log;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/7/11 10:05
 */
public class MediaRendor {
    public static final int IMAGE_FORMAT_I420 = 0x04;
    private static String TAG = "MediaRendor";
    public void onPreviewFrame(int format, byte[] data, int width, int height) {
        native_OnPreviewFrame(format, data, width, height);
    }




    /////////////////////////////////////////////////////////////////////////////////////
    protected native void native_OnPreviewFrame(int format, byte[] data, int width, int height);
}
