//
// Created by pirate on 2022/8/3.
//

#ifndef MEDIADEMO_EGLRENDER_H
#define MEDIADEMO_EGLRENDER_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#define  FLAG_TRY_GLES2         (0x1)
#define  FLAG_TRY_GLES3         (FLAG_TRY_GLES2 << 1)

typedef EGLBoolean(EGLAPIENTRYP EGL_PRESENTATION_TIME_ANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);

class EGLRender {
public:
    EGLRender();
    ~EGLRender();
    int init(EGLContext sharedContext, int flags);
    EGLSurface createWindowSurface(ANativeWindow* pAnw);
    int makeCurrent(EGLSurface drawSurface, EGLSurface readSurface);
    EGLSurface createOffscreenSurface(int width, int height);
private:
    EGLConfig getConfig(int version);
    void checkEglError(const char* msg);
private:
    /* egl environment */
    EGLDisplay      m_eglDisplay;      /* 实际显示设备的抽象,可以理解为一个前端的显示窗口 */
    EGLContext      m_pEglContext;      /* OpenGL ES 图形上下文，它代表了OpenGL状态机 */
    EGLConfig       m_pEglConfig;       /* 绘制目标framebuffer的配置属性 */
    int             m_nGlVersion;
    // 设置时间戳方法
    EGL_PRESENTATION_TIME_ANDROIDPROC m_eglPresentationTimeANDROID;
};


#endif //MEDIADEMO_EGLRENDER_H
