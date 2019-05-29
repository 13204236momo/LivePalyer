#include <jni.h>
#include <string>
#include <zconf.h>
#include <android/log.h>
#include "android/native_window_jni.h"

const char *LOG_TAG = "native-lib";
#define MAX_AUDIO_FRME_SIZE 48000 * 4
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
    while (av_read_frame(formatContext, packet) >= 0) {
        avcodec_send_packet(codecContext, packet);
        result = avcodec_receive_frame(codecContext, frame);
        if (result == AVERROR(EAGAIN)) {
            continue;
        } else if (result < 0) {
            break;
        }
        //接收的容器
        uint8_t *dst_data[0];
        //每一行的首地址
        int dst_lineSize[0];
        av_image_alloc(dst_data, dst_lineSize, codecContext->width, codecContext->height,
                       AV_PIX_FMT_RGBA, 1);

        if (packet->stream_index == video_stream_idx) {
            //非零 正在解码
            if (result == 0) {
                //
                ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
                //绘制
                sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dst_data,
                          dst_lineSize);
                //拿到一行有多少个字节RGBA
                int destStride = outBuffer.stride * 4;
                uint8_t *src_data = dst_data[0];
                int src_lineSize = dst_lineSize[0];
                uint8_t *firstWindow = static_cast<uint8_t *>(outBuffer.bits);
                //渲染
                for (int i = 0; i < outBuffer.height; ++i) {
                    //内存拷贝 进行渲染
                    memcpy(firstWindow + i * destStride, src_data + i * src_lineSize, destStride);
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

////注册各大组件
//   // LOGE("注册成功");
//    AVFormatContext *avFormatContext = avformat_alloc_context();//获取上下文
//    int error;
//    char buf[] = "";
//    //打开视频地址并获取里面的内容(解封装)
//    if (error = avformat_open_input(&avFormatContext, path, NULL, NULL) < 0) {
//        av_strerror(error, buf, 1024);
//        // LOGE("%s" ,inputPath)
//        //LOGE("Couldn't open file %s: %d(%s)", inputPath, error, buf);
//        // LOGE("%d",error)
//        //LOGE("打开视频失败");
//        return;
//    }
//    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
//        //LOGE("获取内容失败");
//        return;
//    }
//    //获取到整个内容过后找到里面的视频流
//    int video_index=-1;
//    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
//        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
//            //如果是视频流,标记一哈
//            video_index = i;
//        }
//    }
//   // LOGE("成功找到视频流")
//    //对视频流进行解码
//    //获取解码器上下文
//    AVCodecContext *avCodecContext = avFormatContext->streams[video_index]->codec;
//    //获取解码器
//    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
//    //打开解码器
//    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
//       // LOGE("打开失败")
//        return;
//    }
//
//    //申请AVPacket
//    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
//    av_init_packet(packet);
//    //申请AVFrame
//    AVFrame *frame = av_frame_alloc();//分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧
//    AVFrame *rgb_frame = av_frame_alloc();//分配一个AVFrame结构体，指向存放转换成rgb后的帧
//    //输出文件
//    //FILE *fp = fopen(outputPath,"wb");
//
//
//    //缓存区
//    uint8_t  *out_buffer= (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA,
//                                                                  avCodecContext->width,avCodecContext->height));
//    //与缓存区相关联，设置rgb_frame缓存区
//    avpicture_fill((AVPicture *)rgb_frame,out_buffer,AV_PIX_FMT_RGBA,avCodecContext->width,avCodecContext->height);
//
//
//    SwsContext* swsContext = sws_getContext(avCodecContext->width,avCodecContext->height,avCodecContext->pix_fmt,
//                                            avCodecContext->width,avCodecContext->height,AV_PIX_FMT_RGBA,
//                                            SWS_BICUBIC,NULL,NULL,NULL);
//
//    //取到nativewindow
//    ANativeWindow *nativeWindow=ANativeWindow_fromSurface(env,surface);
//    if(nativeWindow==0){
//       // LOGE("nativewindow取到失败");
//        return;
//    }
//    //视频缓冲区
//    ANativeWindow_Buffer native_outBuffer;
//
//
//    int frameCount;
//    int h =0;
//   // LOGE("解码 ")
//    while (av_read_frame(avFormatContext, packet) >= 0) {
//       // LOGE("解码 %d",packet->stream_index)
//       // LOGE("VINDEX %d",video_index)
//        if(packet->stream_index==video_index){
//       //     LOGE("解码 hhhhh")
//            //如果是视频流
//            //解码
//            avcodec_decode_video2(avCodecContext, frame, &frameCount, packet);
//        //    LOGE("解码中....  %d",frameCount)
//            if (frameCount) {
//             //   LOGE("转换并绘制")
//                //说明有内容
//                //绘制之前配置nativewindow
//                ANativeWindow_setBuffersGeometry(nativeWindow,avCodecContext->width,avCodecContext->height,WINDOW_FORMAT_RGBA_8888);
//                //上锁
//                ANativeWindow_lock(nativeWindow, &native_outBuffer, NULL);
//                //转换为rgb格式
//                sws_scale(swsContext,(const uint8_t *const *)frame->data,frame->linesize,0,
//                          frame->height,rgb_frame->data,
//                          rgb_frame->linesize);
//                //  rgb_frame是有画面数据
//                uint8_t *dst= (uint8_t *) native_outBuffer.bits;
////            拿到一行有多少个字节 RGBA
//                int destStride=native_outBuffer.stride*4;
//                //像素数据的首地址
//                uint8_t * src=  rgb_frame->data[0];
////            实际内存一行数量
//                int srcStride = rgb_frame->linesize[0];
//                //int i=0;
//                for (int i = 0; i < avCodecContext->height; ++i) {
////                memcpy(void *dest, const void *src, size_t n)
//                    //将rgb_frame中每一行的数据复制给nativewindow
//                    memcpy(dst + i * destStride,  src + i * srcStride, srcStride);
//                }
////解锁
//                ANativeWindow_unlockAndPost(nativeWindow);
//                usleep(1000 * 16);
//
//            }
//        }
//        av_free_packet(packet);
//    }
//    //释放
//    ANativeWindow_release(nativeWindow);
//    av_frame_free(&frame);
//    av_frame_free(&rgb_frame);
//    avcodec_close(avCodecContext);
//    avformat_free_context(avFormatContext);
//    env->ReleaseStringUTFChars(path_, path);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_demo_livePlayer_LivePlayerUtil_native_1sound(JNIEnv *env, jobject instance, jstring input_,
                                                      jstring output_) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);

    avformat_network_init();
    //总上下文
    AVFormatContext *formatContext = avformat_alloc_context();
    //打开音频文件
    if (avformat_open_input(&formatContext, input, NULL, NULL) != 0) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "无法打开音频文件");
        return;
    }
    //获取输入文件信息
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "无法获取输入文件信息");
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
    while (av_read_frame(formatContext, packet) >= 0) {
        avcodec_send_packet(codecContext, packet);
        //解压缩
        AVFrame *frame = av_frame_alloc();
        int result = avcodec_receive_frame(codecContext, frame);

        if (result == AVERROR(EAGAIN)) {
            continue;
        } else if (result < 0) {
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "解码完成");
            break;
        }

        if (packet->stream_index == audio_stream_idx) {
            //输出为统一格式
            swr_convert(swrContext, &out_buffer, 2 * 44100,
                        (const uint8_t **) frame->data, frame->nb_samples);

            int out_channel_nb = av_get_channel_layout_nb_channels((uint64_t) out_buffer);
            //缓冲区大小
            int out_buffer_size = av_samples_get_buffer_size(NULL,out_channel_nb, frame->nb_samples,
                    out_sample,1);
            fwrite(out_buffer,1,out_buffer_size,fp_pcm);
        }
    }


