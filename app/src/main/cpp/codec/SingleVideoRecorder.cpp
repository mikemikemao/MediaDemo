//
// Created by pirate on 2022/7/26.
//

#include "utils/ToolUnits.h"
#include "SingleVideoRecorder.h"
#include "LogUtil.h"
#include "cstring"
#define CODEC_H264 "video/avc"


static int64_t m_Timeout = 50000;

SingleVideoRecorder::SingleVideoRecorder(const char *outUrl, int frameWidth, int frameHeight,long bitRate, int fps)
                        :m_frameWidth(frameWidth),
                         m_frameHeight(frameHeight),
                         m_bitRate(bitRate),
                         m_frameRate(fps),
                         codec(NULL){
    LOGCATE("SingleVideoRecorder::SingleVideoRecorder outUrl=%s, [w,h]=[%d,%d], bitRate=%ld, fps=%d", outUrl, frameWidth, frameHeight, bitRate, fps);
    strcpy(m_outUrl, outUrl);
}


int SingleVideoRecorder::OnFrame2Encode(NativeImage *inputFrame) {
    if(m_exit) return 0;
    LOGCATE("SingleVideoRecorder::OnFrame2Encode [w,h,format]=[%d,%d,%d]", inputFrame->width, inputFrame->height, inputFrame->format);
    NativeImage *pImage = new NativeImage();
    pImage->width = inputFrame->width;
    pImage->height = inputFrame->height;
    pImage->format = inputFrame->format;
    NativeImageUtil::AllocNativeImage(pImage);
    NativeImageUtil::CopyNativeImage(inputFrame, pImage);
    NativeImageUtil::DumpNativeImage(pImage, "/sdcard", "camera");
    m_frameQueue.Push(pImage);
    return 0;
}

int SingleVideoRecorder::StartRecord() {
    int ret = 0;
    //创建mediacodec
    codec = AMediaCodec_createEncoderByType(CODEC_H264);
    if(codec==NULL){
        LOGCATE("create encoder failed");
        return -1;
    }
    AMediaFormat* format = AMediaFormat_new();
    if (format==NULL){
        LOGCATE("create AMediaFormat failed");
        return -1;
    }
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, CODEC_H264);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, m_frameWidth);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, m_frameHeight);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_BIT_RATE, m_bitRate);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_FRAME_RATE, m_frameRate);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 1); //I帧间隔 默认为1
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 0x7F42016B);

    LOGCATD("SingleVideoRecorder::format is : %s", AMediaFormat_toString(format));

    ret = AMediaCodec_configure(codec, format, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    AMediaFormat_delete(format);
    if (ret!=0){
        LOGCATE("AMediaCodec_configure return  %d ", ret);
        return ret;
    }
    ret = AMediaCodec_start(codec);
    if (ret!=0){
        LOGCATE("AMediaCodec_start return  %d ", ret);
        return ret;
    }
    //开线程 处理编码
    m_encodeThread = new thread(StartH264EncoderThread, this);

    return 0;
}

int64_t getNowUs()
{
    timeval tv;
    gettimeofday(&tv, 0);
    return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
}


void SingleVideoRecorder::StartH264EncoderThread(SingleVideoRecorder *recorder) {
    LOGCATE("SingleVideoRecorder::StartH264EncoderThread start");
    //停止编码且队列为空时退出循环
    int in_frame = 0;
    while (!recorder->m_exit || !recorder->m_frameQueue.Empty())
    {
        if(recorder->m_frameQueue.Empty()) {
            //队列为空，休眠等待
            usleep(10 * 1000);
            continue;
        }
        //从队列中取一帧预览帧
        NativeImage *pImage = recorder->m_frameQueue.Pop();
        //目前支持YUV420P的编解码
        //如果超时时间设置为0， 就相当于轮训访问的方式，非常占用CPU，所以这里设置为-1，无限超时
        ssize_t bufidx = AMediaCodec_dequeueInputBuffer(recorder->codec,m_Timeout);
        if(bufidx >= 0)
        {
            size_t bufsize;
            int64_t pts = getNowUs();
            uint8_t *buf = AMediaCodec_getInputBuffer(recorder->codec, bufidx, &bufsize);
            //填充yuv数据
            //如只是测试编码速度，这里读文件操作也应该优化，预先载入到内存中，直接从内存中拷贝，降低耗时。
            memcpy(buf,pImage->ppPlane,bufsize);
            //入队列
            LOGCATD("SingleVideoRecorder:: in[%d] pts:%ld bufidx:%d bufsize=%d\n", in_frame++, pts,bufidx,bufsize);
            AMediaCodec_queueInputBuffer(recorder->codec, bufidx, 0, bufidx, pts, 0);
        }
        NativeImageUtil::FreeNativeImage(pImage);
        delete pImage;
    }

    LOGCATE("SingleVideoRecorder::StartH264EncoderThread end");
}