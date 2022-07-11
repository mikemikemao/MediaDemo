//
// Created by pirate on 2022/3/12.
//

#ifndef FFMPEGDEMO_MYGLRENDERCONTEXT_H
#define FFMPEGDEMO_MYGLRENDERCONTEXT_H
#include <stdint.h>
#include <GLES3/gl3.h>
#include "LogUtil.h"
#include "GLSampleBase.h"
#include "TriangleSample.h"
#include "TextureMapSample.h"
#include "NV21TextureMapSample.h"
#include "VaoSample.h"
#include "FBOSample.h"

class MyGLRenderContext {
public:
    MyGLRenderContext();
    ~MyGLRenderContext();
    static MyGLRenderContext* GetInstance();
    static void DestroyInstance();
    void OnSurfaceCreated();
    void OnSurfaceChanged(int width, int height);
    void OnDrawFrame();
    void SetParamsInt(int paramType, int value0, int value1);
    void SetImageData(int format, int width, int height, uint8_t *pData);
private:
    static MyGLRenderContext *m_pContext;
    GLSampleBase *m_pBeforeSample;
    GLSampleBase *m_pCurSample;
    int m_ScreenW;
    int m_ScreenH;
};


#endif //FFMPEGDEMO_MYGLRENDERCONTEXT_H
