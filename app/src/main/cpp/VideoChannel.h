//
// Created by zhoumohan on 2019/6/18.
//

#ifndef LIVEPALYER_VIDEOCHANNEL_H
#define LIVEPALYER_VIDEOCHANNEL_H


class VideoChannel {

public:
    void setVideoEncInfo(int width, int height, int fps, int bitrate);

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
};


#endif //LIVEPALYER_VIDEOCHANNEL_H
