#include <jni.h>
#include <string>
#include <zconf.h>
#include <android/log.h>
#include "android/native_window_jni.h"

const char *LOG_TAG = "native-lib";
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


extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_LivePlayerUtil_native_1start(JNIEnv *env, jobject instance, jstring path_,
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
    uint8_t *out_buffer= (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA,
                                                                  codecContext->width,codecContext->height));
    //与缓存区相关联，设置rgb_frame缓存区
    avpicture_fill((AVPicture *)rgb_frame,out_buffer,AV_PIX_FMT_RGBA,codecContext->width,codecContext->height);


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
                sws_scale(swsContext,(const uint8_t *const *)frame->data,frame->linesize,0,
                          frame->height,rgb_frame->data, rgb_frame->linesize);
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
Java_com_demo_livePlayer_LivePlayerUtil_native_1sound(JNIEnv *env, jobject instance, jstring input_,
                                                      jstring output_) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

//    avformat_network_init();
//    //总上下文
//    AVFormatContext *formatContext = avformat_alloc_context();
//    //打开音频文件
//    if (avformat_open_input(&formatContext, input, NULL, NULL) != 0) {
//        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "无法打开音频文件");
//        return;
//    }
//    //获取输入文件信息
//    if (avformat_find_stream_info(formatContext, NULL) < 0) {
//        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "无法获取输入文件信息");
//        return;
//    }
//
//    int audio_stream_idx = -1;
//    for (int i = 0; i < formatContext->nb_streams; ++i) {
//        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//            audio_stream_idx = i;
//            break;
//        }
//    }
//
//    AVCodecParameters *codecParameters = formatContext->streams[audio_stream_idx]->codecpar;
//    AVCodec *dec = avcodec_find_decoder(codecParameters->codec_id);
//    //创建解码器上下文
//    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
//    avcodec_parameters_to_context(codecContext, codecParameters);
//    avcodec_open2(codecContext, dec, NULL);
//    //转换器上下文
//    SwrContext *swrContext = swr_alloc();
//    //音频输入参数
//    AVSampleFormat in_sample = codecContext->sample_fmt;
//    //输入采样率
//    int in_sample_rate = codecContext->sample_rate;
//    //输入声道布局
//    uint64_t in_ch_layout = codecContext->channel_layout;
//
//    //音频输出参数  （固定）
//    AVSampleFormat out_sample = AV_SAMPLE_FMT_S16;
//    //输出采样率
//    int out_sample_rate = 44100;
//    //输出声道布局
//    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
//    //设置转换器
//    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample, out_sample_rate, in_ch_layout,
//                       in_sample, in_sample_rate, 0, NULL);
//    //初始化转换器
//    swr_init(swrContext);
//
//    //
//    uint8_t *out_buffer = (uint8_t *) (av_malloc(2 * 44100));
//    FILE *fp_pcm = fopen(output, "wb");
//    //读取包 (压缩数据)
//    AVPacket *packet = av_packet_alloc();
//    AVFrame *frame = av_frame_alloc();
//    while (av_read_frame(formatContext, packet) >= 0) {
//        avcodec_send_packet(codecContext, packet);
//        //解压缩
//        int result = avcodec_receive_frame(codecContext, frame);
//        if (result == AVERROR(EAGAIN)) {
//            continue;
//        } else if (result < 0) {
//            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "解码完成");
//            break;
//        }
//        if (packet->stream_index == audio_stream_idx) {
//            //输出为统一格式
//            swr_convert(swrContext, &out_buffer, 2 * 44100,
//                        (const uint8_t **) frame->data, frame->nb_samples);
//
//            int out_channel_nb = av_get_channel_layout_nb_channels((uint64_t) out_buffer);
//            //缓冲区大小
//            int out_buffer_size = av_samples_get_buffer_size(NULL,out_channel_nb, frame->nb_samples,
//                    out_sample,1);
//            fwrite(out_buffer,1,out_buffer_size,fp_pcm);
//        }
//    }
//    fclose(fp_pcm);
//    av_frame_free(&frame);
//    av_free(out_buffer);
//    swr_free(&swrContext);
//    avcodec_close(codecContext);
//    avformat_close_input(&formatContext);

    AVCodec *pCodec;
    AVCodecContext *pCodecContext;
    AVFormatContext *pFormatContext;
    struct SwrContext *au_convert_ctx;


    uint8_t *out_buffer;

    //1. 注册
    av_register_all();
    //2.打开解码器 <-- 拿到解码器  <-- 拿到id <-- 拿到stream和拿到AVCodecContext <-- 拿到AVFormatContext

    //2.1 拿到AVFormatContext
    pFormatContext = avformat_alloc_context();
    //2.1.1 打开文件
    if (avformat_open_input(&pFormatContext, input, NULL, NULL) != 0) {
       // LOGE("打开文件失败!");
       // return -1;
        return;
    }
    //2.2 拿到AVCodecContext
    //2.2.1 拿到流信息
    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
      //  LOGE("AVFormatContext获取流信息失败!");
      //  return -1;
        return;
    }
    //打印信息
