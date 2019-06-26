//
// Created by zhoumohan on 2019/6/24.
//
#include "AudioPullChannel.h"


void AudioPullChannel::play() {

}

void AudioPullChannel::start() {

}

void AudioPullChannel::stop() {}

AudioPullChannel::AudioPullChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                                   AVCodecContext *avCodecContext) : BaseChannel(channelId,
                                                                                 javaCallHelper,
                                                                                 avCodecContext) {
    this->avCodecContext = avCodecContext;
    this->javaCallHelper = javaCallHelper;
}
