#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// when DEV is defined we get all output, useful for debugging
//#define DEV 1
// when TEST is defined we get minimum output, to verify expectations
#define TEST 1

// use floating point standard for precision
#ifndef __STDC_IEC_559__
    #define __STDC_IEC_559__ 1
#endif

#define BUFFER_SIZE 32
#define KEY_SIZE 32 // as unsigned char array = 256 bit

#ifdef DEV
    #define PTF(A,...) printf(A,##__VA_ARGS__);
#else
    #define PTF(A,...) ;
#endif

#ifdef TEST
    #define PTF_IMPT(A,...) printf(A,##__VA_ARGS__);
#else
    #define PTF_IMPT(A,...) ;
#endif

#ifndef ENC_MODE
#define ENC_MODE 1
#endif

#ifndef DEC_MODE
#define DEC_MODE 2
#endif

typedef struct AlgorithmParameters {
    double X;
    unsigned char C;
} AlgorithmParameter;

AlgorithmParameter generateInitialContitions(unsigned char *key);
void encrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char *key, unsigned char *iv);
void decrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char *key, unsigned char *iv);

// only in header file for testing
// convert a value from the logistic map to the range of an image byte = 0-255
unsigned char convertM2(double x);
// convert a value from image bytes (as double because calculations could have happenend before) to logistic map range 0-1
double convertM1(double x);