//    av_dump_format(pFormatContext, 0, input, false);

    //2.2.2 通过streams找到audio的索引下标 也就获取到了stream
    int audioStream = -1;
    int i = 0;
    for (; i < pFormatContext->nb_streams; i++)
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            break;
        }

    if (audioStream == -1) {
//        LOGE("AVMEDIA_TYPE_AUDIO索引没找到!");
//        return -1;
        return;
    }
    //2.2.3 获取到AVCodecContext
    pCodecContext = pFormatContext->streams[audioStream]->codec;

    //2.2.4 通过AVCodecContext拿到id ，拿到解码器
    pCodec = avcodec_find_decoder(pCodecContext->codec_id);
    if (pCodec == NULL) {
//        LOGE("AVCodec获取失败!");
//        return -1;
        return;
    }
    //2.2.5 打开解码器
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
//        LOGE("打开解码器失败!");
//        return -1;
        return;
    }

    //3. 解码  将解码数据封装在AVFrame <-- 拿到编码的数据AVPacket  <-- 读取数据源 <-- 解码文件参数设置

    //3.1 AVPacket初始化
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);

    //3.2 解码文件参数设置
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;

    //nb_samples: AAC-1024 MP3-1152
    //音频帧中每个声道的采样数
    int out_nb_samples = pCodecContext->frame_size;

    //音频采样格式 量化精度
    AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //采样率
    int out_sample_rate = 44100;
    //声道
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);

    //获取到 缓冲大小
    int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples,
                                                     out_sample_fmt, 1);
    out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

    //3.3 初始化AVFrame
    AVFrame *pFrame = av_frame_alloc();


    //3.4 获取到编码文件的参数信息
    //声道
    int64_t in_channel_layout = av_get_default_channel_layout(pCodecContext->channels);

    //3.5 参数设置
    au_convert_ctx = swr_alloc();
    au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt,
                                        out_sample_rate,
                                        in_channel_layout, pCodecContext->sample_fmt,
                                        pCodecContext->sample_rate, 0, NULL);
    swr_init(au_convert_ctx);

    //4. 读取编码数据到AVPacket 然后将数据解码存储到AVFrame  转换存储数据
    //4.1 读取编码数据到AVPacket
    int got_picture;
    int index = 0;
    FILE *outputFile = fopen(output, "wb");
    while (av_read_frame(pFormatContext, packet) >= 0) {
        if (packet->stream_index == audioStream) {
            //4.2 将数据解码存储到AVFrame
            if (avcodec_decode_audio4(pCodecContext, pFrame, &got_picture, packet) < 0) {
//                LOGE("解码失败");
//                return -1;
                return;
            }

            if (got_picture > 0) {
                //4.3 转换音频数据
                swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE,
                            (const uint8_t **) pFrame->data, pFrame->nb_samples);
              //  LOGE("index:%5d\t pts:%lld\t packet size:%d\n",index,packet->pts,packet->size);

                //4.4 存储数据
                fwrite(out_buffer, 1, static_cast<size_t>(out_buffer_size), outputFile);
                index++;
            }
        }
        //5. 释放相关资源
        av_packet_unref(packet);
    }

    swr_free(&au_convert_ctx);
    fclose(outputFile);
    av_free(out_buffer);
    // Close the codec
    avcodec_close(pCodecContext);
    // Close the video file
    avformat_close_input(&pFormatContext);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}