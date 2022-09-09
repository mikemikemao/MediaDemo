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
                         codec(NULL),
                         m_pANWindowRef(NULL){
    LOGCATE("SingleVideoRecorder::SingleVideoRecorder outUrl=%s, [w,h]=[%d,%d], bitRate=%ld, fps=%d", outUrl, frameWidth, frameHeight, bitRate, fps);
    strcpy(m_outUrl, outUrl);
}

SingleVideoRecorder::~SingleVideoRecorder(){
    if(codec!=NULL){
        AMediaCodec_flush(codec);
        AMediaCodec_stop(codec);
        AMediaCodec_delete(codec);
        codec = NULL;
    }
}

int SingleVideoRecorder::OnFrame2Encode(NativeImage *inputFrame) {
    if(m_exit) return 0;
    LOGCATE("SingleVideoRecorder::OnFrame2Encode [w,h,format]=[%d,%d,%d]", inputFrame->width, inputFrame->height, inputFrame->format);
//    NativeImage *pImage = new NativeImage();
//    pImage->width = inputFrame->width;
//    pImage->height = inputFrame->height;
//    pImage->format = inputFrame->format;
//    NativeImageUtil::AllocNativeImage(pImage);
//    NativeImageUtil::CopyNativeImage(inputFrame, pImage);
//    //NativeImageUtil::DumpNativeImage(pImage, "/sdcard", "camera");
//    m_frameQueue.Push(pImage);
    return 0;
}

ANativeWindow* SingleVideoRecorder::getInputSurface()
{
    return m_pANWindowRef;
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
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 0x7F000789);


    LOGCATD("SingleVideoRecorder::format is : %s", AMediaFormat_toString(format));

    ret = AMediaCodec_configure(codec, format, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    AMediaFormat_delete(format);
    if (ret!=0){
        LOGCATE("AMediaCodec_configure return  %d ", ret);
        return ret;
    }
    ret = AMediaCodec_createInputSurface(codec, &m_pANWindowRef);
    if (ret!=AMEDIA_OK) {
        LOGCATE("AMediaCodec_createInputSurface failed ret = %d ", ret);
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
    int in_frame = 0;
    recorder->m_exit = false;
    while (!recorder->m_exit)
    {
        //如果超时时间设置为0， 就相当于轮训访问的方式，非常占用CPU，所以这里设置为-1，无限超时
        AMediaCodecBufferInfo info;
        ssize_t bufidx = AMediaCodec_dequeueOutputBuffer(recorder->codec,&info,m_Timeout);
        if (bufidx==AMEDIACODEC_INFO_TRY_AGAIN_LATER){
            LOGCATD("SingleVideoRecorder::no output available yet");
        }else if (bufidx==AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED){
            LOGCATD("SingleVideoRecorder::encoder output buffers changed");
        }else if (bufidx == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            auto format = AMediaCodec_getOutputFormat(recorder->codec);
            if(format!=NULL){
                LOGCATD("SingleVideoRecorder::format changed to: %s", AMediaFormat_toString(format));
                size_t length = 0;
                void* data = NULL;
                LOGCATD("SingleVideoRecorder::new format %d is %p length = %ld", AMediaFormat_getBuffer(format, "csd-0", &data, &length), data, length);
                LOGCATD("SingleVideoRecorder::new format is %p length = %ld",data, length);
                //backupSPSPPS((u8*)data, length);
                AMediaFormat_delete(format);
            }
        }else if (bufidx < 0){
            LOGCATD("SingleVideoRecorder::unexpected result from encoder.dequeueOutputBuffer: %zd", bufidx);
        }else{
            if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                LOGCATD("SingleVideoRecorder::output EOS");
                recorder->m_exit = true;
            }
            size_t BufferSize;
            uint8_t* outputBuf = AMediaCodec_getOutputBuffer(recorder->codec, bufidx, &BufferSize);
            if (outputBuf == nullptr) {
                LOGCATE("SingleVideoRecorder::outputBuf nullptr!");
                return;
            }
            LOGCATD("BufferSize = %d, info.size = %d", BufferSize, info.size);
            if (info.flags & AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG) {
                // 标记为BUFFER_FLAG_CODEC_CONFIG的缓冲区包含编码数据(PPS SPS)
                LOGCATD("SingleVideoRecorder::capture Video BUFFER_FLAG_CODEC_CONFIG.");
                //backupSPSPPS(outputBuf, info.size);
            }
            else if (info.flags & AMEDIACODEC_BUFFER_FLAG_KEY_FRAME) {
                LOGCATD("SingleVideoRecorder::capture Video BUFFER_FLAG_KEY_FRAME.");
            }
            else {
                LOGCATD("SingleVideoRecorder::capture Frame AMediaCodecBufferInfo.flags %d.", info.flags);
            }
            //保存数据
            LOGCATD("SingleVideoRecorder::EncodeBuff %x %x %x %x %x",*outputBuf,*(outputBuf+1),*(outputBuf+2),*(outputBuf+3),*(outputBuf+4));
            int64_t pts = info.presentationTimeUs;
            //if (isDebug) ALOGD("AMediaCodec_getOutputBuffer 容器空间BufferSize : %d", BufferSize);
            LOGCATD("SingleVideoRecorder::outputBuf size : %d, PTS : %lld", info.size, pts);
            ToolUnits::SaveDataApp(recorder->m_outUrl,outputBuf,info.size);
            AMediaCodec_releaseOutputBuffer(recorder->codec, bufidx, false);//不显示预览
        }
    }

    LOGCATE("SingleVideoRecorder::StartH264EncoderThread end");
}