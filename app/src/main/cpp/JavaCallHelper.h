//
// Created by zhoumohan on 2019/6/24.
//

#ifndef LIVEPALYER_JAVACALLHELPER_H
#define LIVEPALYER_JAVACALLHELPER_H


#include <jni.h>

class JavaCallHelper {
public:
    JavaCallHelper(JavaVM *_javaVM, JNIEnv *_env, jobject &jobj);

    void onPrepare(int thread);
    void onProgress(int thread,int progress);
    void onError(int thread,int code);

    ~JavaCallHelper();

private:
    JavaVM *javaVM;
    JNIEnv *env;
    jobject jobj;
    jmethodID jmid_prepare;
    jmethodID jmid_error;
    jmethodID jmid_progress;
};


#endif //LIVEPALYER_JAVACALLHELPER_H
