//
// Created by zhoumohan on 2019/6/18.
//

#ifndef LIVEPALYER_MACRO_H
#define LIVEPALYER_MACRO_H

#include <android/log.h>
const char *LOG_TAG = "native-lib";
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif //LIVEPALYER_MACRO_H
