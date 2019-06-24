#include <jni.h>
#include <string>
#include <zconf.h>
#include <android/log.h>
#include <pthread.h>
#include "android/native_window_jni.h"
#include <stdint.h>
#include "x264.h"
#include "librtmp/rtmp.h"
#include "VideoPushChannel.h"
#include "AudioPushChannel.h"
#include "macro.h"
#include "safe_queue.h"
#include "FFmpegHelper.h"
#include "JavaCallHelper.h"
#define MAX_AUDIO_FRAME_SIZE 192000
extern "C" {
//封装格式
#include <libavformat/avformat.h>
//缩放
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
//解码
#include <libavcodec/avcodec.h>
//重采样
#include <libswresample/swresample.h>
}

VideoChannel *videoChannel;
AudioChannel *audioChannel;
//是否已开启推流线程
int isStart = 0;
pthread_t pid;
uint32_t start_time;
int readyPushing = 0;
//队列
SafeQueue<RTMPPacket *> packets;


ANativeWindow *window = 0;
FFmpegHelper *ffmpegHelper;
JavaCallHelper *javaCallHelper;
JavaVM *javaVM = NULL;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    javaVM = vm;
    return JNI_VERSION_1_4;
}

void releasePacket(RTMPPacket *packet) {
    if (packet) {
        RTMPPacket_Free(packet);
        delete packet;
    }
}

void callback(RTMPPacket *packet) {
    if (packet) {
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        packets.put(packet);
    }
}

