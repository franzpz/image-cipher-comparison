#include <jni.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <android/log.h>
#include "gmp.h"

extern "C" {
    #include "package/gmp/imagecipher1.h"
    #include "package/gmp/imagecipher2.h"
}

#define log_error_f(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)
#define log_error(tag,msg) __android_log_print(ANDROID_LOG_ERROR,tag,msg)
#define log_info_f(tag,fmt,...) __android_log_print(ANDROID_LOG_INFO,tag,fmt,__VA_ARGS__)
#define log_info(tag,msg) __android_log_print(ANDROID_LOG_INFO,tag,msg)
#define log_warn(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)

#define NANOS_IN_SECOND 1000000000

static long currentTimeInMs() {

    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);

    return lround(res.tv_nsec / 1.0e6) + res.tv_sec * 1000;
}


#define TAG "Ciphers C"
extern "C"
void useImageCipher1(int mode, unsigned char imageBytes[], long length, long sumOfBytes, int rounds){
    PermutationSetup *permSetups = (PermutationSetup*)malloc(sizeof(PermutationSetup)*4);

    mpf_inits(permSetups[0].r, permSetups[0].x, permSetups[1].r, permSetups[1].x, permSetups[2].r, permSetups[2].x, permSetups[3].r, permSetups[3].x, NULL);

    mpf_set_d(permSetups[0].r, 3.6000000001);
    mpf_set_d(permSetups[0].x, 0.8000000001);

    mpf_set_d(permSetups[1].r, 3.6000000002);
    mpf_set_d(permSetups[1].x, 0.8000000002);

    mpf_set_d(permSetups[2].r, 3.6000000003);
    mpf_set_d(permSetups[2].x, 0.8000000003);

    mpf_set_d(permSetups[3].r, 3.6000000004);
    mpf_set_d(permSetups[3].x, 0.8000000004);

    DiffusionSetup *diffuSetups = (DiffusionSetup*)malloc(sizeof(DiffusionSetup)*2);

    mpf_inits(diffuSetups[0].miu, diffuSetups[0].x, diffuSetups[0].y, diffuSetups[1].miu, diffuSetups[1].x, diffuSetups[1].y, NULL);

    mpf_set_d(diffuSetups[0].miu, 0.600000000000001);
    mpf_set_d(diffuSetups[0].x, 0.350000000000001);
    mpf_set_d(diffuSetups[0].y, 0.350000000000002);

    mpf_set_d(diffuSetups[1].miu, 0.600000000000002);
    mpf_set_d(diffuSetups[1].x, 0.360000000000001);
    mpf_set_d(diffuSetups[1].y, 0.360000000000002);

    runAlgorithm(mode, imageBytes, length, sumOfBytes, permSetups, diffuSetups,  1);

    free(permSetups);
    free(diffuSetups);
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
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1Gmp_runImageBytesCipher1GmpCipherResult(
        JNIEnv *env, jobject instance, jintArray originalImageBytes_, jlong sumOfImageBytes,
        jint rounds, jint sleepInSeconds, jint mode) {

    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long len = env->GetArrayLength(originalImageBytes_);

    int algMode = mode == 1 ? ENC_MODE : DEC_MODE;

    jlong measurements[rounds];

    long tmp;
    //sleep((unsigned int)sleepInSeconds);

    for(int r = 0; r < rounds; r++) {

        tmp = currentTimeInMs();

        unsigned char *imageBytes = (unsigned char *) malloc(sizeof(unsigned char) * len);
        long sumOfBytes = (long) sumOfImageBytes;

        convertToUnsignedCharArray(imageBytes, originalImageBytes, len);

        if(algMode == ENC_MODE) {
            sumOfBytes = 0;
            for (int i = 0; i < len; i++)
                sumOfBytes += (long) imageBytes[i];
        }

        useImageCipher1(algMode, imageBytes, len, sumOfBytes, 1);

        jint *convertedImageBytes = (jint*)malloc(sizeof(jint)*len);
        convertToJintArray(imageBytes, convertedImageBytes, len);
        free(imageBytes);
        free(convertedImageBytes);

        measurements[r] = (jlong) (currentTimeInMs() - tmp);
    }

    //sleep((unsigned int)sleepInSeconds);

    /*jint *convertedImageBytes = (jint *) malloc(sizeof(jint *) * len);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, convertedImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, convertedImageBytes, 0);
     */

    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    jlongArray out;
    out = env->NewLongArray(rounds);
    env->SetLongArrayRegion(out, 0, rounds, measurements);
    return out;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1Gmp_decryptImageBytesCipher1Rounds(JNIEnv *env,
                                                                                           jobject instance,
                                                                                           jintArray originalImageBytes_,
                                                                                           jlong sumOfImageBytes, jint rounds) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    int len = env->GetArrayLength(originalImageBytes_);

    unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    useImageCipher1(ENC_MODE, imageBytes, len, sumOfBytes, (int)rounds);

    jint* convertedImageBytes = (jint*)malloc(sizeof(jint*)*len);
    convertToJintArray(imageBytes, convertedImageBytes, len);
    free(imageBytes);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, convertedImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, convertedImageBytes, 0);
    return originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1Gmp_encryptImageBytesCipher1(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    long len = env->GetArrayLength(originalImageBytes_);

    unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    useImageCipher1(DEC_MODE, imageBytes, len, sumOfBytes, 1);

    jint* convertedImageBytes = (jint*)malloc(sizeof(jint*)*len);
    convertToJintArray(imageBytes, convertedImageBytes, len);
    free(imageBytes);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, convertedImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, convertedImageBytes, 0);
    return  originalImageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher1Gmp_decryptImageBytesCipher1(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jintArray originalImageBytes_,
                                                                                 jlong sumOfImageBytes) {
    jint *originalImageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    int len = env->GetArrayLength(originalImageBytes_);

    unsigned char *imageBytes = (unsigned char*)malloc(sizeof(unsigned char)*len);
    long sumOfBytes = (long)sumOfImageBytes;

    convertToUnsignedCharArray(imageBytes, originalImageBytes, len);
    env->ReleaseIntArrayElements(originalImageBytes_, originalImageBytes, 0);

    useImageCipher1(ENC_MODE, imageBytes, len, sumOfBytes, 1);

    jint* convertedImageBytes = (jint*)malloc(sizeof(jint*)*len);
    convertToJintArray(imageBytes, convertedImageBytes, len);
    free(imageBytes);

    env->SetIntArrayRegion(originalImageBytes_, 0, len, convertedImageBytes);
    env->ReleaseIntArrayElements(originalImageBytes_, convertedImageBytes, 0);
    return originalImageBytes_;
}

static char *key = (char *) "1234578901234567890123456789012";
unsigned char iv[] = {
        34, 45, 56, 78, 90, 12, 34, 23, 56, 78, 9, 3, 5, 23, 87,
        3, 4, 5, 1, 9, 8, 34, 89, 34, 22, 93, 75, 76, 23, 16, 39, 53
};

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2Gmp_encryptImageBytesCipher2Rounds(JNIEnv *env,
                                                                                           jobject instance,
                                                                                           jintArray imageBytes_, jint rounds) {
    jint *imageBytes = env->GetIntArrayElements(imageBytes_, NULL);
    int len = env->GetArrayLength(imageBytes_);

    AlgorithmParameter params = generateInitialContitions((unsigned char*)key);

    int block, blockPos, bytesProcessed;

    unsigned char buffer[BUFFER_SIZE];

    for(int r = 0; r < (int)rounds; r++) {

        block = 0;
        blockPos = 0;
        bytesProcessed = 0;
        while (bytesProcessed < len) {

            blockPos = 0;
            while (blockPos < BUFFER_SIZE && bytesProcessed < len) {
                buffer[blockPos] = (unsigned char) imageBytes[bytesProcessed];
                blockPos++;
                bytesProcessed++;
            }

            encrypt(&params, buffer, blockPos, (unsigned char *) key, iv);

            for (int i = 0; i < blockPos; i++) {
                imageBytes[block * BUFFER_SIZE + i] = buffer[i];
            }

            block++;
        }
    }

    env->SetIntArrayRegion(imageBytes_, 0, len, imageBytes);
    env->ReleaseIntArrayElements(imageBytes_, imageBytes, 0);
    return imageBytes_;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2Gmp_decryptImageBytesCipher2Rounds(JNIEnv *env,
                                                                                           jobject instance,
                                                                                           jintArray imageBytes_, jint rounds) {
    jint *imageBytes = env->GetIntArrayElements(imageBytes_, NULL);
    int len = env->GetArrayLength(imageBytes_);

    AlgorithmParameter params = generateInitialContitions((unsigned char*)key);

    int block, blockPos, bytesProcessed;

    unsigned char buffer[BUFFER_SIZE];

    for(int r = 0; r < (int)rounds; r++) {

        block = 0;
        blockPos = 0;
        bytesProcessed = 0;
        while (bytesProcessed < len) {

            blockPos = 0;
            while (blockPos < BUFFER_SIZE && bytesProcessed < len) {
                buffer[blockPos] = (unsigned char) imageBytes[bytesProcessed];
                blockPos++;
                bytesProcessed++;
            }

            decrypt(&params, buffer, blockPos, (unsigned char *) key, iv);

            for (int i = 0; i < blockPos; i++) {
                imageBytes[block * BUFFER_SIZE + i] = buffer[i];
            }

            block++;
        }
    }

    env->SetIntArrayRegion(imageBytes_, 0, len, imageBytes);
    env->ReleaseIntArrayElements(imageBytes_, imageBytes, 0);
    return imageBytes_;
}


extern "C"
JNIEXPORT jintArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2Gmp_encryptImageBytesCipher2(JNIEnv *env,
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

        encrypt(&params, buffer, blockPos, (unsigned char*)key, iv);

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
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2Gmp_decryptImageBytesCipher2(JNIEnv *env,
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

        decrypt(&params, buffer, blockPos, (unsigned char*)key, iv);

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
JNIEXPORT jlongArray JNICALL
Java_at_fhjoanneum_platzerf_imageciphercomparison_ImageCipher2Gmp_runImageBytesCipher2GmpLong(JNIEnv *env,
                                                                                           jobject instance,
                                                                                           jintArray originalImageBytes_,
                                                                                           jint rounds,
                                                                                           jint sleepInSeconds,
                                                                                           jint mode) {
    jint *imageBytes = env->GetIntArrayElements(originalImageBytes_, NULL);
    int len = env->GetArrayLength(originalImageBytes_);

    jlong measurements[rounds];

    long tmp;
    //sleep((unsigned int)sleepInSeconds);

    for(int r = 0; r < (int)rounds; r++) {

        tmp = currentTimeInMs();

        char *key = (char *) "1234578901234567890123456789012";
        unsigned char iv[] = {
                34, 45, 56, 78, 90, 12, 34, 23, 56, 78, 9, 3, 5, 23, 87,
                3, 4, 5, 1, 9, 8, 34, 89, 34, 22, 93, 75, 76, 23, 16, 39, 53
        };

        int block, blockPos, bytesProcessed;

        unsigned char buffer[BUFFER_SIZE];

        AlgorithmParameter params = generateInitialContitions((unsigned char*)key);

        block = 0;
        blockPos = 0;
        bytesProcessed = 0;
        while (bytesProcessed < len) {

            blockPos = 0;
            while (blockPos < BUFFER_SIZE && bytesProcessed < len) {
                buffer[blockPos] = (unsigned char) imageBytes[bytesProcessed];
                blockPos++;
                bytesProcessed++;
            }

            if(mode == 1)
                encrypt(&params, buffer, blockPos, (unsigned char *) key, iv);
            else if(mode == 2)
                decrypt(&params, buffer, blockPos, (unsigned char *) key, iv);

            for (int i = 0; i < blockPos; i++) {
                imageBytes[block * BUFFER_SIZE + i] = buffer[i];
            }

            block++;
        }

        measurements[r] = (jlong) (currentTimeInMs() - tmp);
    }

    //sleep((unsigned int)sleepInSeconds);

    jlongArray out;
    out = env->NewLongArray(rounds);
    env->SetLongArrayRegion(out, 0, rounds, measurements);
    return out;
}
