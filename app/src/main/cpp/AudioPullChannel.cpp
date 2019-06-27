//
// Created by zhoumohan on 2019/6/24.
//
#include "AudioPullChannel.h"
#include <SLES/OpenSLES_Android.h>

void *audioPlay(void *args) {
    AudioPullChannel *audioPullChannel = static_cast<AudioPullChannel *>(args);
    audioPullChannel->play();
    return 0;
}

void *audioDecode(void *args) {
    AudioPullChannel *audioPullChannel = static_cast<AudioPullChannel *>(args);
    audioPullChannel->initOpenSL();
    return 0;
}

void AudioPullChannel::play() {
    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    //创建初始化OPENSL ES的线程
    pthread_create(&pid_audio_play, NULL, audioPlay, this);
    //创建音频解码的线程
    pthread_create(&pid_audio_decodec, NULL, audioDecode, this);
}

void AudioPullChannel::start() {

}

void AudioPullChannel::stop() {}

AudioPullChannel::AudioPullChannel(volatile int channelId, JavaCallHelper *javaCallHelper,
                                   AVCodecContext *avCodecContext) : BaseChannel(channelId,
                                                                                 javaCallHelper,
                                                                                 avCodecContext) {
    this->avCodecContext = avCodecContext;
    this->javaCallHelper = javaCallHelper;
}

void AudioPullChannel::initOpenSL() {
    //音频引擎
    SLEngineItf engineInterface = NULL;
    //音频对象
    SLObjectItf engineObjectItf = NULL;
    //混音器
    SLObjectItf outputMixObject = NULL;

    //播放器
    SLObjectItf bqPlayerObject = NULL;
    //回调接口
    SLPitchItf bqPlayerInterface = NULL;
    //缓冲队列
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;

    //1.-------------初始化播放引擎----------------
    SLresult result;
    result = slCreateEngine(&engineObjectItf,0,NULL,0,NULL,NULL);
    if (SL_RESULT_SUCCESS != result){
        return;
    }
    result = (*engineObjectItf)->Realize(engineObjectItf,SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result){
        return;
    }
    //1.-------------初始化混音器----------------
    result = (*engineInterface)->CreateOutputMix(engineInterface,&outputMixObject,0,0,0);
    if (SL_RESULT_SUCCESS != result){
        return;
    }
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {
            SL_DATALOCATOR_ANDROIDBUFFERQUEUE,2};
    SLDataFormat_PCM pcm;
    SLDataSource slDataSource = {&android_queue,&pcm};
    (*engineInterface)->CreateAudioPlayer(engineInterface,
            &bqPlayerObject,
            );
}
