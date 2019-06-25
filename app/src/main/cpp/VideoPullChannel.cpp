//
// Created by zhoumohan on 2019/6/24.
//

#include "VideoPullChannel.h"
#include "JavaCallHelper.h"

VideoPullChannel::VideoPullChannel(int id, JavaCallHelper *javaCallHelper,
                                   AVCodecContext *avCodecContext) : BaseChannel(id, javaCallHelper,
                                                                                 avCodecContext) {

}

VideoPullChannel::~VideoPullChannel() {

}

void *decode(void *args) {
    VideoPullChannel *videoPullChannel = static_cast<VideoPullChannel *>(args);
    videoPullChannel->decodePacket();
    return 0;
}

void *synchronize(void *args) {
    VideoPullChannel *videoPullChannel = static_cast<VideoPullChannel *>(args);
    videoPullChannel->synchronizeFrame();
    return 0;
}

void VideoPullChannel::play() {
    pkt_quene.setWork(1);
    frame_quene.setWork(1);
    isPlaying = true;
    pthread_create(&pid_video_play, NULL, decode, this);
    pthread_create(&pid_synchronize, NULL, synchronize, this);
}

void VideoPullChannel::start() {

}

void VideoPullChannel::decodePacket() {
//子线程
    AVPacket *packet = 0;
    while (isPlaying) {
        int ret = pkt_quene.get(packet);
        if (!ret) {
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, packet);
        releaseAvPacket(packet);
        if (ret == AVERROR(EAGAIN)) {
            //需要更多数据
            continue;
        } else if (ret < 0) {
            break;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, frame);
        frame_quene.put(frame);
        while (frame_quene.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }
    }
    releaseAvPacket(packet);
}

void VideoPullChannel::synchronizeFrame() {
    //初始化转换器
    //AVFrame yuv420 nv21 ->rb8888
    SwsContext *swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                            avCodecContext->pix_fmt,
                                            avCodecContext->width, avCodecContext->height,
                                            AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);

    //double frame_delay = 1.0 / fps;
    uint8_t *dst_data[4];
    int dst_lineSize[4];
    av_image_alloc(dst_data, dst_lineSize, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);
    AVFrame *frame = 0;
    while (isPlaying) {
        int ret = frame_quene.get(frame);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(frame->data),
                  frame->linesize, 0,
                  frame->height, dst_data, dst_lineSize);
        renderFrame(dst_data[0], dst_lineSize[0], avCodecContext->width, avCodecContext->height);
         av_usleep(16*1000000);
        releaseAvFrame(frame);
    }
    av_free(&dst_data[0]);
    isPlaying = 0;
    releaseAvFrame(frame);
    sws_freeContext(swsContext);

}



