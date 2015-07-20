
#include <jni.h>
#include <android/log.h>

#include <stdlib.h>
#include <stdbool.h>

#include "rwpng.h"
#include "pngquant.h"

#define  LOG_TAG    "LibPngQuantizer"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void * reserved) {
    return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_nicdahlquist_pngquant_PngQuantizer_nativeInitialize(JNIEnv * env, jobject obj, jstring jFilename) {
    LOGI("nativeInitialize3()");

    const char* filename = (*env)->GetStringUTFChars(env,jFilename, 0);

    struct pngquant_options options = {
        .floyd = 1.f, // floyd-steinberg dithering
    };
    options.liq = liq_attr_create();
    options.verbose = true;
    pngquant_file(filename, "/sdcard/out.png", &options);

    (*env)->ReleaseStringUTFChars(env, jFilename, filename);

    LOGI("leave nativeInitialize3()");
}
