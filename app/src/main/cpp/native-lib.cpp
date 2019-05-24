#include <jni.h>
#include <string>

extern "C"{
#include <libavutil/avutil.h>
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_demo_livepalyer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}
