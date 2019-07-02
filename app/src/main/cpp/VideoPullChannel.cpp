//
// Created by Administrator on 2019/6/4.
//
#include "BaseChannel.h"

#include "VideoPullChannel.h"
#include "JavaCallHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}


void dropPacket(queue<AVPacket *> &q) {
    while (!q.empty()) {
        LOGE("丢弃视频");
        AVPacket *pkt = q.front();
        if (pkt->flags != AV_PKT_FLAG_KEY) {
            q.pop();
            BaseChannel::releaseAvPacket(pkt);
        } else {
            break;
        }
    }
}

void dropFrame(queue<AVFrame *> &q) {
    while (!q.empty()) {
        LOGE("丢弃视频");
        AVFrame *frame = q.front();
        if (frame->flags != AV_PKT_FLAG_KEY) {
            q.pop();
            BaseChannel::releaseAvFrame(frame);
        } else {
            break;
        }
    }
}

VideoPullChannel::VideoPullChannel(int id, JavaCallHelper *javaCallHelper,
                                   AVCodecContext *avCodecContext, AVRational time_base)
        : BaseChannel(id, javaCallHelper, avCodecContext, time_base) {
    frame_queue.setReleaseHandle(releaseAvFrame);
    frame_queue.setSyncHandle(dropFrame);
    this->javaCallHelper = javaCallHelper;
    this->avCodecContext = avCodecContext;
}

void *decode(void *args) {
    VideoPullChannel *videoChannel = static_cast<VideoPullChannel *>(args);
    videoChannel->decodePacket();
    return 0;
}

void *synchronize(void *args) {
    VideoPullChannel *videoChannel = static_cast<VideoPullChannel *>(args);
    videoChannel->synchronizeFrame();
    return 0;
}

void VideoPullChannel::play() {
    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    pthread_create(&pid_video_play, NULL, decode, this);
    pthread_create(&pid_synchronize, NULL, synchronize, this);

}

void VideoPullChannel::stop() {

}


void VideoPullChannel::decodePacket() {
//子线程
    AVPacket *packet = 0;
    while (isPlaying) {
//        流 --packet  ---音频 视频     可以   单一
        int ret = pkt_queue.deQueue(packet);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, packet);
        releaseAvPacket(packet);
        if (ret == AVERROR(EAGAIN)) {
            //需要更多数据
            continue;
        } else if (ret < 0) {
            //失败  直播  端
            break;
        }
//       AVFrame  yuvi420   nv21  --->rgb8888
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, frame);
//        压缩数据        要解压
        frame_queue.enQueue(frame);
        while (frame_queue.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }

    }
//    保险起见
    releaseAvPacket(packet);
}

void VideoPullChannel::synchronizeFrame() {
//初始换
    SwsContext *sws_ctx = sws_getContext(
            avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
            avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, 0, 0, 0);

    //1s
    uint8_t *dst_data[4]; //argb
    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize,
                   avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA, 1);
    AVFrame *frame = 0;
    while (isPlaying) {
        int ret = frame_queue.deQueue(frame);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        sws_scale(sws_ctx,
                  reinterpret_cast<const uint8_t *const *>(frame->data), frame->linesize, 0,
                  frame->height,
                  dst_data, dst_linesize);
        //回调出去
        renderFrame(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);

        clock = frame->pts * av_q2d(time_base);
        double frame_delays = 1.0 / fps;

        double audioClock = audioPullChannel->clock;
        double diff = clock - audioClock;
        //将解码所需的时间算进去，配置差的手机解码耗时多
        //解码时间
        double extra_delay = frame->repeat_pict / (2 * fps);
        double delay = extra_delay + frame_delays;

        if (clock > audioClock) {
            //视频播放超前
            if (diff > 1) {  //超前很多
                av_usleep((delay * 2) * 1000000);
            } else {
                av_usleep((delay * diff) * 1000000);
            }
        } else {
            //视频播放延后
            if (fabs(diff) > 1) {
                //不休眠
            } else if (fabs(diff) >= 0.05) {
                //视频需要追赶   进行丢帧操作
                releaseAvFrame(frame);
                frame_queue.sync();
            } else {


            }
        }


        releaseAvFrame(frame);
        //绘制 1    不是 2
        //dst_data   rgba
        //window

    }
    av_freep(&dst_data[0]);
    isPlaying = false;
    releaseAvFrame(frame);
    sws_freeContext(sws_ctx);
}

void VideoPullChannel::setRenderCallback(RenderFrame renderFrame) {
    this->renderFrame = renderFrame;

}

void VideoPullChannel::setFps(double fps) {
    this->fps = fps;
}

VideoPullChannel::~VideoPullChannel() {

}