//    avformat_network_init();
////    总上下文
//    AVFormatContext * formatContext = avformat_alloc_context();
//    //打开音频文件
//    if(avformat_open_input(&formatContext,input,NULL,NULL) != 0){
//      //  LOGI("%s","无法打开音频文件");
//        return;
//    }
//
//    //获取输入文件信息
//    if(avformat_find_stream_info(formatContext,NULL) < 0){
//       // LOGI("%s","无法获取输入文件信息");
//        return;
//    }
//    //视频时长（单位：微秒us，转换为秒需要除以1000000）
//    int audio_stream_idx=-1;
//    for (int i = 0; i < formatContext->nb_streams; ++i) {
//        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//            audio_stream_idx=i;
//            break;
//        }
//    }
//
//    AVCodecParameters *codecpar = formatContext->streams[audio_stream_idx]->codecpar;
//    //找到解码器
//    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
//    //创建上下文
//    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
//    avcodec_parameters_to_context(codecContext, codecpar);
//    avcodec_open2(codecContext, dec, NULL);
//    SwrContext *swrContext = swr_alloc();
//
////    输入的这些参数
//    AVSampleFormat in_sample =  codecContext->sample_fmt;
//    // 输入采样率
//    int in_sample_rate = codecContext->sample_rate;
//    //    输入声道布局
//    uint64_t in_ch_layout=codecContext->channel_layout;
////        输出参数  固定
//
////    输出采样格式
//    AVSampleFormat out_sample=AV_SAMPLE_FMT_S16;
////    输出采样
//    int out_sample_rate=44100;
////    输出声道布局
//    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
////设置转换器 的输入参数 和输出参数
//    swr_alloc_set_opts(swrContext,out_ch_layout,out_sample,out_sample_rate
//            ,in_ch_layout,in_sample,in_sample_rate,0,NULL);
////    初始化转换器其他的默认参数
//    swr_init(swrContext);
//    uint8_t *out_buffer = (uint8_t *)(av_malloc(2 * 44100));
//    FILE *fp_pcm = fopen(output, "wb");
//    //读取包  压缩数据
//    AVPacket *packet = av_packet_alloc();
//    int count = 0;
//    //    设置音频缓冲区间 16bit   44100  PCM数据
////            输出 值
//    while (av_read_frame(formatContext, packet)>=0) {
//        avcodec_send_packet(codecContext, packet);
//        //解压缩数据  未压缩
//        AVFrame *frame = av_frame_alloc();
////        c    指针
//        int ret = avcodec_receive_frame(codecContext, frame);
////        frame
//        if (ret == AVERROR(EAGAIN))
//            continue;
//        else if (ret < 0) {
//           // LOGE("解码完成");
//            break;
//        }
//        if (packet->stream_index!= audio_stream_idx) {
//            continue;
//        }
//        //LOGE("正在解码%d",count++);
////frame  ---->统一的格式
//        swr_convert(swrContext, &out_buffer, 2 * 44100,
//                    (const uint8_t **)frame->data, frame->nb_samples);
//        int out_channerl_nb= av_get_channel_layout_nb_channels(out_ch_layout);
////缓冲区的 大小
//        int out_buffer_size=  av_samples_get_buffer_size(NULL, out_channerl_nb, frame->nb_samples, out_sample, 1);
//        fwrite(out_buffer,1,out_buffer_size,fp_pcm);
//    }




    fclose(fp_pcm);
    av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}