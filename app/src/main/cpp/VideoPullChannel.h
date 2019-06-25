//
// Created by zhoumohan on 2019/6/24.
//

#ifndef LIVEPALYER_VIDEOPULLCHANNEL_H
#define LIVEPALYER_VIDEOPULLCHANNEL_H


#include "BaseChannel.h"
#include <pthread.h>
#include <android/native_window.h>
#include "JavaCallHelper.h"
#include "AudioPullChannel.h"

extern "C" {
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
};
typedef void (*RenderFrame)(uint8_t *,int,int,int);
class VideoPullChannel : public BaseChannel {
public:
    VideoPullChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext);

    ~VideoPullChannel();

    virtual void start();

    virtual void play();

    virtual void stop();

    void decodePacket();

    void synchronizeFrame();

    void setReaderFrameCallback(RenderFrame renderFrame);
private:
    pthread_t pid_video_play;
    pthread_t pid_synchronize;
    RenderFrame renderFrame;
};


#endif //LIVEPALYER_VIDEOPULLCHANNEL_H
