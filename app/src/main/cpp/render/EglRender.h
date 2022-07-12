//
// Created by pirate on 2022/7/12.
//

#ifndef MEDIADEMO_EGLRENDER_H
#define MEDIADEMO_EGLRENDER_H

#include <jni.h>
#include "stdint.h"
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/native_window.h>
#include <EGL/eglplatform.h>

typedef EGLBoolean (EGLAPIENTRYP EGL_PRESENTATION_TIME_ANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);

class EglRender {
public:
    EglRender();
    ~EglRender();
    void init(JNIEnv *jniEnv,jobject surface);
private:
    int CreateGlesEnv(ANativeWindow* pAnw);
private:
    EGLDisplay      m_EglDisplay;//相当于一个显示窗口
    EGLContext      m_EglContext;//上下文
    EGLConfig       m_EglConfig; //绘制目标framebuffer的配置属性 指针
    EGLSurface      m_EglSurface;
    EGL_PRESENTATION_TIME_ANDROIDPROC m_EglPresentationTimeANDROID;//设置时间戳
    EGLConfig       getConfig(int flags, int version);// 查找合适的EGLConfig 函数指针
    ANativeWindow*  m_aNativeWindow;
    int             m_GlVersion;
};


#endif //MEDIADEMO_EGLRENDER_H
