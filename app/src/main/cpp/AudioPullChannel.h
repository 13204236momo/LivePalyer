//
// Created by zhoumohan on 2019/6/24.
//

#ifndef LIVEPALYER_AUDIOPULLCHANNEL_H
#define LIVEPALYER_AUDIOPULLCHANNEL_H


#include "BaseChannel.h"
#include "JavaCallHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
};
class AudioPullChannel : public BaseChannel {

public:
    AudioPullChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                     AVCodecContext *avCodecContext);

    virtual void start();
    virtual void play();
    virtual void stop();
    void initOpenSL();
    void decode();

private:
    pthread_t pid_audio_play;
    pthread_t pid_audio_decodec;
};


#endif //LIVEPALYER_AUDIOPULLCHANNEL_H
