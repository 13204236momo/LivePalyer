//
// Created by zhoumohan on 2019/6/25.
//

#ifndef LIVEPALYER_BASECHANNEL_H
#define LIVEPALYER_BASECHANNEL_H
#include "safe_queue.h"
#include "JavaCallHelper.h"
#include "macro.h"

extern "C"{
#include <libavcodec/avcodec.h>
};
class BaseChannel {
public:
    SafeQueue<AVPacket *> pkt_queue;
    SafeQueue<AVFrame *> frame_queue;
    volatile int channelId;
    volatile bool isPlaying;
    AVCodecContext *avCodecContext;
    JavaCallHelper *javaCallHelper;

    BaseChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                AVCodecContext *avCodecContext);

    virtual void play() = 0;

    virtual void stop() = 0;

    static void releaseAvPacket(AVPacket *&packet) {
        if (packet) {
            av_packet_free(&packet);
            packet = 0;
        }
    }

    static void releaseAvFrame(AVFrame *&frame) {
        if (frame) {
            av_frame_free(&frame);
            frame = 0;
        }
    }

    virtual ~BaseChannel() {
        if (avCodecContext) {
            avcodec_close(avCodecContext);
            avcodec_free_context(&avCodecContext);
            avCodecContext = 0;
        }
        pkt_queue.clear();
        frame_queue.clear();
        LOGE("释放channel:%d%d", pkt_queue.size(), frame_queue.size());

    }
};


#endif //LIVEPALYER_BASECHANNEL_H
