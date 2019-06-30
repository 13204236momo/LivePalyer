//
// Created by zhoumohan on 2019/6/26.
//
#include "BaseChannel.h"
BaseChannel::BaseChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
            AVCodecContext *avCodecContext,AVRational time_base)  : channelId(channelId),
                                               avCodecContext(avCodecContext),
                                               javaCallHelper(javaCallHelper),
                                               time_base(time_base){
    pkt_queue.setReleaseHandle(releaseAvPacket);
    frame_queue.setReleaseHandle(releaseAvFrame);

};