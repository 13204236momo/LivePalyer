//
// Created by Administrator on 2019/6/4.
//

#ifndef PALYERWANGYI_WANGYIFFMPEG_H
#define PALYERWANGYI_WANGYIFFMPEG_H
#include <pthread.h>
#include <android/native_window.h>
#include "VideoPullChannel.h"
#include "AudioPullChannel.h"
#include "BaseChannel.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}
//控制层
class FFmpegHelper{
public:
    FFmpegHelper(JavaCallHelper *javaCallHelper, const char *dataSource);
    ~FFmpegHelper();
    void prepare();

    void prepareFFmpeg();
    void start();
    void play();
    void setRenderCallback(RenderFrame renderFrame);

private:
    bool isPlaying;
    char *url;
    pthread_t pid_prepare;//销毁
    pthread_t pid_play;//知道播放完毕
    VideoPullChannel *videoChannel;
    AudioPullChannel *audioChannel;
    AVFormatContext *formatContext;
    JavaCallHelper *javaCallHelper;
    RenderFrame renderFrame;

};


#endif //PALYERWANGYI_WANGYIFFMPEG_H
