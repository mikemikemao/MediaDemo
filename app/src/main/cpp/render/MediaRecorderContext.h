//
// Created by pirat on 2022/7/16.
//

#ifndef MEDIADEMO_MEDIARECORDERCONTEXT_H
#define MEDIADEMO_MEDIARECORDERCONTEXT_H
#include <jni.h>
#include <thread>
#include <codec/SingleVideoRecorder.h>
#include "utils/ImageDef.h"
#include "BaseGLRender.h"


using namespace std;

#define RECORDER_TYPE_SINGLE_VIDEO  0 //仅录制视频
#define RECORDER_TYPE_SINGLE_AUDIO  1 //仅录制音频
#define RECORDER_TYPE_AV            2 //同时录制音频和视频,打包成 MP4 文件

typedef enum GL_RENDER_TYPE{
    GL_RENDER_APPS = 0,
    GL_RENDER_TESTS,
};

class MediaRecorderContext {
public:
    /**
     * 创建GLCameraRender的单例模式
     * */
    MediaRecorderContext(GL_RENDER_TYPE renderType);
    /**
     * 销毁GLCameraRender的单例模式
     * */
    ~MediaRecorderContext();
    /**
    * 创建类的对象，当作赋值与mNativeContextHandle
    * */
    static void CreateContext(JNIEnv *env, jobject instance,GL_RENDER_TYPE renderType);

    static void StoreContext(JNIEnv *env, jobject instance, MediaRecorderContext *pContext);
    /**
   * 销毁类的对象，当作赋值与mNativeContextHandle
   * */
    static void DeleteContext(JNIEnv *env, jobject instance);

    /**
   * 获取类的对象
   * */
    static MediaRecorderContext* GetContext(JNIEnv *env, jobject instance);

    /**
   * openg初始化
   * */
    int Init();

    /**
   * openg去初始化
   * */
    int UnInit();

    /**
   * 将图片数据从应用程转到NDK
   * */
    void OnPreviewFrame(int format, uint8_t *pBuffer, int width, int height);

    /**
   * glsurface 创建
   * */
    void OnSurfaceCreated();

    /**
  * glsurface 改变
  * */
    void OnSurfaceChanged(int width, int height);

    /**
  * glsurface 渲染
  * */
    void OnDrawFrame();

    //回调 将YUV数据转成RGBA传回
    static void OnGLRenderFrame(void *ctx, NativeImage *pImage);

    /**
  * 开始编码
  * */
    int StartRecord(int recorderType, const char *outUrl,
                                          int frameWidth, int frameHeight, long videoBitRate,int fps);

private:
    GL_RENDER_TYPE m_renderType;
    static jfieldID s_ContextHandle;
    SingleVideoRecorder *m_pVideoRecorder = nullptr;
    BaseGLRender * m_BaseGLRender;
    mutex m_mutex;
};


#endif //MEDIADEMO_MEDIARECORDERCONTEXT_H
