//
// Created by 周末寒 on 2019/6/23.
//

#ifndef LIVEPALYER_FFMPEGHELPER_H
#define LIVEPALYER_FFMPEGHELPER_H

#include <pthread.h>
#include <android/native_window_jni.h>
#include "JavaCallHelper.h"
#include "VideoPullChannel.h"
#include "AudioPullChannel.h"
#include "BaseChannel.h"

extern "C"{
#include <libavformat/avformat.h>

#include <libavutil/time.h>
}

/**
 * 控制层
 */
class FFmpegHelper{
    ~FFmpegHelper();

public:
    FFmpegHelper(JavaCallHelper *javaCallHelper, const char *string1);

    void prepare();

    void start();
    void play();
    void prepareFFmpeg();
    void setRenderFrameCallback(RenderFrame renderFrame);

private:
    pthread_t pid_prepare; //初始完就销毁了
    pthread_t pid_play; //直到播放完成
    AVFormatContext *formatContext;
    char *url;
    JavaCallHelper *javaCallHelper;
    VideoPullChannel *videoPullChannel;
    AudioPullChannel *audioPullChannel;
    RenderFrame renderFrame;
    bool isPlaying;
};


#endif //LIVEPALYER_FFMPEGHELPER_H
