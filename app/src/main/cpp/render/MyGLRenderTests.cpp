//
// Created by pirate on 2022/9/2.
//

#include <render/samples/TriangleSample.h>
#include <render/samples/TextureMapSample.h>
#include <render/samples/NV21TextureMapSample.h>
#include <render/samples/VaoSample.h>
#include <render/samples/FBOSample.h>
#include <render/samples/SharedEGLContextSample.h>
#include "MyGLRenderTests.h"
#include "../utils/LogUtil.h"

MyGLRenderTests* MyGLRenderTests::m_pContext = nullptr;

MyGLRenderTests::MyGLRenderTests()
{
    m_pCurSample = new TriangleSample();
    m_pBeforeSample = nullptr;
}

MyGLRenderTests::~MyGLRenderTests()
{
    if (m_pCurSample)
    {
        delete m_pCurSample;
        m_pCurSample = nullptr;
    }

    if (m_pBeforeSample)
    {
        delete m_pBeforeSample;
        m_pBeforeSample = nullptr;
    }
}

MyGLRenderTests *MyGLRenderTests::GetInstance()
{
    LOGCATE("MyGLRenderTests::GetInstance");
    if (m_pContext == nullptr)
    {
        m_pContext = new MyGLRenderTests();
    }
    return m_pContext;
}

void MyGLRenderTests::ReleaseInstance()
{
    LOGCATE("MyGLRenderTests::DestroyInstance");
    if (m_pContext)
    {
        delete m_pContext;
        m_pContext = nullptr;
    }
}


void MyGLRenderTests::OnSurfaceCreated()
{
    LOGCATE("MyGLRenderTests::OnSurfaceCreated");
    glClearColor(1.0f,1.0f,1.0f, 1.0f);
}

void MyGLRenderTests::OnSurfaceChanged(int width, int height)
{
    LOGCATE("MyGLRenderTests::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
    glViewport(0, 0, width, height);
    m_ScreenW = width;
    m_ScreenH = height;
}

void MyGLRenderTests::OnDrawFrame()
{
    LOGCATE("MyGLRenderTests::OnDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (m_pBeforeSample)
    {
        m_pBeforeSample->Destroy();
        delete m_pBeforeSample;
        m_pBeforeSample = nullptr;
    }

    if (m_pCurSample)
    {
        m_pCurSample->Init();
        m_pCurSample->Draw(m_ScreenW, m_ScreenH);
    }
}



void MyGLRenderTests::SetParamsInt(int paramType, int value0, int value1) {
    LOGCATE("MyGLRenderTests::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType,
            value0, value1);
    if (paramType == SAMPLE_TYPE) {
        m_pBeforeSample = m_pCurSample;

        LOGCATE("MyGLRenderTests::SetParamsInt 0 m_pBeforeSample = %p", m_pBeforeSample);
        switch (value0) {
            case SAMPLE_TYPE_KEY_TRIANGLE:
                m_pCurSample = new TriangleSample();
                break;
            case SAMPLE_TYPE_KEY_TEXTURE_MAP:
                m_pCurSample = new TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_YUV_TEXTURE_MAP :
                m_pCurSample = new NV21TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_VAO :
                m_pCurSample = new VaoSample();
                break;
            case SAMPLE_TYPE_KEY_FBO :
                m_pCurSample = new FBOSample();
                break;
            case SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER :
                m_pCurSample = new SharedEGLContextSample();
                break;
            default:
                m_pCurSample = nullptr;
                break;
        }
        LOGCATE("MyGLRenderTests::SetParamsInt m_pBeforeSample = %p, m_pCurSample=%p", m_pBeforeSample, m_pCurSample);
    }

}
