//
// Created by pirate on 2022/7/26.
//

#ifndef MEDIADEMO_SINGLEVIDEORECORDER_H
#define MEDIADEMO_SINGLEVIDEORECORDER_H

#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"
#include "cstdio"

enum {
    kMsgCodecBuffer,
    kMsgPause,
    kMsgResume,
    kMsgPauseAck,
    kMsgDecodeDone,
    kMsgSeek,
};


class SingleVideoRecorder {
public:
    SingleVideoRecorder();
    ~SingleVideoRecorder(){}
    int  startRecord();
    void setSurface(JNIEnv *jniEnv,jobject surface);
    int CreateStreamingMediaPlayer(int width,int height);
private:
    FILE *fp_out = NULL;
    ANativeWindow* m_window;
    AMediaFormat *m_format;
    AMediaCodec *m_codec;
    int64_t renderstart;
    bool isPlaying;
    bool renderonce;
};


#endif //MEDIADEMO_SINGLEVIDEORECORDER_H
