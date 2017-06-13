#include <jni.h>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <android/log.h>
#include "gmp.h"

extern "C" {
    #include "imagecipher2.h"
}

#define log_error_f(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)
#define log_error(tag,msg) __android_log_print(ANDROID_LOG_ERROR,tag,msg)
#define log_info_f(tag,fmt,...) __android_log_print(ANDROID_LOG_INFO,tag,fmt,__VA_ARGS__)
#define log_info(tag,msg) __android_log_print(ANDROID_LOG_INFO,tag,msg)
#define log_warn(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)

#define TAG "Ciphers C"

static char *key = (char *) "1234578901234567890123456789012";

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2_encryptImageBytes(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray imageBytes_) {
    jint *imageBytes = env->GetIntArrayElements(imageBytes_, NULL);
    int len = env->GetArrayLength(imageBytes_);

    AlgorithmParameter params = generateInitialContitions((unsigned char*)key);

    int block = 0;
    int blockPos;
    int bytesProcessed = 0;

    unsigned char buffer[BUFFER_SIZE];

    while(bytesProcessed < len) {

        blockPos = 0;
        while(blockPos < BUFFER_SIZE && bytesProcessed < len) {
            buffer[blockPos] = (unsigned char)imageBytes[bytesProcessed];
            blockPos++;
            bytesProcessed++;
        }

        encrypt(&params, buffer, blockPos, (unsigned char*)key);

        for(int i = 0; i < blockPos; i++){
            imageBytes[block*BUFFER_SIZE+i] = buffer[i];
        }

        block++;
    }

    env->SetIntArrayRegion(imageBytes_, 0, len, imageBytes);
    env->ReleaseIntArrayElements(imageBytes_, imageBytes, 0);
    return imageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2_decryptImageBytes(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray imageBytes_) {
    jint *imageBytes = env->GetIntArrayElements(imageBytes_, NULL);
    int len = env->GetArrayLength(imageBytes_);

    AlgorithmParameter params = generateInitialContitions((unsigned char*)key);

    int block = 0;
    int blockPos;
    int bytesProcessed = 0;

    unsigned char buffer[BUFFER_SIZE];

    while(bytesProcessed < len) {

        blockPos = 0;
        while(blockPos < BUFFER_SIZE && bytesProcessed < len) {
            buffer[blockPos] = (unsigned char)imageBytes[bytesProcessed];
            blockPos++;
            bytesProcessed++;
        }

        decrypt(&params, buffer, blockPos, (unsigned char*)key);

        for(int i = 0; i < blockPos; i++){
            imageBytes[block*BUFFER_SIZE+i] = buffer[i];
        }

        block++;
    }

    env->SetIntArrayRegion(imageBytes_, 0, len, imageBytes);
    env->ReleaseIntArrayElements(imageBytes_, imageBytes, 0);
    return imageBytes_;
}

extern "C"
JNIEXPORT jint JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2_gmpTest(JNIEnv *env,
                                                                       jobject instance, jint a) {
    log_info("franz", "got in");

    char *sum;

    mpz_t r, c, b;
    mpz_init(r);
    mpz_init(c);
    mpz_init(b);

    mpz_set_ui(c, 2);
    mpz_set_ui(b, 3);

    mpz_add(r, c, b);



    log_info_f("franz", "%s", (char*)mpz_get_str(NULL, 10, r));

    mpz_clear(r);
    mpz_clear(c);
    mpz_clear(b);

    return a;
}

/*
extern "C"
 void convertToUnsignedCharArray(unsigned char *convImageBytes, jint *imageBytes, long length) {
    //log_info(TAG, "------- Provided Image -------");
    for(int i = 0; i < length; i++) {
        convImageBytes[i] = (unsigned char)imageBytes[i];
        //log_info_f(TAG, "%d - %u", i, convImageBytes[i]);
    }
}

void convertToJintArray(unsigned char *convImageBytes, jint *imageBytes, long length) {
    //log_info(TAG, "------- Converted Image -------");
    for(int i = 0; i < length; i++) {
        imageBytes[i] = convImageBytes[i];
        //log_info_f(TAG, "%d - %u", i, imageBytes[i]);
    }
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1_decryptImageBytes(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long len = env->GetArrayLength(originalImageBytes_);

    //unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    //long sumOfBytes = (long)sumOfImageBytes;

    //convertToUnsignedCharArray(imageBytes, originalImageBytes, len);
    //env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    //AlgorithmParameter params = generateInitialContitions(key);

    int block = 0;
    int blockPos;
    int bytesProcessed = 0;

    while(true) {

    }

    /*

    jint* convertedImageBytes = (jint*)malloc(sizeof(jint*)*len);
    convertToJintArray(imageBytes, convertedImageBytes, len);
    free(imageBytes);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, originalImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return  originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1_encryptImageBytes(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    int len = env->GetArrayLength(originalImageBytes_);

    unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    useImageCipher1(ENC_MODE, imageBytes, len, sumOfBytes);

    jint* convertedImageBytes = (jint*)malloc(sizeof(jint*)*len);
    convertToJintArray(imageBytes, convertedImageBytes, len);
    free(imageBytes);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, convertedImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, convertedImageBytes, 0);
    return originalImageBytes_;
}

 */