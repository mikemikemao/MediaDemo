//
// Created by pirate on 2022/8/3.
//

#include "EGLRender.h"
#include "LogUtil.h"

EGLRender::EGLRender()
    :m_eglDisplay(EGL_NO_DISPLAY),
     m_pEglContext(EGL_NO_CONTEXT),
     m_pEglConfig(NULL),
     m_nGlVersion(-1),
     m_eglPresentationTimeANDROID(NULL){
}

EGLRender::~EGLRender()
{
    if (m_eglDisplay != EGL_NO_DISPLAY){
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_eglDisplay, m_pEglContext);
        eglReleaseThread();
        eglTerminate(m_eglDisplay);
        m_eglDisplay = EGL_NO_DISPLAY;
        m_pEglContext = EGL_NO_CONTEXT;
        m_pEglConfig = NULL;
    }
}


int EGLRender::init(EGLContext sharedContext, int flags){
    EGLint eglMajVers, eglMinVers;
    //1. 获取 EGLDisplay 对象，建立与本地窗口系统的连接
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(m_eglDisplay == EGL_NO_DISPLAY)
    {
        //Unable to open connection to local windowing system
        LOGCATE("EGLRender::CreateGlesEnv Unable to open connection to local windowing system");
        return -1;
    }
    //2. 初始化 EGL 方法
    if(!eglInitialize(m_eglDisplay, &eglMajVers, &eglMinVers))
    {
        // Unable to initialize EGL. Handle and recover
        LOGCATE("EGLRender::CreateGlesEnv Unable to initialize EGL");
        return -1;
    }
    LOGCATE("EGLRender::CreateGlesEnv EGL init with version %d.%d", eglMajVers, eglMinVers);
    // 尝试使用GLES3
    if ((flags & FLAG_TRY_GLES3) != 0) {
        EGLConfig config = getConfig(3);
        if (config != NULL) {
            int attrib3_list[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
            EGLContext context = eglCreateContext(m_eglDisplay, config,
                                                  sharedContext, attrib3_list);
            checkEglError("eglCreateContext");
            if (eglGetError() == EGL_SUCCESS) {
                m_pEglConfig    = config;
                m_pEglContext   = context;
                m_nGlVersion    = 3;
            }
        }
    }
    //
    // 如果GLES3没有获取到，则尝试使用GLES2
    if (m_pEglContext == EGL_NO_CONTEXT)
    {
        EGLConfig config = getConfig(2);
        if (config != NULL) {
            int attrib2_list[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
            };
            EGLContext context = eglCreateContext(m_eglDisplay, config,
                                                  sharedContext, attrib2_list);
            checkEglError("eglCreateContext");
            if (eglGetError() == EGL_SUCCESS) {
                m_pEglConfig = config;
                m_pEglContext = context;
                m_nGlVersion = 2;
            }
        }
    }
    // 获取eglPresentationTimeANDROID方法的地址
    m_eglPresentationTimeANDROID = (EGL_PRESENTATION_TIME_ANDROIDPROC)eglGetProcAddress("eglPresentationTimeANDROID");
    if (!m_eglPresentationTimeANDROID) {
        LOGCATE("EGLRender::eglPresentationTimeANDROID is not available!");
        return -1;
    }

    int values = 0;
    eglQueryContext(m_eglDisplay, m_pEglContext, EGL_CONTEXT_CLIENT_VERSION, &values);
    LOGCATD("EGLRender::EGLContext created, client version %d", values);
    int rval = 0;
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_RED_SIZE, &rval);
    LOGCATD("EGLRender:: EGL_RED_SIZE %d", rval);
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_GREEN_SIZE, &rval);
    LOGCATD("EGLRender:: EGL_GREEN_SIZE %d", rval);
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_BLUE_SIZE, &rval);
    LOGCATD(" EGLRender::EGL_BLUE_SIZE %d", rval);
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_ALPHA_SIZE, &rval);
    LOGCATD(" EGLRender::EGL_ALPHA_SIZE %d", rval);
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_DEPTH_SIZE, &rval);
    LOGCATD(" EGLRender::EGL_DEPTH_SIZE %d", rval);
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_STENCIL_SIZE, &rval);
    LOGCATD(" EGLRender::EGL_STENCIL_SIZE %d", rval);
    eglGetConfigAttrib(m_eglDisplay, m_pEglConfig, EGL_SURFACE_TYPE, &rval);
    LOGCATD(" EGLRender::EGL_SURFACE_TYPE %d", rval);

    return 0;
}


EGLConfig EGLRender::getConfig(int version) {
    int renderableType = EGL_OPENGL_ES2_BIT;
    if (version >= 3) {
        renderableType |= EGL_OPENGL_ES3_BIT;
    }
    int attribList[] =
            {
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    EGL_DEPTH_SIZE, 16,
                    EGL_STENCIL_SIZE, 8,
                    EGL_RENDERABLE_TYPE, renderableType,
                    EGL_RECORDABLE_ANDROID, 1,      // EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    EGL_NONE
            };

    EGLConfig configs = NULL;
    int numConfigs;
    if (!eglChooseConfig(m_eglDisplay, attribList, &configs, 1, &numConfigs)) {
        LOGCATE("EGLRender::unable to find RGB8888, version =  %d  EGLConfig", version);
        return NULL;
    }
    return configs;
}

/**
 * 检查是否出错
 * @param msg
 */
void EGLRender::checkEglError(const char* msg)
{
    int error;
    if ((error = eglGetError()) != EGL_SUCCESS)
    {
        // TODO 抛出异常
        LOGCATE("EGLRender::%s: EGL error: %x", msg, error);
    }
}


EGLSurface EGLRender::createWindowSurface(ANativeWindow* pAnw)
{
    if(pAnw ==NULL){
        return NULL;
    }
    if(m_eglDisplay == EGL_NO_DISPLAY){
        return NULL;
    }
    int surfaceAttribs[] =
            {
                    EGL_NONE
            };
    LOGCATE("EGLRender::eglCreateWindowSurface start");
    EGLSurface pEglSurface = eglCreateWindowSurface(m_eglDisplay, m_pEglConfig, pAnw, surfaceAttribs);
    checkEglError("eglCreateWindowSurface");
    return pEglSurface;
}


/**
 * 创建离屏渲染的EGLSurface
 * @param width
 * @param height
 * @return
 */
EGLSurface EGLRender::createOffscreenSurface(int width, int height)
{
    int surfaceAttribs[] =
            {
                    EGL_WIDTH, width,
                    EGL_HEIGHT, height,
                    EGL_NONE
            };
    EGLSurface pEglSurface = eglCreatePbufferSurface(m_eglDisplay, m_pEglConfig, surfaceAttribs);
    checkEglError("createOffscreenSurface");
    return pEglSurface;
}


int EGLRender::makeCurrent(EGLSurface drawSurface, EGLSurface readSurface)
{
    int ret = 0;
    do
    {
        /* clean */
        if (drawSurface == readSurface && EGL_NO_SURFACE == readSurface)
        {
            if (!eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            {
                checkEglError("clean current");
                ret = -1;
                break;
            }
        }
        else
        {
            if (!eglMakeCurrent(m_eglDisplay, drawSurface, readSurface, m_pEglContext))
            {
                checkEglError("makeCurrent");
                ret = -1;
                break;
            }
        }
        ret = 0;
    } while (0);

    return ret;
}