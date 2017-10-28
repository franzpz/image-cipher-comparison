#include <jni.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <android/log.h>


extern "C" {
    #include "tinyaes2/aes.h"
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

        uint8_t *padded_buffer_in = (uint8_t *) malloc(sizeof(uint8_t) * numberOfBytes);
        uint8_t *padded_buffer_out = (uint8_t *) malloc(sizeof(uint8_t) * numberOfBytes);

        convertToUnsignedCharArray(padded_buffer_in, imageBytes, numberOfBytes);

        if(mode == 1) {
            AES_CBC_encrypt_buffer(padded_buffer_out, padded_buffer_in, numberOfBytes, AES_KEY, AES_IV);
        }
        else if(mode == 2) {
            AES_CBC_decrypt_buffer(padded_buffer_out, padded_buffer_in, numberOfBytes, AES_KEY, AES_IV);
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
