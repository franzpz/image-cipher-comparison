#include <jni.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <android/log.h>


extern "C" {
    #include "tinyaes/aes.h"
}

//CRYPT CONFIG
#define MAX_LEN (2*1024*1024)
#define ENCRYPT 0
#define DECRYPT 1
#define AES_KEY_SIZE 256
#define READ_LEN 10

#define TARGET_CLASS "com/panxw/aes/jni/AESCryptor"
#define TARGET_CRYPT "crypt"
#define TARGET_CRYPT_SIG "([BJI)[B"
#define TARGET_READ "read"
#define TARGET_READ_SIG "(Ljava/lang/String;J)[B"



#define log_error_f(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)
#define log_error(tag,msg) __android_log_print(ANDROID_LOG_ERROR,tag,msg)
#define log_info_f(tag,fmt,...) __android_log_print(ANDROID_LOG_INFO,tag,fmt,__VA_ARGS__)
#define log_info(tag,msg) __android_log_print(ANDROID_LOG_INFO,tag,msg)
#define log_warn(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)

#define TAG "Ciphers C"

#define NANOS_IN_SECOND 1000000000

static long currentTimeInMs() {

    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);

    return lround(res.tv_nsec / 1.0e6) + res.tv_sec * 1000;
}

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
JNIEXPORT jlongArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_AesCCipher_runAesCLong(JNIEnv *env,
                                                                         jobject instance,
                                                                         jintArray imageBytes_,
                                                                         jint rounds,
                                                                         jint sleepInSeconds,
                                                                         jint mode) {
    jint *imageBytes = env->GetIntArrayElements(imageBytes_, NULL);
    long numberOfBytes = env->GetArrayLength(imageBytes_);

    jlong measurements[rounds];

    long tmp;
    sleep(sleepInSeconds);

    for(int r = 0; r < rounds; r++) {

        tmp = currentTimeInMs();

        //AES_IV
        unsigned char AES_IV[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                   0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

        //AES_KEY
        unsigned char AES_KEY[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71,
                                     0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f,
                                     0x35, 0x2c,
                                     0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09,
                                     0x14, 0xdf,
                                     0xf4};

        BYTE *padded_buffer_in = (BYTE *) malloc(sizeof(BYTE) * numberOfBytes);
        BYTE *padded_buffer_out = (BYTE *) malloc(sizeof(BYTE) * numberOfBytes);

        //set key & iv
        unsigned int key_schedule[AES_BLOCK_SIZE * 4] = {0};
        aes_key_setup(AES_KEY, key_schedule, AES_KEY_SIZE);

        int currentBlock = 0;
        int proccessedBytes = 0;
        long byteLength = AES_BLOCK_SIZE;
        while (proccessedBytes < numberOfBytes) {
            if(numberOfBytes - proccessedBytes < AES_BLOCK_SIZE)
                byteLength = numberOfBytes - proccessedBytes;

            if(mode == 1) {
                aes_encrypt_ctr(&padded_buffer_in[currentBlock * AES_BLOCK_SIZE], (int)byteLength,
                            &padded_buffer_out[currentBlock * AES_BLOCK_SIZE], key_schedule,
                            AES_KEY_SIZE, AES_IV);
            }
            else if(mode == 2) {
                aes_decrypt_ctr(&padded_buffer_in[currentBlock * AES_BLOCK_SIZE], (int)byteLength,
                            &padded_buffer_out[currentBlock * AES_BLOCK_SIZE], key_schedule,
                            AES_KEY_SIZE, AES_IV);
            }
            proccessedBytes += AES_BLOCK_SIZE;
            currentBlock++;
        }

        //log_info(TAG, " converted bytes ");
        for (int i = 0; i < numberOfBytes; i++) {
            imageBytes[i] = padded_buffer_out[i];
            //log_info_f(TAG, "%d ", originalImageBytes[i]);
        }

        free(padded_buffer_in);
        free(padded_buffer_out);

        measurements[r] = (jlong) (currentTimeInMs() - tmp);
    }

    sleep(sleepInSeconds);

    //env->SetIntArrayRegion(originalImageBytes_, 0, numberOfBytes, originalImageBytes);
    env->ReleaseIntArrayElements(imageBytes_, imageBytes, 0);

    jlongArray out;
    out = env->NewLongArray(rounds);
    env->SetLongArrayRegion(out, 0, rounds, measurements);
    return out;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_AesCCipher_encryptImageBytesAesCRounds(JNIEnv *env,
                                                                                   jobject instance,
                                                                                   jintArray originalImageBytes_, jint rounds) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long numberOfBytes = env->GetArrayLength(originalImageBytes_);

    //AES_IV
    unsigned char AES_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    //AES_KEY
    unsigned char AES_KEY[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71,
                                  0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c,
                                  0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf,
                                  0xf4 };

    int paddedSize = 0;

    if(numberOfBytes <= AES_BLOCK_SIZE)
        paddedSize = AES_BLOCK_SIZE;
    else {
        if(numberOfBytes % AES_BLOCK_SIZE == 0)
            paddedSize = (int)numberOfBytes;
        else
            paddedSize = ((int)numberOfBytes / AES_BLOCK_SIZE) * AES_BLOCK_SIZE + AES_BLOCK_SIZE;
    }

    BYTE *padded_buffer_in = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);
    BYTE *padded_buffer_out = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);

    for(int r = 0; r < (int)rounds; r++) {

        //set key & iv
        unsigned int key_schedule[AES_BLOCK_SIZE * 4] = { 0 };
        aes_key_setup(AES_KEY, key_schedule, AES_KEY_SIZE);

        //log_info(TAG, " orig bytes ");
        for (int i = 0; i < paddedSize; i++) {
            if (i < numberOfBytes)
                padded_buffer_in[i] = (BYTE) originalImageBytes[i];
            else
                padded_buffer_in[i] = 0x00;

            //log_info_f(TAG, "%u ", padded_buffer_in[i]);
        }

        int currentBlock = 0;
        int proccessedBytes = 0;
        while (proccessedBytes < paddedSize) {
            aes_encrypt(&padded_buffer_in[currentBlock * AES_BLOCK_SIZE],
                        &padded_buffer_out[currentBlock * AES_BLOCK_SIZE], key_schedule,
                        AES_KEY_SIZE);
            proccessedBytes += AES_BLOCK_SIZE;
            currentBlock++;
        }

        //log_info(TAG, " converted bytes ");
        for (int i = 0; i < numberOfBytes; i++) {
            originalImageBytes[i] = padded_buffer_out[i];
            //log_info_f(TAG, "%d ", originalImageBytes[i]);
        }
    }

    free(padded_buffer_in);
    free(padded_buffer_out);

    env->SetIntArrayRegion(originalImageBytes_, 0, numberOfBytes, originalImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_AesCCipher_decryptImageBytesAesCRounds(JNIEnv *env,
                                                                                   jobject instance,
                                                                                   jintArray originalImageBytes_, jint rounds) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long numberOfBytes = env->GetArrayLength(originalImageBytes_);

    //AES_IV
    unsigned char AES_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    //AES_KEY
    unsigned char AES_KEY[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71,
                                  0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c,
                                  0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf,
                                  0xf4 };

    int paddedSize = 0;

    if(numberOfBytes <= AES_BLOCK_SIZE)
        paddedSize = AES_BLOCK_SIZE;
    else {
        if(numberOfBytes % AES_BLOCK_SIZE == 0)
            paddedSize = (int)numberOfBytes;
        else
            paddedSize = ((int)numberOfBytes / AES_BLOCK_SIZE) * AES_BLOCK_SIZE + AES_BLOCK_SIZE;
    }

    BYTE *padded_buffer_in = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);
    BYTE *padded_buffer_out = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);

    for(int r = 0; r < (int)rounds; r++) {

        //set key & iv
        unsigned int key_schedule[AES_BLOCK_SIZE * 4] = { 0 };
        aes_key_setup(AES_KEY, key_schedule, AES_KEY_SIZE);

        //log_info(TAG, " orig bytes ");
        for (int i = 0; i < paddedSize; i++) {
            if (i < numberOfBytes)
                padded_buffer_in[i] = (BYTE) originalImageBytes[i];
            else
                padded_buffer_in[i] = 0x00;

            //log_info_f(TAG, "%u ", padded_buffer_in[i]);
        }

        int currentBlock = 0;
        int proccessedBytes = 0;
        while (proccessedBytes < paddedSize) {
            aes_decrypt(&padded_buffer_in[currentBlock * AES_BLOCK_SIZE],
                        &padded_buffer_out[currentBlock * AES_BLOCK_SIZE], key_schedule,
                        AES_KEY_SIZE);
            proccessedBytes += AES_BLOCK_SIZE;
            currentBlock++;
        }

        //log_info(TAG, " converted bytes ");
        for (int i = 0; i < numberOfBytes; i++) {
            originalImageBytes[i] = padded_buffer_out[i];
            //log_info_f(TAG, "%d ", originalImageBytes[i]);
        }

    }

    free(padded_buffer_in);
    free(padded_buffer_out);

    env->SetIntArrayRegion(originalImageBytes_, 0, numberOfBytes, originalImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_AesCCipher_encryptImageBytesAesC(JNIEnv *env,
                                                                                   jobject instance,
                                                                                   jintArray originalImageBytes_) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long numberOfBytes = env->GetArrayLength(originalImageBytes_);

    //AES_IV
    unsigned char AES_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    //AES_KEY
    unsigned char AES_KEY[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71,
                                  0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c,
                                  0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf,
                                  0xf4 };

    //set key & iv
    unsigned int key_schedule[AES_BLOCK_SIZE * 4] = { 0 };
    aes_key_setup(AES_KEY, key_schedule, AES_KEY_SIZE);

    int paddedSize = 0;

    if(numberOfBytes <= AES_BLOCK_SIZE)
        paddedSize = AES_BLOCK_SIZE;
    else {
        if(numberOfBytes % AES_BLOCK_SIZE == 0)
            paddedSize = (int)numberOfBytes;
        else
            paddedSize = ((int)numberOfBytes / AES_BLOCK_SIZE) * AES_BLOCK_SIZE + AES_BLOCK_SIZE;
    }

    BYTE *padded_buffer_in = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);
    BYTE *padded_buffer_out = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);

    log_info(TAG, " orig bytes ");
    for(int i = 0; i < paddedSize; i++){
        if(i < numberOfBytes)
            padded_buffer_in[i] = (BYTE)originalImageBytes[i];
        else
            padded_buffer_in[i] = 0x00;

        log_info_f(TAG, "%u ", padded_buffer_in[i]);
    }

    int currentBlock = 0;
    int proccessedBytes = 0;
    while(proccessedBytes < paddedSize) {
        aes_encrypt(&padded_buffer_in[currentBlock*AES_BLOCK_SIZE], &padded_buffer_out[currentBlock*AES_BLOCK_SIZE], key_schedule, AES_KEY_SIZE);
        proccessedBytes += AES_BLOCK_SIZE;
        currentBlock ++;
    }

    log_info(TAG, " converted bytes ");
    for(int i = 0; i < numberOfBytes; i++){
        originalImageBytes[i] = padded_buffer_out[i];
        log_info_f(TAG, "%d ", originalImageBytes[i]);
    }

    free(padded_buffer_in);
    free(padded_buffer_out);

    env->SetIntArrayRegion(originalImageBytes_, 0, numberOfBytes, originalImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_AesCCipher_decryptImageBytesAesC(JNIEnv *env,
                                                                                   jobject instance,
                                                                                   jintArray originalImageBytes_) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long numberOfBytes = env->GetArrayLength(originalImageBytes_);

    //AES_IV
    unsigned char AES_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    //AES_KEY
    unsigned char AES_KEY[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71,
                                  0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c,
                                  0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf,
                                  0xf4 };

    //set key & iv
    unsigned int key_schedule[AES_BLOCK_SIZE * 4] = { 0 };
    aes_key_setup(AES_KEY, key_schedule, AES_KEY_SIZE);

    int paddedSize = 0;

    if(numberOfBytes <= AES_BLOCK_SIZE)
        paddedSize = AES_BLOCK_SIZE;
    else {
        if(numberOfBytes % AES_BLOCK_SIZE == 0)
            paddedSize = (int)numberOfBytes;
        else
            paddedSize = ((int)numberOfBytes / AES_BLOCK_SIZE) * AES_BLOCK_SIZE + AES_BLOCK_SIZE;
    }

    BYTE *padded_buffer_in = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);
    BYTE *padded_buffer_out = (BYTE*)malloc(sizeof(BYTE)*numberOfBytes);

    log_info(TAG, " orig bytes ");
    for(int i = 0; i < paddedSize; i++){
        if(i < numberOfBytes)
            padded_buffer_in[i] = (BYTE)originalImageBytes[i];
        else
            padded_buffer_in[i] = 0x00;

        log_info_f(TAG, "%u ", padded_buffer_in[i]);
    }

    int currentBlock = 0;
    int proccessedBytes = 0;
    while(proccessedBytes < paddedSize) {
        aes_decrypt(&padded_buffer_in[currentBlock*AES_BLOCK_SIZE], &padded_buffer_out[currentBlock*AES_BLOCK_SIZE], key_schedule, AES_KEY_SIZE);
        proccessedBytes += AES_BLOCK_SIZE;
        currentBlock ++;
    }

    log_info(TAG, " converted bytes ");
    for(int i = 0; i < numberOfBytes; i++){
        originalImageBytes[i] = padded_buffer_out[i];
        log_info_f(TAG, "%d ", originalImageBytes[i]);
    }

    free(padded_buffer_in);
    free(padded_buffer_out);

    env->SetIntArrayRegion(originalImageBytes_, 0, numberOfBytes, originalImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);
    return originalImageBytes_;
}

/*
extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1_decryptImageBytesCipher1(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long len = env->GetArrayLength(originalImageBytes_);

    unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    useImageCipher1(DEC_MODE, imageBytes, len, sumOfBytes);

    jint* convertedImageBytes = (jint*)malloc(sizeof(jint*)*len);
    convertToJintArray(imageBytes, convertedImageBytes, len);
    free(imageBytes);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, convertedImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, convertedImageBytes, 0);
    return  originalImageBytes_;
}*/
