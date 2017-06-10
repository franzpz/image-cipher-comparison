#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <android/log.h>

//#define DEV 1
//#define TEST 1

#ifndef __STDC_IEC_559__
    #define __STDC_IEC_559__ 1
#endif

#define BUFFER_SIZE 32
#define KEY_SIZE 32 // as unsigned char array = 256 bit

#define log_error_f(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)
#define log_error(tag,msg) __android_log_print(ANDROID_LOG_ERROR,tag,msg)
#define log_info_f(tag,fmt,...) __android_log_print(ANDROID_LOG_INFO,tag,fmt,__VA_ARGS__)
#define log_info(tag,msg) __android_log_print(ANDROID_LOG_INFO,tag,msg)
#define log_warn(tag,fmt,...) __android_log_print(ANDROID_LOG_ERROR,tag,fmt,__VA_ARGS__)

#ifdef DEV
#define PTF(A,...) __android_log_print(ANDROID_LOG_INFO,"Ciphers",A,##__VA_ARGS__);
#else
#define PTF(A,...) do {} while(0);
#endif

#ifdef TEST
#define PTF_IMPT(A,...) __android_log_print(ANDROID_LOG_INFO,"Ciphers",A,##__VA_ARGS__);
#else
#define PTF_IMPT(A,...) do {} while(0);
#endif

static int ENC_MODE = 1;
static int DEC_MODE = 2;

typedef struct AlgorithmParameters {
    double X;
    unsigned char C;
} AlgorithmParameter;

AlgorithmParameter generateInitialContitions(unsigned char *key);
void encrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char *key);
void decrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char *key);

unsigned char convertM2(double x);
double convertM1(double x);
