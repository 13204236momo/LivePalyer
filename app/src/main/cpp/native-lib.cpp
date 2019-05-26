#include <jni.h>
#include <string>


extern "C"{
#include <libavformat/avformat.h>
}

extern "C"
JNIEXPORT void JNICALL
Java_com_demo_liveplayer_LivePlayerUtil_native_1start(JNIEnv *env, jobject instance, jstring path_,
                                                      jobject surface) {
    const char *path = env->GetStringUTFChars(path_, 0);

    // ffmpeg视频绘制  音频绘制
    //初始化网络模块
    avformat_network_init();
    //当前视频的总上下文
    AVFormatContext * formatContext = avformat_alloc_context();

    AVDictionary *opts = NULL;
    //设置打开视频超时时间 单位：微秒
    av_dict_set(&opts,"timeout","3000000",0);
    //0:成功 非0：失败
    int result = avformat_open_input(&formatContext,path,NULL,&opts);
    if (result){
        return;
    }
    
    
    //视频流

    int video_stream_idx = -1;
    //解析流  视频流 音频流   nb_streams:流的个数
    avformat_find_stream_info(formatContext,NULL);
    for (int i = 0; i <formatContext->nb_streams ; ++i) {
        if(formatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
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
    avcodec_parameters_to_context(codecContext,codecpar);
    
    env->ReleaseStringUTFChars(path_, path);
}