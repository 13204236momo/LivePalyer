//
// Created by zhoumohan on 2019/6/24.
//

#ifndef LIVEPALYER_AUDIOPULLCHANNEL_H
#define LIVEPALYER_AUDIOPULLCHANNEL_H



#include "BaseChannel.h"
#include "JavaCallHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};
class AudioPullChannel : public BaseChannel {

public:
    AudioPullChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                     AVCodecContext *avCodecContext,AVRational time_base);

    virtual void start();
    virtual void play();
    virtual void stop();
    void initOpenSL();
    void decode();
    int getPcm();



private:
    pthread_t pid_audio_play;
    pthread_t pid_audio_decodec;
    SwrContext *swr_ctx=NULL; //转换上下文
    int out_channels;  //通道数
    int out_sampleSize; //采样位数
    int out_sample_rate; //采样频率

public:
    uint8_t *buffer;
};


#endif //LIVEPALYER_AUDIOPULLCHANNEL_H
