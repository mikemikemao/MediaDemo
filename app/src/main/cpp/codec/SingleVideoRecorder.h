//
// Created by pirate on 2022/7/26.
//

#ifndef MEDIADEMO_SINGLEVIDEORECORDER_H
#define MEDIADEMO_SINGLEVIDEORECORDER_H

#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <utils/ImageDef.h>
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"
#include "cstdio"
#include "thread"
#include "ThreadSafeQueue.h"


using namespace std;

enum {
    kMsgCodecBuffer,
    kMsgPause,
    kMsgResume,
    kMsgPauseAck,
    kMsgDecodeDone,
    kMsgSeek,
};

#define AMEDIACODEC_BUFFER_FLAG_KEY_FRAME 1

class SingleVideoRecorder {
public:
    SingleVideoRecorder(const char* outUrl, int frameWidth, int frameHeight, long bitRate, int fps);
    ~SingleVideoRecorder();
    int StartRecord();
    int OnFrame2Encode(NativeImage *inputFrame);
    ANativeWindow* getInputSurface();
private:
    static void StartH264EncoderThread(SingleVideoRecorder *context);
private:
    char m_outUrl[1024] = {0};
    int m_frameWidth;
    int m_frameHeight;
    int m_frameIndex = 0;
    long m_bitRate;
    int m_frameRate;

    AMediaCodec *codec;
    thread *m_encodeThread = nullptr;
    ThreadSafeQueue<NativeImage *> m_frameQueue;
    volatile int m_exit = 0;
    ANativeWindow*  m_pANWindowRef;       /* mediacodec input buffer producer */
};


#endif //MEDIADEMO_SINGLEVIDEORECORDER_H
