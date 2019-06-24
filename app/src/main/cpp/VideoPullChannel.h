//
// Created by zhoumohan on 2019/6/24.
//

#ifndef LIVEPALYER_VIDEOPULLCHANNEL_H
#define LIVEPALYER_VIDEOPULLCHANNEL_H


#include <libavcodec/avcodec.h>

class VideoPullChannel {
public:
    VideoPullChannel(int id,JavaCallHelper *javaCallHelper,AVCodecContext *avCodecContext);
    ~VideoPullChannel();

};


#endif //LIVEPALYER_VIDEOPULLCHANNEL_H