void *start(void *args) {
    char *url = static_cast<char *>(args);
    RTMP *rtmp = 0;
    rtmp = RTMP_Alloc();
    if (!rtmp) {
        LOGE("alloc rtmp失败");
        return NULL;
    }

    RTMP_Init(rtmp);
    int ret = RTMP_SetupURL(rtmp, url);
    if (!ret) {
        LOGE("设置地址失败：%s", url);
        return NULL;
    }

    //设置连接超时
    rtmp->Link.timeout = 5;
    RTMP_EnableWrite(rtmp);
    ret = RTMP_Connect(rtmp, 0);
    if (!ret) {
        LOGE("连接服务器失败：%s", url);
        return NULL;
    }

    //连接流
    ret = RTMP_ConnectStream(rtmp, 0);
    if (!ret) {
        LOGE("连接流失败：%s", url);
        return NULL;
    }

    start_time = RTMP_GetTime();
    //表示可以推流了
    readyPushing = 1;
    packets.setWork(1);
    RTMPPacket *packet = 0;
    callback(audioChannel->getAudioTag());
    while (readyPushing) {
        //列队获取数据 packets
        packets.get(packet);
        LOGE("取出一帧数据");
        if (!readyPushing) {
            break;
        }
        if (!packet) {
            continue;
        }
        packet->m_nInfoField2 = rtmp->m_stream_id;
        //发送数据包
        ret = RTMP_SendPacket(rtmp, packet, 1);
        if (!ret) {
            LOGE("发送数据包失败");
            return NULL;
        }
        //packet 释放
        releasePacket(packet);
    }

    isStart = 0;
    readyPushing = 0;
    packets.setWork(0);
    packets.clear();
    if (rtmp) {
        //关闭连接
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    delete (url);
    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_util_LivePlayerUtil_native_1start(JNIEnv *env, jobject instance,
                                                           jstring path_,
                                                           jobject surface) {

    const char *path = env->GetStringUTFChars(path_, 0);

    // ffmpeg视频绘制  音频绘制
    //初始化网络模块
    avformat_network_init();
    //当前视频的总上下文
    AVFormatContext *formatContext = avformat_alloc_context();

    AVDictionary *opts = NULL;
    //设置打开视频超时时间 单位：微秒
    av_dict_set(&opts, "timeout", "3000000", 0);
    //0:成功 非0：失败
    int result = avformat_open_input(&formatContext, path, NULL, &opts);
    if (result) {
        return;
    }

    //视频流

    int video_stream_idx = -1;
    //解析流  视频流 音频流 :流的个数
    avformat_find_stream_info(formatContext, NULL);
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    //视频流解码参数
    AVCodecParameters *codecpar = formatContext->streams[video_stream_idx]->codecpar;
    //得到解码器
    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    //解码器上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
    //将解码器参数copy到解码器上下文
    avcodec_parameters_to_context(codecContext, codecpar);
    //打开解码器
    avcodec_open2(codecContext, dec, NULL);
    //解码  AVPacket
    AVPacket *packet = av_packet_alloc();
    //从视频流读取数据包
    SwsContext *swsContext = sws_getContext(codecContext->width, codecContext->height,
                                            codecContext->pix_fmt,
                                            codecContext->width, codecContext->height,
                                            AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //设置缓冲区
    ANativeWindow_setBuffersGeometry(nativeWindow, codecContext->width, codecContext->height,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer outBuffer;
    AVFrame *frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    //缓存区
    uint8_t *out_buffer = (uint8_t *) av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA,
                                                                   codecContext->width,
                                                                   codecContext->height));
    //与缓存区相关联，设置rgb_frame缓存区
    avpicture_fill((AVPicture *) rgb_frame, out_buffer, AV_PIX_FMT_RGBA, codecContext->width,
                   codecContext->height);


    while (av_read_frame(formatContext, packet) >= 0) {
        avcodec_send_packet(codecContext, packet);
        result = avcodec_receive_frame(codecContext, frame);
        if (result == AVERROR(EAGAIN)) {
            continue;
        } else if (result < 0) {
            break;
        }

        if (packet->stream_index == video_stream_idx) {
            //非零 正在解码
            if (result == 0) {
                ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
                //绘制
                sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
                          frame->height, rgb_frame->data, rgb_frame->linesize);
                //拿到一行有多少个字节RGBA
                int destStride = outBuffer.stride * 4;
                uint8_t *firstWindow = static_cast<uint8_t *>(outBuffer.bits);
                uint8_t *src_data = rgb_frame->data[0];;
                int srcStride = rgb_frame->linesize[0];

                //渲染
                for (int i = 0; i < outBuffer.height; ++i) {
                    //内存拷贝 进行渲染
                    memcpy(firstWindow + i * destStride, src_data + i * srcStride, srcStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000 * 16);

            }
        }
    }
    ANativeWindow_release(nativeWindow);
    av_frame_free(&frame);
    avcodec_close(codecContext);
    avformat_free_context(formatContext);
    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_util_LivePlayerUtil_native_1sound(JNIEnv *env, jobject instance,
                                                           jstring input_,
                                                           jstring output_) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

    avformat_network_init();
    //总上下文
    AVFormatContext *formatContext = avformat_alloc_context();
    //打开音频文件
    if (avformat_open_input(&formatContext, input, NULL, NULL) != 0) {
        LOGE("无法打开音频文件");
        return;
    }
    //获取输入文件信息
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        LOGE("无法获取输入文件信息");
        return;
    }

    int audio_stream_idx = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_idx = i;
            break;
        }
    }

    AVCodecParameters *codecParameters = formatContext->streams[audio_stream_idx]->codecpar;
    AVCodec *dec = avcodec_find_decoder(codecParameters->codec_id);
    //创建解码器上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
    avcodec_parameters_to_context(codecContext, codecParameters);
    avcodec_open2(codecContext, dec, NULL);
    //转换器上下文
    SwrContext *swrContext = swr_alloc();
    //音频输入参数
    AVSampleFormat in_sample = codecContext->sample_fmt;
    //输入采样率
    int in_sample_rate = codecContext->sample_rate;
    //输入声道布局
    uint64_t in_ch_layout = codecContext->channel_layout;

    //音频输出参数  （固定）
    AVSampleFormat out_sample = AV_SAMPLE_FMT_S16;
    //输出采样率
    int out_sample_rate = 44100;
    //输出声道布局
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    //设置转换器
    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample, out_sample_rate, in_ch_layout,
                       in_sample, in_sample_rate, 0, NULL);
    //初始化转换器
    swr_init(swrContext);

    //
    uint8_t *out_buffer = (uint8_t *) (av_malloc(2 * 44100));
    FILE *fp_pcm = fopen(output, "wb");
    //读取包 (压缩数据)
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    while (av_read_frame(formatContext, packet) >= 0) {
        avcodec_send_packet(codecContext, packet);
        //解压缩
        int result = avcodec_receive_frame(codecContext, frame);
        if (result == AVERROR(EAGAIN)) {
            continue;
        } else if (result < 0) {
            LOGE("解码完成");
            break;
        }
        if (packet->stream_index == audio_stream_idx) {
            //输出为统一格式
            swr_convert(swrContext, &out_buffer, 2 * 44100,
                        (const uint8_t **) frame->data, frame->nb_samples);

            int out_channel_nb = av_get_channel_layout_nb_channels((uint64_t) out_buffer);
            //缓冲区大小
            int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb,
                                                             frame->nb_samples,
                                                             out_sample, 1);
            fwrite(out_buffer, 1, out_buffer_size, fp_pcm);
        }
    }
    fclose(fp_pcm);
    av_frame_free(&frame);
    av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_LivePushActivity_toStringX264(JNIEnv *env, jobject instance) {

    x264_picture_t *x264_picture = new x264_picture_t;
    RTMP_Alloc();
}

