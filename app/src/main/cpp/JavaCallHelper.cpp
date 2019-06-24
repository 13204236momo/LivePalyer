//
// Created by zhoumohan on 2019/6/24.
//
#include "macro.h"
#include "JavaCallHelper.h"


JavaCallHelper::JavaCallHelper(JavaVM *_javaVM, JNIEnv *_env, jobject &jobj) {
    this->javaVM = _javaVM;
    this->env = _env;
    this->jobj = env->NewGlobalRef(jobj);

    jclass jClazz = env->GetObjectClass(jobj);
    jmid_error = env->GetMethodID(jClazz, "onError", "(I)V");
    jmid_progress = env->GetMethodID(jClazz, "onProgress", "(I)V");
    jmid_prepare = env->GetMethodID(jClazz, "onPrepared", "(I)V");

}

void JavaCallHelper::onPrepare(int thread) {

}

void JavaCallHelper::onProgress(int thread, int progress) {

}

void JavaCallHelper::onError(int thread, int code) {
    if (thread == THREAD_CHILD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            return;
        } else{
            jniEnv->CallVoidMethod(jobj, jmid_error, code);
            javaVM->DetachCurrentThread();
        }
    } else{
        env->CallVoidMethod(jobj, jmid_error, code);
    }

}

void JavaCallHelper::~JavaCallHelper() {

}