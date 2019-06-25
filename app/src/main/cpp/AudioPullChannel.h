//
// Created by zhoumohan on 2019/6/24.
//

#ifndef LIVEPALYER_AUDIOPULLCHANNEL_H
#define LIVEPALYER_AUDIOPULLCHANNEL_H


class AudioPullChannel : public BaseChannel {

public:
    AudioPullChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                     AVCodecContext *avCodecContext);

    virtual void start();
    virtual void play();
    virtual void stop();
};


#endif //LIVEPALYER_AUDIOPULLCHANNEL_H
