//
// Created by zhoumohan on 2019/6/21.
//

#ifndef LIVEPALYER_AUDIOCHANNEL_H
#define LIVEPALYER_AUDIOCHANNEL_H


#include <faac.h>
#include "librtmp/rtmp.h"

class AudioChannel {
    typedef void (*AudioCallback)(RTMPPacket *packet);
public:
    void setAudioEncInfo(int samplesInHZ, int channels);
    void encodeData(jbyte *string);

    int getInputSamples();

    void setAudioCallback(AudioCallback audioCallback);
private:
    AudioCallback audioCallback;
    int mChannels;
    faacEncHandle audioCodec;
    u_long inputSample;
    u_long maxOutputBytes;
    u_char *buffer = 0;

};


#endif //LIVEPALYER_AUDIOCHANNEL_H
