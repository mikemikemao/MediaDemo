//
// Created by pirate on 2022/7/12.
//

#include "EglRender.h"
#include "utils/LogUtil.h"
#include <android/native_window_jni.h>

EglRender::EglRender()
    :m_EglDisplay(EGL_NO_DISPLAY)
    ,m_EglContext(EGL_NO_CONTEXT)
    ,m_EglConfig(NULL)
    ,m_GlVersion(-1)
    ,m_EglPresentationTimeANDROID(NULL){

}

void EglRender::init(JNIEnv *jniEnv,jobject surface){
    if (m_EglDisplay != EGL_NO_DISPLAY){
        LOGCATD("EglDisplay has been used");
        return;
    }
    //将surface转成ANativeWindow
    m_aNativeWindow = ANativeWindow_fromSurface(jniEnv, surface);


}


int EglRender::CreateGlesEnv(ANativeWindow* pAnw){
    const EGLint confAttr[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RECORDABLE_ANDROID, 1,      // EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };

    // EGL context attributes
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    // surface attributes
    // the surface size is set to the input frame size
    const EGLint surfaceAttr[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT,1,
            EGL_NONE
    };

    int ret = 0;
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;

    do {
        //1. 获取 EGLDisplay 对象，建立与本地窗口系统的连接
        m_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(m_EglDisplay == EGL_NO_DISPLAY)
        {
            LOGCATE("eglGetDisplay failed");
            ret = -1;
            break;
        }
        //2. 初始化 EGL 方法
        if(!eglInitialize(m_EglDisplay, &eglMajVers, &eglMinVers))
        {
            LOGCATE("eglInitialize failed");
            ret = -1;
            break;
        }

        LOGCATD("eglInitialize version %d.%d", eglMajVers, eglMinVers);

        //3. 获取 EGLConfig 对象，确定渲染表面的配置信息
        if(!eglChooseConfig(m_EglDisplay, confAttr, &m_EglConfig, 1, &numConfigs))
        {
            LOGCATE("EGLRender::CreateGlesEnv some config is wrong");
            ret = -1;
            break;
        }

        //4. 创建渲染表面 EGLSurface, 使用 eglCreatePbufferSurface 创建屏幕外渲染区域
        m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, pAnw,surfaceAttr);
        if(m_EglSurface == EGL_NO_SURFACE)
        {
            switch(eglGetError())
            {
                case EGL_BAD_ALLOC:
                    // Not enough resources available. Handle and recover
                    LOGCATE(" Not enough resources available");
                    break;
                case EGL_BAD_CONFIG:
                    // Verify that provided EGLConfig is valid
                    LOGCATE(" provided EGLConfig is invalid");
                    break;
                case EGL_BAD_PARAMETER:
                    // Verify that the EGL_WIDTH and EGL_HEIGHT are
                    // non-negative values
                    LOGCATE("provided EGL_WIDTH and EGL_HEIGHT is invalid");
                    break;
                case EGL_BAD_MATCH:
                    // Check window and EGLConfig attributes to determine
                    // compatibility and pbuffer-texture parameters
                    LOGCATE("Check window and EGLConfig attributes");
                    break;
            }
        }

        //5. 创建渲染上下文 EGLContext
        m_EglContext = eglCreateContext(m_EglDisplay, m_EglConfig, EGL_NO_CONTEXT, ctxAttr);
        if(m_EglContext == EGL_NO_CONTEXT)
        {
            EGLint error = eglGetError();
            if(error == EGL_BAD_CONFIG)
            {
                // Handle error and recover
                LOGCATE("eglCreateContext EGL_BAD_CONFIG");
                ret = -1;
                break;
            }
        }

        //6. 绑定上下文
        if(!eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext))
        {
            LOGCATE("EGLRender::CreateGlesEnv MakeCurrent failed");
            ret = -1;
            break;
        }
        LOGCATE("EGLRender::CreateGlesEnv initialize success!");

        // 7.获取eglPresentationTimeANDROID方法的地址
        m_EglPresentationTimeANDROID = (EGL_PRESENTATION_TIME_ANDROIDPROC)eglGetProcAddress("eglPresentationTimeANDROID");
        if (!m_EglPresentationTimeANDROID) {
            LOGCATE("EglPresentationTimeANDROID is NULL!");
        }

        //8.打印信息
        GLint values = 0;
        eglQueryContext(m_EglDisplay, m_EglContext, EGL_CONTEXT_CLIENT_VERSION, &values);
        LOGCATI("EGLContext created, client version %d", values);
        int rval = 0;
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_RED_SIZE, &rval);
        LOGCATI(" EGL_RED_SIZE %d", rval);
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_GREEN_SIZE, &rval);
        LOGCATI(" EGL_GREEN_SIZE %d", rval);
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_BLUE_SIZE, &rval);
        LOGCATI(" EGL_BLUE_SIZE %d", rval);
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_ALPHA_SIZE, &rval);
        LOGCATI(" EGL_ALPHA_SIZE %d", rval);
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_DEPTH_SIZE, &rval);
        LOGCATI(" EGL_DEPTH_SIZE %d", rval);
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_STENCIL_SIZE, &rval);
        LOGCATI(" EGL_STENCIL_SIZE %d", rval);
        eglGetConfigAttrib(m_EglDisplay, m_EglConfig, EGL_SURFACE_TYPE, &rval);
        LOGCATI(" EGL_SURFACE_TYPE %d", rval);

    }while (false);

    return ret;

}