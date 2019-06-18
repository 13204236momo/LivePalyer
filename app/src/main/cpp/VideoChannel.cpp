//
// Created by zhoumohan on 2019/6/18.
//

#include <x264.h>
#include "VideoChannel.h"

void VideoChannel::setVideoEncInfo(int width, int height, int fps, int bitrate) {
    mWidth = width;
    mHeight = height;
    mFps = fps;
    mBitrate = bitrate;
    ySize = width * height;
    uvSize = ySize / 4;

    //初始x264的编码器
    x264_param_t param;
    x264_param_default_preset(&param,"ultrafast","zerolatency");
    //编码复杂度base_line
    param.i_level_idc = 32;
    //输入数据格式
    param.i_csp = X264_CSP_I420;
    param.i_width = width;
    param.i_height = height;
    //无b帧（直播首开）
    param.i_bframe = 0;
    //码率控制，CQP：恒定质量 CRF：恒定码率，ABR：平均码率
    param.rc.i_rc_method = X264_RC_ABR;
    //码率（比特率Kbps）
    param.rc.i_bitrate = bitrate /1000;
    //瞬时最大码率
    param.rc.i_vbv_max_bitrate = bitrate /1000 *1.2;
    //设置i_vbv_max_bitrate必须设置此参数，码率控制去大小，单位kbps
    param.rc.i_vbv_buffer_size = bitrate / 1000;
    //帧率的分子
    param.i_fps_num = fps;
    //帧率的分母
    param.i_fps_den = 1;
    //时间基的分子
    param.i_timebase_num = param.i_fps_num;
    //时间基的分母
    param.i_timebase_den = param.i_fps_den;
    //用fps而不是时间戳来计算帧间距离
    param.b_vfr_input = 0;
    //帧距离（关键帧）2s一个关键帧
    param.i_keyint_max = fps * 2;
    //是否复制sps和pps放在每个关键真的前面 该餐宿设置是让每个关键帧（I帧）都附带sps、pps
    param.b_repeat_headers = 1;
    //多线程
    param.i_threads = 1;
    x264_param_apply_profile(&param,"baseline");
    //打开解码器
    videoCodec = x264_encoder_open(&param);
    pic_in = new x264_picture_t;
    x264_picture_alloc(pic_in,X264_CSP_I420,width,height);
}
