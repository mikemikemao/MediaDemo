//
// Created by pirat on 2022/7/16.
//

#include "MediaRecorderContext.h"
#include "utils/LogUtil.h"
#include "GLCameraRender.h"

jfieldID MediaRecorderContext::s_ContextHandle = 0L;

MediaRecorderContext::MediaRecorderContext() {
    GLCameraRender::GetInstance();
}

MediaRecorderContext::~MediaRecorderContext()
{
    GLCameraRender::ReleaseInstance();
}


void MediaRecorderContext::CreateContext(JNIEnv *env, jobject instance)
{
    LOGCATE("MediaRecorderContext::CreateContext");
    MediaRecorderContext *pContext = new MediaRecorderContext();
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
    GLCameraRender::GetInstance()->Init(0, 0, nullptr);
    //GLCameraRender::GetInstance()->SetRenderCallback(this, OnGLRenderFrame);
    return 0;
}

int MediaRecorderContext::UnInit()
{
    GLCameraRender::GetInstance()->UnInit();

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

    //NativeImageUtil::DumpNativeImage(&nativeImage, "/sdcard", "camera");
    GLCameraRender::GetInstance()->RenderVideoFrame(&nativeImage);
}



void MediaRecorderContext::OnSurfaceCreated()
{
    GLCameraRender::GetInstance()->OnSurfaceCreated();
}

void MediaRecorderContext::OnSurfaceChanged(int width, int height)
{
    GLCameraRender::GetInstance()->OnSurfaceChanged(width, height);
}

void MediaRecorderContext::OnDrawFrame()
{
    GLCameraRender::GetInstance()->OnDrawFrame();
}

int MediaRecorderContext::StartRecord(int recorderType, const char *outUrl,
                                      int frameWidth, int frameHeight, long videoBitRate,int fps) {
    LOGCATE("MediaRecorderContext::StartRecord recorderType=%d, outUrl=%s, [w,h]=[%d,%d], videoBitRate=%ld, fps=%d", recorderType, outUrl, frameWidth, frameHeight, videoBitRate, fps);
    std::unique_lock<std::mutex> lock(m_mutex);
    switch (recorderType) {
        case RECORDER_TYPE_SINGLE_VIDEO:
            if(m_pVideoRecorder == nullptr) {
//                m_pVideoRecorder = new SingleVideoRecorder(outUrl, frameHeight, frameWidth, videoBitRate, fps);
//                m_pVideoRecorder->StartRecord();
            }
            break;

        default:
            break;
    }


    return 0;

}