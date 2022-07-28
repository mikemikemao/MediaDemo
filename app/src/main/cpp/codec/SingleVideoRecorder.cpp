//
// Created by pirate on 2022/7/26.
//

#include "SingleVideoRecorder.h"
#include "LogUtil.h"

SingleVideoRecorder::SingleVideoRecorder()
        :m_window(NULL),
         m_format(NULL),
         m_codec(NULL),
         renderstart(0),
         isPlaying(false),
         renderonce(false){

}

int SingleVideoRecorder::startRecord() {
    return 0;
}


void SingleVideoRecorder::setSurface(JNIEnv *jniEnv,jobject surface){
    if(m_window!=NULL){
        ANativeWindow_release(m_window);
        m_window = NULL;
    }
    m_window = ANativeWindow_fromSurface(jniEnv, surface);
    LOGCATD("setsurface %p",m_window);
}


int SingleVideoRecorder::CreateStreamingMediaPlayer(int width,int height){

//    m_codec = AMediaCodec_createDecoderByType("video/avc");//h264
//    if (m_codec==NULL){
//        LOGCATE("create codec failed");
//        return -1;
//    }
//    m_format = AMediaFormat_new();
//    if (m_format==NULL){
//        LOGCATE("create format failed");
//        return -1;
//    }
//    AMediaFormat_setString(m_format, "mime", "video/avc");
//    AMediaFormat_setInt32(m_format,AMEDIAFORMAT_KEY_WIDTH,width);
//    AMediaFormat_setInt32(m_format,AMEDIAFORMAT_KEY_HEIGHT,height);
//    //这里配置,解码flags 给0，编码给AMEDIACODEC_CONFIGURE_FLAG_ENCODE
//    media_status_t status = AMediaCodec_configure(m_codec,m_format,NULL,NULL,0);
//    if(status!=0)
//    {
//        LOGCATE("erro config %d\n",status);
//        return -1;
//    }
//    //启动
//    AMediaCodec_start(m_codec);
    return 0;
}