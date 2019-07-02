//
// Created by zhoumohan on 2019/6/24.
//
#include "AudioPullChannel.h"
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <libavutil/time.h>
}

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

    swr_ctx = swr_alloc_set_opts(0, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, out_sample_rate,
                                 avCodecContext->channel_layout,
                                 avCodecContext->sample_fmt,
                                 avCodecContext->sample_rate, 0, 0);
    swr_init(swr_ctx);

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
                                   AVCodecContext *avCodecContext,AVRational time_base) : BaseChannel(channelId,
                                                                                 javaCallHelper,
                                                                                 avCodecContext,
                                                                                 time_base) {
    //根据布局获取声道数
    out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    out_sampleSize = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    out_sample_rate = 44100;
    //CD音频标准
    //44100 双声道 2字节    out_samplesize  16位  2个字节   out_channels  2
    buffer = (uint8_t *) malloc(out_sample_rate * out_sampleSize * out_channels);
}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    AudioPullChannel *audioPullChannel = static_cast<AudioPullChannel *>(context);
    int dataLen = audioPullChannel->getPcm();
    if (dataLen > 0) {
        (*bq)->Enqueue(bq, audioPullChannel->buffer, dataLen);
    }
}

void AudioPullChannel::initOpenSL() {
    //音频引擎
    SLEngineItf engineInterface = NULL;
    //音频对此昂
    SLObjectItf engineObject = NULL;
    //混音器
    SLObjectItf outputMixObject = NULL;

    //播放器
    SLObjectItf bqPlayerObject = NULL;
    //回调接口
    SLPlayItf bqPlayerInterface = NULL;
    //缓冲队列
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;

    //    ----------------1-----初始化播放引擎-----------------------------
    SLresult result;
    result= slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //音频接口  相当于SurfaceHodler
    result=(*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineInterface);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    //    ---------2------------初始化播放引擎-----------------------------
    result =  (*engineInterface)->CreateOutputMix(engineInterface, &outputMixObject, 0, 0, 0);
    // 初始化混音器outputMixObject
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    //pcm数据格式
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM//播放pcm格式的数据
            , 2,//2个声道（立体声）
                            SL_SAMPLINGRATE_44_1, //44100hz的频率
                            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
                            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
                            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
                            SL_BYTEORDER_LITTLEENDIAN//小端模式
    };
    //
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};

    SLDataSink audioSnk = {&outputMix, NULL};
    SLDataSource slDataSource = {&android_queue, &pcm};
    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    (*engineInterface)->CreateAudioPlayer(engineInterface
            ,&bqPlayerObject// //播放器
            ,&slDataSource//播放器参数  播放缓冲队列   播放格式
            ,&audioSnk,//播放缓冲区
                                          1,//播放接口回调个数
                                          ids,//设置播放队列ID
                                          req//是否采用内置的播放队列
    );
    //初始化播放器
    (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    //bqPlayerObject   这个对象
    //得到接口后调用  获取Player接口
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerInterface);
    //获得播放器接口
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                    &bqPlayerBufferQueue);

    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
    //    设置播放状态
    (*bqPlayerInterface)->SetPlayState(bqPlayerInterface, SL_PLAYSTATE_PLAYING);
    bqPlayerCallback(bqPlayerBufferQueue, this);
    LOGE("--- 手动调用播放 packet:%d",this->pkt_queue.size());

}

void AudioPullChannel::decode() {

    AVPacket *packet = 0;
    while (isPlaying) {
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
            //需要更多队列
            continue;
        } else if (ret < 0) {
            //失败
            break;
        }
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, frame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

        while (frame_queue.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }

        frame_queue.enQueue(frame);
    }

}

int AudioPullChannel::getPcm() {
    AVFrame *frame = 0;
    int data_size = 0;
    while (isPlaying) {
        int ret = frame_queue.deQueue(frame);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        uint64_t dst_nb_samples = av_rescale_rnd(
                swr_get_delay(swr_ctx, frame->sample_rate) + frame->nb_samples,
                out_sample_rate, frame->sample_rate,
                AV_ROUND_UP);
        //转换
        int nb = swr_convert(swr_ctx, &buffer, dst_nb_samples, (const uint8_t **) frame->data,
                             frame->nb_samples);
        //buffer的大小
        data_size = nb * out_channels * out_sampleSize;
        clock = frame->pts * av_q2d(time_base);

        break;
    }
    if (javaCallHelper){
        javaCallHelper->onProgress(THREAD_CHILD,clock);
    }
    releaseAvFrame(frame);
    return data_size;
}

AudioPullChannel::~AudioPullChannel() {
free(buffer);
buffer = 0;
}
