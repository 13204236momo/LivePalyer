//
// Created by zhoumohan on 2019/6/21.
//

#include <cstdint>
#include <zconf.h>
#include <jni.h>
#include <cstring>
#include "AudioChannel.h"
#include "faac.h"
#include "librtmp/rtmp.h"

RTMPPacket * AudioChannel::getAudioTag(){
u_char *buf;
u_long len;
faacEncGetDecoderSpecificInfo(audioCodec,&buf,&len);
int bodySize = 2 + len;

    RTMPPacket *packet = new RTMPPacket;
    RTMPPacket_Alloc(packet,bodySize);
    packet->m_body[0] = 0xAF;
    if (mChannels ==1){
        packet->m_body[0] = 0xAE;
    }
    packet->m_body[1] = 0x00;
    //编码之后的aac数据
    memcpy(&packet->m_body[2],buf,len);
    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = 0x11;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    audioCallback(packet);
}

void AudioChannel::encodeData(int8_t *data) {
    int byteLen = faacEncEncode(audioCodec, reinterpret_cast<int32_t *>(data), inputSample, buffer,
                  maxOutputBytes);
     if (byteLen>0){
         RTMPPacket *packet = new RTMPPacket;
         int bodySize = 2+byteLen;
         RTMPPacket_Alloc(packet,bodySize);
         packet->m_body[0] = 0xAF;
         if (mChannels ==1){
             packet->m_body[0] = 0xAF;
         }
         packet->m_body[1] = 0x01;
         //编码之后的aac数据
         memcpy(&packet->m_body[2],buffer,byteLen);
         packet->m_hasAbsTimestamp = 0;
         packet->m_nBodySize = bodySize;
         packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
         packet->m_nChannel = 0x11;
         packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
         audioCallback(packet);
     }
}

/**
 * 初始化
 * @param samplesInHZ
 * @param channels
 */
void AudioChannel::setAudioEncInfo(int samplesInHZ, int channels) {
    audioCodec = faacEncOpen(samplesInHZ, channels, &inputSample, &maxOutputBytes);
    //设置参数
    faacEncConfigurationPtr config = faacEncGetCurrentConfiguration(audioCodec);
    //编码版本
    config->mpegVersion = MPEG4;
    //lc标准
    config->aacObjectType = LOW;
    //16位
    config->inputFormat = FAAC_INPUT_16BIT;
    //编码出原始数据，
    config->outputFormat = 0;
    faacEncSetConfiguration(audioCodec, config);
    buffer = new u_char[maxOutputBytes];
}

void AudioChannel::setAudioCallback(AudioChannel::AudioCallback audioCallback) {
    this->audioCallback = audioCallback;
}

int AudioChannel::getInputSamples() {
    return inputSample;
}