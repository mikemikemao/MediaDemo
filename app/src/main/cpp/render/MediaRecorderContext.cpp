//
// Created by pirat on 2022/7/16.
//

#include "MediaRecorderContext.h"
#include "utils/LogUtil.h"
#include "MyGLRenderApps.h"
#include "MyGLRenderTests.h"


jfieldID MediaRecorderContext::s_ContextHandle = 0L;

MediaRecorderContext::MediaRecorderContext(GL_RENDER_TYPE renderType)
                    :m_renderType(renderType) {
    if (renderType==GL_RENDER_APPS){
        m_BaseGLRender = MyGLRenderApps::GetInstance();
    }
    if (renderType==GL_RENDER_TESTS){
        m_BaseGLRender = MyGLRenderTests::GetInstance();
    }
}

MediaRecorderContext::~MediaRecorderContext()
{
    if (m_renderType==GL_RENDER_APPS){
        MyGLRenderApps::ReleaseInstance();
    }
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::ReleaseInstance();
    }
}


void MediaRecorderContext::CreateContext(JNIEnv *env, jobject instance,GL_RENDER_TYPE renderType)
{
    LOGCATE("MediaRecorderContext::CreateContext");
    MediaRecorderContext *pContext = new MediaRecorderContext(renderType);
    StoreContext(env, instance, pContext);
}

void MediaRecorderContext::StoreContext(JNIEnv *env, jobject instance, MediaRecorderContext *pContext)
{
    LOGCATE("MediaRecorderContext::StoreContext");
    jclass cls = env->GetObjectClass(instance);
    if (cls == NULL)
    {
        LOGCATE("MediaRecorderContext::StoreContext cls == NULL");
        return;
    }
    s_ContextHandle = env->GetFieldID(cls, "mNativeContextHandle", "J");
    if (s_ContextHandle == NULL)
    {
        LOGCATE("MediaRecorderContext::StoreContext s_ContextHandle == NULL");
        return;
    }
    env->SetLongField(instance, s_ContextHandle, reinterpret_cast<jlong>(pContext));
}


void MediaRecorderContext::DeleteContext(JNIEnv *env, jobject instance)
{
    LOGCATE("MediaRecorderContext::DeleteContext");
    if (s_ContextHandle == NULL)
    {
        LOGCATE("MediaRecorderContext::DeleteContext Could not find render context.");
        return;
    }
    MediaRecorderContext *pContext = reinterpret_cast<MediaRecorderContext *>(env->GetLongField(
            instance, s_ContextHandle));
    if (pContext)
    {
        delete pContext;
    }
    env->SetLongField(instance, s_ContextHandle, 0L);
}

MediaRecorderContext *MediaRecorderContext::GetContext(JNIEnv *env, jobject instance)
{
    LOGCATE("MediaRecorderContext::GetContext");
    if (s_ContextHandle == NULL)
    {
        LOGCATE("MediaRecorderContext::GetContext Could not find render context.");
        return NULL;
    }
    MediaRecorderContext *pContext = reinterpret_cast<MediaRecorderContext *>(env->GetLongField(
            instance, s_ContextHandle));
    return pContext;
}

int MediaRecorderContext::Init()
{
    //设置回调函数
    //MyGLRenderApps::GetInstance()->SetRenderCallback(this, OnGLRenderFrame);
    return 0;
}

int MediaRecorderContext::UnInit()
{
    //MyGLRenderApps::GetInstance()->UnInit();
    return 0;
}



void MediaRecorderContext::OnPreviewFrame(int format, uint8_t *pBuffer, int width, int height)
{
    LOGCATE("MediaRecorderContext::UpdateFrame format=%d, width=%d, height=%d, pData=%p",
            format, width, height, pBuffer);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pBuffer;

    switch (format)
    {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.pLineSize[0] = width;
            nativeImage.pLineSize[1] = width;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            nativeImage.pLineSize[0] = width;
            nativeImage.pLineSize[1] = width / 2;
            nativeImage.pLineSize[2] = width / 2;
            break;
        default:
            break;
    }

//	std::unique_lock<std::mutex> lock(m_mutex);
//	if(m_pVideoRecorder!= nullptr) {
//        m_pVideoRecorder->OnFrame2Encode(&nativeImage);
//    }
//	lock.unlock();

    NativeImageUtil::DumpNativeImage(&nativeImage, "/sdcard", "camera1");
    if (m_renderType==GL_RENDER_APPS){
        MyGLRenderApps::GetInstance()->RenderVideoFrame(&nativeImage);
    }
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::GetInstance()->RenderVideoFrame(&nativeImage);
    }
}



void MediaRecorderContext::OnSurfaceCreated()
{
    if (m_renderType==GL_RENDER_APPS){
        MyGLRenderApps::GetInstance()->OnSurfaceCreated();
    }
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::GetInstance()->OnSurfaceCreated();
    }
}

void MediaRecorderContext::OnSurfaceChanged(int width, int height)
{
    if (m_renderType==GL_RENDER_APPS){
        MyGLRenderApps::GetInstance()->OnSurfaceChanged(width, height);
    }
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::GetInstance()->OnSurfaceChanged(width, height);
    }
}

void MediaRecorderContext::OnDrawFrame()
{
    if (m_renderType==GL_RENDER_APPS){
        MyGLRenderApps::GetInstance()->OnDrawFrame();
    }
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::GetInstance()->OnDrawFrame();
    }
}

//回调 将YUV数据转成RGBA传回
void MediaRecorderContext::OnGLRenderFrame(void *ctx, NativeImage *pImage) {
    LOGCATE("MediaRecorderContext::OnGLRenderFrame ctx=%p, pImage=%p", ctx, pImage);
    MediaRecorderContext *context = static_cast<MediaRecorderContext *>(ctx);
    std::unique_lock<std::mutex> lock(context->m_mutex);
    if(context->m_pVideoRecorder != nullptr)
        context->m_pVideoRecorder->OnFrame2Encode(pImage);
}

int MediaRecorderContext::StartRecord(int recorderType, const char *outUrl,
                                      int frameWidth, int frameHeight, long videoBitRate,int fps) {
    if (m_renderType==GL_RENDER_APPS){
        //MyGLRenderApps::GetInstance()->OnStartRecord();
    }
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::GetInstance()->OnStartRecord(recorderType,outUrl,frameWidth,frameHeight,videoBitRate,fps);
    }
    return 0;
}


void MediaRecorderContext::SetParamsInt(int paramType, int value0, int value1) {
    if (m_renderType==GL_RENDER_TESTS){
        MyGLRenderTests::GetInstance()->SetParamsInt(paramType,value0,value1);
    }
}