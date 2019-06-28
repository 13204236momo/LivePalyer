//
// Created by zhoumohan on 2019/6/24.
//
#include "AudioPullChannel.h"
#include <SLES/OpenSLES_Android.h>

void *audioPlay(void *args) {
    AudioPullChannel *audioPullChannel = static_cast<AudioPullChannel *>(args);
    audioPullChannel->initOpenSL();
    return 0;
}

void *audioDecode(void *args) {
    AudioPullChannel *audioPullChannel = static_cast<AudioPullChannel *>(args);
    audioPullChannel->decode();
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

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioPullChannel *audioPullChannel = static_cast<AudioPullChannel *>(context);

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
    SLPlayItf bqPlayerInterface = NULL;
    //缓冲队列
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;

    //1.-------------初始化播放引擎----------------
    SLresult result;
    result = slCreateEngine(&engineObjectItf, 0, NULL, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    result = (*engineObjectItf)->Realize(engineObjectItf, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //1.-------------初始化混音器----------------
    result = (*engineInterface)->CreateOutputMix(engineInterface, &outputMixObject, 0, 0, 0);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {
            SL_DATALOCATOR_ANDROIDBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, //播放pcm格式的数据
                            2, //两个声道
                            SL_SAMPLINGRATE_44_1, //44100hz的频率
                            SL_PCMSAMPLEFORMAT_FIXED_16, //位数16位
                            SL_PCMSAMPLEFORMAT_FIXED_16,  //和位数一致
                            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_BACK_RIGHT, //立体声
                            SL_BYTEORDER_LITTLEENDIAN //小端模式
    };
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};
    SLDataSource slDataSource = {&android_queue, &pcm};
    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    (*engineInterface)->
            CreateAudioPlayer(engineInterface,
                              &bqPlayerObject, //播放器
                              &slDataSource,  //播放器参数 播放缓冲队列 播放格式
                              &audioSnk, //播放缓冲区
                              1, //播放回调接口
                              ids,//设置播放队列ID
                              req//是否采用内置的播放队列
    );
    //初始化播放器
    (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    //得到接口后调用 获取Player接口
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
    (*bqPlayerInterface)->SetPlayState(bqPlayerInterface, SL_PLAYSTATE_PLAYING);
    bqPlayerCallback(bqPlayerBufferQueue, this);


}

void AudioPullChannel::decode() {

}
