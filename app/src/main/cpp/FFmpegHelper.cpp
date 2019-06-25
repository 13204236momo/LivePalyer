//
// Created by 周末寒 on 2019/6/23.
//

#include "FFmpegHelper.h"
#include "macro.h"


FFmpegHelper::FFmpegHelper(JavaCallHelper *javaCallHelper, const char *dataSource) {
    url = new char[strlen(dataSource) + 1];
    strcpy(url, dataSource);
    this->javaCallHelper = javaCallHelper;
}

void *prepareFFmpeg_(void *arg) {
    FFmpegHelper *ffmpegHelper = static_cast<FFmpegHelper *> (arg);
    ffmpegHelper->prepareFFmpeg();
}

void FFmpegHelper::prepare() {
    pthread_create(&pid_prepare, NULL, prepareFFmpeg_, this);
}

void FFmpegHelper::prepareFFmpeg() {
    /*
     * 子线程
     * 访问到对象属性
     */
    // ffmpeg视频绘制  音频绘制
    //初始化网络模块
    avformat_network_init();
    //当前视频的总上下文
    formatContext = avformat_alloc_context();

    AVDictionary *opts = NULL;
    //设置打开视频超时时间 单位：微秒
    av_dict_set(&opts, "timeout", "3000000", 0);
    //0:成功 非0：失败
    int result = avformat_open_input(&formatContext, url, NULL, &opts);
    if (result) {
        javaCallHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }

    //2.查找流
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        if (javaCallHelper) {
            javaCallHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        }
        return;
    }

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVCodecParameters *codecParameters = formatContext->streams[i]->codecpar;
        //找到解码器
        AVCodec *dec = avcodec_find_decoder(codecParameters->codec_id);
        if (!dec) {
            if (javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            }
            return;
        }

        //创建解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(dec);
        if (!codecContext) {
            if (javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            }
            return;
        }
        //打开解码器
        if (avcodec_open2(codecContext, dec, 0) != 0) {
            if (javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            }
            return;
        }

        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audioPullChannel = new AudioPullChannel(i, javaCallHelper, codecContext);
        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            videoPullChannel = new VideoPullChannel(i, javaCallHelper, codecContext);
            videoPullChannel->setReaderFrameCallback(renderFrame);
        }

        //音视频都没有
        if (!audioPullChannel && !videoPullChannel) {
            if (javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_NOMEDIA);
            }
            return;
        }
        //到此准备完成
        if (javaCallHelper) {
            javaCallHelper->onPrepare(THREAD_CHILD);
        }
    }
}

void *startThread(void *args) {
    FFmpegHelper *fFmpegHelper = static_cast<FFmpegHelper *>(args);
    fFmpegHelper->play();
    return 0;
}

void FFmpegHelper::start() {
    isPlaying = true;
    if (audioPullChannel) {
        audioPullChannel->play();
    }

    if (videoPullChannel) {
        videoPullChannel->play();
    }

    pthread_create(&pid_play, NULL, startThread, this);
}

void FFmpegHelper::play() {
    int ret = 0;
    while (isPlaying) {
        //减缓读取，来避免队列溢出
        if (audioPullChannel && audioPullChannel->pkt_quene.size() > 100) {
            //生产者的生产速度远远大于消费者的消费速度
            av_usleep(1000 * 10);
            continue;
        }

        if (videoPullChannel && videoPullChannel->pkt_quene.size() > 100) {
            //生产者的生产速度远远大于消费者的消费速度
            av_usleep(1000 * 10);
            continue;
        }
        //读取包
        AVPacket *packet = av_packet_alloc();
        //从媒体流中读取音频，视频包
        ret = av_read_frame(formatContext, packet);
        if (ret == 0) {
            //将数据包加入队列
            if (audioPullChannel && packet->stream_index == audioPullChannel->channelId) {
                audioPullChannel->pkt_quene.put(packet);
            } else if (videoPullChannel && packet->stream_index == videoPullChannel->channelId) {
                videoPullChannel->pkt_quene.put(packet);
            }
        } else if (ret == AVERROR_EOF) {
            //读取完毕 但不一定是播放完毕
            if (videoPullChannel->pkt_quene.empty() && videoPullChannel->frame_quene.empty() &&
                audioPullChannel->pkt_quene.empty() && audioPullChannel->frame_quene.empty()) {
                LOGE("播放完毕。。。");
                break;
            }
            //因为seek的存在，就算读取完毕，依然要循环去执行av_read_frame（否则seek了没用）
        } else {
            break;
        }
        isPlaying = 0;
        audioPullChannel->stop();
        videoPullChannel->stop();
    }
}


FFmpegHelper::~FFmpegHelper() {}

void FFmpegHelper::setRenderFrameCallback(RenderFrame renderFrame) {
    this->renderFrame = renderFrame;
}
