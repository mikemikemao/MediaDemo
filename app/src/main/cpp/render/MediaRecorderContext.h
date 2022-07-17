//
// Created by pirat on 2022/7/16.
//

#ifndef MEDIADEMO_MEDIARECORDERCONTEXT_H
#define MEDIADEMO_MEDIARECORDERCONTEXT_H
#include <jni.h>
#include "utils/ImageDef.h"
#define RECORDER_TYPE_SINGLE_VIDEO  0 //仅录制视频
#define RECORDER_TYPE_SINGLE_AUDIO  1 //仅录制音频
#define RECORDER_TYPE_AV            2 //同时录制音频和视频,打包成 MP4 文件

class MediaRecorderContext {
public:
    MediaRecorderContext();

    ~MediaRecorderContext();

    static void CreateContext(JNIEnv *env, jobject instance);

    static void StoreContext(JNIEnv *env, jobject instance, MediaRecorderContext *pContext);

    static void DeleteContext(JNIEnv *env, jobject instance);

    static MediaRecorderContext* GetContext(JNIEnv *env, jobject instance);

    static void OnGLRenderFrame(void *ctx, NativeImage * pImage);

    int Init();

    int UnInit();

    void OnPreviewFrame(int format, uint8_t *pBuffer, int width, int height);

    //OpenGL callback
    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

private:
    static jfieldID s_ContextHandle;

};


#endif //MEDIADEMO_MEDIARECORDERCONTEXT_H
