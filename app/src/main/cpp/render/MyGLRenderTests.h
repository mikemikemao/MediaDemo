//
// Created by pirate on 2022/9/2.
//

#ifndef MEDIADEMO_MYGLRENDERTESTS_H
#define MEDIADEMO_MYGLRENDERTESTS_H

#include <stdint.h>
#include "thread"
#include <GLES3/gl3.h>
#include "BaseGLRender.h"
#include "GLSampleBase.h"

class MyGLRenderTests : public BaseGLRender {
public:
    //GLSurfaceView 的三个回调
    virtual void OnSurfaceCreated();
    virtual void OnSurfaceChanged(int w, int h);
    virtual void OnDrawFrame();
    //单例模式
    static MyGLRenderTests* GetInstance();
    static void ReleaseInstance();
    //切换各类模式
    void SetParamsInt(int paramType, int value0, int value1);
    void RenderVideoFrame(NativeImage *pImage);
    void OnStartRecord(int recorderType, const char *outUrl,
                                      int frameWidth, int frameHeight, long videoBitRate,int fps);
private:
    MyGLRenderTests();
    ~MyGLRenderTests();
    static void StartSwapThread(MyGLRenderTests *myGLRenderTests);
private:
    static MyGLRenderTests *m_pContext;
    GLSampleBase *m_pBeforeSample;
    GLSampleBase *m_pCurSample;
    int m_ScreenW;
    int m_ScreenH;
    std::mutex m_Mutex;
    NativeImage m_RenderImage;

    EglCore* eglEnv = nullptr;
    EGLSurface windSurface = nullptr;
    EGLContext m_currentContex = nullptr;
    thread *m_swapThread = nullptr;

    std::condition_variable m_Cond;

};


#endif //MEDIADEMO_MYGLRENDERTESTS_H
