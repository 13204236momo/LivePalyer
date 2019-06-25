//
// Created by zhoumohan on 2019/6/25.
//

#ifndef LIVEPALYER_BASECHANNEL_H
#define LIVEPALYER_BASECHANNEL_H


#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include "safe_queue.h"
#include "JavaCallHelper.h"
#include "macro.h"

class BaseChannel {
public:
    SafeQueue<AVPacket *> pkt_quene;
    SafeQueue<AVFrame *> frame_quene;
    volatile int channelId;
    volatile bool isPlaying;
    AVCodecContext *avCodecContext;
    JavaCallHelper *javaCallHelper;

    BaseChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                AVCodecContext *avCodecContext)
            : channelId(channelId),
              avCodecContext(avCodecContext),
              javaCallHelper(javaCallHelper) {};

    virtual void start() = 0;

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
            pkt_quene.clear();
            frame_quene.clear();
            LOGE("释放channel:%d%d", pkt_quene.size(), frame_quene.size());
        }
    }
};


#endif //LIVEPALYER_BASECHANNEL_H