extern "C"
JNIEXPORT void JNICALL
/**
 * 初始化
 * @param env
 * @param instance
 */
Java_com_demo_livePlayer_util_live_LivePusher_native_1init(JNIEnv *env, jobject instance) {

    videoChannel = new VideoChannel;
    videoChannel->setVideoCallback(callback);
    audioChannel = new AudioChannel;
    audioChannel->setAudioCallback(callback);
}


extern "C"
JNIEXPORT void JNICALL
/**
 *
 * @param env
 * @param instance
 * @param width
 * @param height
 * @param fps 帧率
 * @param bitrate 码率
 */
Java_com_demo_livePlayer_util_live_LivePusher_native_1setVideoEncInfo(JNIEnv *env, jobject instance,
                                                                      jint width, jint height,
                                                                      jint fps, jint bitrate) {
    if (!videoChannel) {
        return;
    }
    videoChannel->setVideoEncInfo(width, height, fps, bitrate);

}

extern "C"
JNIEXPORT void JNICALL
/**
 * 开始推流
 * @param env
 * @param instance
 * @param url_  服务器地址
 */
Java_com_demo_livePlayer_util_live_LivePusher_native_1start(JNIEnv *env, jobject instance,
                                                            jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    if (isStart) {
        return;
    }

    isStart = 1;
    char *path = new char[strlen(url) + 1];
    strcpy(path, url);
    pthread_create(&pid, 0, start, path);

    env->ReleaseStringUTFChars(url_, url);
}


extern "C"
JNIEXPORT void JNICALL
/**
 * 视频推流
 * @param env
 * @param instance
 * @param data_
 */
Java_com_demo_livePlayer_util_live_LivePusher_native_1pushVideo(JNIEnv *env, jobject instance,
                                                                jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    if (!videoChannel || !readyPushing) {
        return;
    }
    videoChannel->encodeData(data);
    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT void JNICALL
/**
 * 音频推流
 * @param env
 * @param instance
 * @param data_
 */
Java_com_demo_livePlayer_util_live_LivePusher_native_1pushAudio(JNIEnv *env, jobject instance,
                                                                jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    if (!audioChannel || !readyPushing) {
        return;
    }
    audioChannel->encodeData(data);
    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_util_live_LivePusher_native_1setAudioEncInfo(JNIEnv *env, jobject instance,
                                                                      jint i, jint channels) {
    if (audioChannel) {
        audioChannel->setAudioEncInfo(i, channels);
    }

}extern "C"
JNIEXPORT jint JNICALL
Java_com_demo_livePlayer_util_live_LivePusher_getInoutSamples(JNIEnv *env, jobject instance) {

    if (audioChannel) {
        return audioChannel->getInputSamples();
    }
    return -1;

}

extern "C"
JNIEXPORT void JNICALL
/**
 * 传入视频流，解析
 * @param env
 * @param instance
 * @param dataSource_
 */
Java_com_demo_livePlayer_util_player_Player_native_1prepare(JNIEnv *env, jobject instance,
                                                            jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);

    javaCallHelper = new JavaCallHelper(javaVM,env,instance);
    ffmpegHelper = new FFmpegHelper(javaCallHelper, dataSource);
    ffmpegHelper->prepare();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_util_player_Player_native_1start(JNIEnv *env, jobject instance) {



}
extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_util_player_Player_native_1set_1surface(JNIEnv *env, jobject instance,
                                                                 jobject surface) {

    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    //创建新的窗口用于视频显示
    window = ANativeWindow_fromSurface(env, surface);

}