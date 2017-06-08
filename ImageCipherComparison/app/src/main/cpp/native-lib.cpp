#include <jni.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <android/log.h>

extern "C" {
    #include "imagecipher1.h"
}

#define log_error_f(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)
#define log_error(tag,msg) __android_log_print(ANDROID_LOG_ERROR,tag,msg)
#define log_info_f(tag,fmt,...) __android_log_print(ANDROID_LOG_INFO,tag,fmt,__VA_ARGS__)
#define log_info(tag,msg) __android_log_print(ANDROID_LOG_INFO,tag,msg)
#define log_warn(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)

#define TAG "Ciphers C"

extern "C"
void useImageCipher1(int mode, unsigned char imageBytes[], long length, long sumOfBytes){
    PermutationSetup permSetups[4];

    permSetups[0].r = 3.6000000001;
    permSetups[0].x = 0.8000000001;

    permSetups[1].r = 3.6000000002;
    permSetups[1].x = 0.8000000002;

    permSetups[2].r = 3.6000000003;
    permSetups[2].x = 0.8000000003;

    permSetups[3].r = 3.6000000004;
    permSetups[3].x = 0.8000000004;

    DiffusionSetup diffuSetups[2];

    diffuSetups[0].miu = 0.600000000000001;
    diffuSetups[0].x = 0.350000000000001;
    diffuSetups[0].y = 0.350000000000002;

    diffuSetups[1].miu = 0.600000000000002;
    diffuSetups[1].x = 0.360000000000001;
    diffuSetups[1].y = 0.360000000000002;

    int encryptionRounds = 2;

    runAlgorithm(mode, imageBytes, length, sumOfBytes, permSetups, diffuSetups,  encryptionRounds);
}

void convertToUnsignedCharArray(unsigned char *convImageBytes, jint *imageBytes, long length) {
    log_info(TAG, "------- Provided Image -------");
    for(int i = 0; i < length; i++) {
        convImageBytes[i] = (unsigned char)imageBytes[i];
        log_info_f(TAG, "%d - %u", i, convImageBytes[i]);
    }
}

void convertToJintArray(unsigned char *convImageBytes, jint *imageBytes, long length) {
    log_info(TAG, "------- Converted Image -------");
    for(int i = 0; i < length; i++) {
        imageBytes[i] = convImageBytes[i];
        log_info_f(TAG, "%d - %u", i, imageBytes[i]);
    }
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_MainActivity_decryptImageBytes(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long len = env->GetArrayLength(originalImageBytes_);

    unsigned char imageBytes[len];
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);

    useImageCipher1(DEC_MODE, imageBytes, len, sumOfBytes);

    convertToJintArray(imageBytes, originalImageBytes, len);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, originalImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return  originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_MainActivity_encryptImageBytes(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    int len = env->GetArrayLength(originalImageBytes_);

    unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);

    useImageCipher1(ENC_MODE, imageBytes, len, sumOfBytes);

    convertToJintArray(imageBytes, originalImageBytes, len);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, originalImageBytes);

    free(imageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return originalImageBytes_;
}