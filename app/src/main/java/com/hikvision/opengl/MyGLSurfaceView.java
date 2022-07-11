package com.hikvision.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;

/**
 * description ： TODO:类的作用
 * author : pirate
 * param
 * date : 2022/3/9 20:26
 */
public class MyGLSurfaceView extends GLSurfaceView {
    private static final String TAG = "MyGLSurfaceView";
    private MyGLRender mGLRender;
    public MyGLSurfaceView(Context context, MyGLRender glRender) {
        this(context, glRender, null);
    }
    public MyGLSurfaceView(Context context, MyGLRender glRender, AttributeSet attrs) {
        super(context, attrs);
        Log.d(TAG, "MyGLSurfaceView: 2");
        this.setEGLContextClientVersion(2);
        mGLRender = glRender;
        setRenderer(mGLRender);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }
}
