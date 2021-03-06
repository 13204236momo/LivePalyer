//
// Created by Administrator on 2019/6/4.
//

#ifndef PALYERWANGYI_VIDEOCHANNEL_H
#define PALYERWANGYI_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include <pthread.h>
#include <android/native_window.h>
#include "AudioPullChannel.h"
#include "JavaCallHelper.h"
typedef void (*RenderFrame)(uint8_t *, int, int, int);
class VideoPullChannel : public BaseChannel{


public:
    VideoPullChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext,AVRational time_base);

    virtual ~VideoPullChannel();
    virtual void play();

    virtual void stop();
    void decodePacket();

    void synchronizeFrame();
    void setRenderCallback(RenderFrame renderFrame);
    void setFps(double fps);
private:
    pthread_t pid_video_play;
    pthread_t pid_synchronize;
    RenderFrame renderFrame;
    double fps;
public:
    AudioPullChannel *audioPullChannel;
};


#endif //PALYERWANGYI_VIDEOCHANNEL_H
