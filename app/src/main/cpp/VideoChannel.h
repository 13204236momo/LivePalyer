//
// Created by zhoumohan on 2019/6/18.
//

#ifndef LIVEPALYER_VIDEOCHANNEL_H
#define LIVEPALYER_VIDEOCHANNEL_H


#include <jni.h>
#include "librtmp/rtmp.h"

class VideoChannel {
typedef void (*VideoCallback)(RTMPPacket* packet);
public:
    void setVideoEncInfo(int width, int height, int fps, int bitrate);

    void encodeData(jbyte *string);

    void setVideoCallback(VideoCallback videoCallback);
private:
    int mWidth;
    int mHeight;
    int mFps;
    int mBitrate;
    int ySize;
    int uvSize;
    x264_t *videoCodec;
    //一帧
    x264_picture_t *pic_in;
    VideoCallback videoCallback;
    void sendSpsPps(uint8_t sps[100], uint8_t pps[100], int len, int pps_len);

    void sendFrame(int type, uint8_t *payload, int i_payload);
};


#endif //LIVEPALYER_VIDEOCHANNEL_H
