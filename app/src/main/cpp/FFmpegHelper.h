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
extern "C"{
#include <libavformat/avformat.h>

#include <libavutil/time.h>
}

/**
 * 控制层
 */
class FFmpegHelper {
    ~FFmpegHelper();

public:
    FFmpegHelper(JavaCallHelper *javaCallHelper, const char *string1);

    void prepare();

    void prepareFFmpeg();

private:
    pthread_t pid_prepare;
    AVFormatContext *formatContext;
    char *url;
    JavaCallHelper *javaCallHelper;
    VideoPullChannel *videoPullChannel;
    AudioPullChannel *audioPullChannel;
};


#endif //LIVEPALYER_FFMPEGHELPER_H
