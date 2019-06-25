//
// Created by zhoumohan on 2019/6/24.
//

#include <libavcodec/avcodec.h>
#include "AudioPullChannel.h"
#include "JavaCallHelper.h"
#include "BaseChannel.h"

void AudioPullChannel::play() {

}

void AudioPullChannel::start() {

}

AudioPullChannel::AudioPullChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                                   AVCodecContext *avCodecContext) : BaseChannel(channelId,
                                                                                 javaCallHelper,
                                                                                 avCodecContext) {}
