#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "imagecipher2.h"

// constant for running logistic map
static double LOGISTIC_R = 3.712345;

// used to cut of incorrect precision values on double values
// used (int)(value/PRECISION) / PRECISION
static double PRECISION = 1000000000000.0;

// convert a value from the logistic map to the range of an image byte = 0-255
unsigned char convertM2(double x) {

    return ((int)x) % 256;

    //return ((int)x) % 256;
    static double minLogistic = 0.0;
    static double maxLogistic = 1.0;
    static double rangeLogistic = 1.0 - 0.0;

    static double minImage = 0.0;
    static double maxImage = 255.0;
    static double rangeImage = 255.0 - 0.0;

    while(x > maxLogistic)
        x = x/10.0;

    return (unsigned char)((((x - minLogistic) * rangeImage) / rangeLogistic) + minImage);

    //return (unsigned char)(((int)((((x - minLogistic) * rangeImage) / rangeLogistic) + minImage)) % 256);
}

// convert a value from image bytes (as double because calculations could have happenend before) to logistic map range 0-1
double convertM1(double x) {
    static double DIVISOR_M1 = 1000.0;

    return x / DIVISOR_M1;

    static double minLogistic = 0.0;
    static double maxLogistic = 1.0;
    static double rangeLogistic = 1.0 - 0.0;

    static double minImage = 0.0;
    static double maxImage = 255.0;
    static double rangeImage = 255.0 - 0.0;

    x = ((int)x) % 256;

    return (double)((((x - minImage) * rangeLogistic) / rangeImage) + minLogistic);
}

AlgorithmParameter generateInitialContitions(unsigned char key[KEY_SIZE]) {
    AlgorithmParameter param;

    double r = 0;
    param.C = 0;

    for(int i = 0; i < KEY_SIZE; i++) {
        r += convertM1((double)key[i]);
        param.C = (param.C + key[i]) % 256;
    }

    param.X = r - floor(r);
    double xn = param.X;

    // skip first 1000 Logistic Map repititions
    //for(int i = 0; i < 1000; i++)
    //    xn = LOGISTIC_R * xn * (1.0 - xn);

    param.X = xn;

    return param;
}

void encrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char key[KEY_SIZE], unsigned char *iv) {

    if(numberOfImageBytes > BUFFER_SIZE)
        exit(1);

    double x = params->X;
    unsigned char lastC = params->C;

    double xn;
    double logisticSum;
    int numberOfLogisticMapRepititions;
    int nextKeyPos;

    for(int l = 0; l < numberOfImageBytes; l++) {
        // start at key pos 0 again after reaching end of key
        nextKeyPos = (l+1) % KEY_SIZE;

        x = convertM1((double)x +(double)lastC+(double)key[l]);
        x = round(x * PRECISION) / PRECISION;

        numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

        xn = x;
        logisticSum = 0.0;
        for(int i = 0; i < numberOfLogisticMapRepititions; i++) {
            xn = LOGISTIC_R * xn * (1.0 - xn);
            logisticSum += xn;
        }

        PTF("logisticSum = %.15f\n", logisticSum);

        PTF("IV bytes %u XOR %u = ", imageBytes[l], iv[l]);
        // apply CBC before encryption
        imageBytes[l] = iv[l]^imageBytes[l];
        PTF("%u\n", imageBytes[l]);

        imageBytes[l] = (((int)imageBytes[l]) + convertM2(logisticSum)) % 256;
        lastC = imageBytes[l];

        // set iv for next encryption round
        iv[l] = lastC;
    }

    params->X = x;
    params->C = lastC;
}

// same as encryption, except convertedBytes = origBytes - convertM2(logisticSum)
// copied, to avoid if in every iteration = better performance
void decrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char key[KEY_SIZE], unsigned char *iv) {

       if(numberOfImageBytes > BUFFER_SIZE)
        exit(1);

    double x = params->X;
    unsigned char lastC = params->C;

    double xn;
    double logisticSum;
    int numberOfLogisticMapRepititions;
    int nextKeyPos;

    for(int l = 0; l < numberOfImageBytes; l++) {
        // start at key pos 0 again after reaching end of key
        nextKeyPos = (l+1) % KEY_SIZE;

        x = convertM1((double)x +(double)lastC+(double)key[l]);
        x = round(x * PRECISION) / PRECISION;

        numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

        xn = x;
        logisticSum = 0.0;
        for(int i = 0; i < numberOfLogisticMapRepititions; i++) {
            xn = LOGISTIC_R * xn * (1.0 - xn);
            logisticSum += xn;
        }

        PTF("logisticSum = %.Ff\n", logisticSum);

        lastC = imageBytes[l];
        imageBytes[l] = (((int)imageBytes[l]) - convertM2(logisticSum)) % 256;

        // apply reverse cbc for decryption
        PTF("IV bytes %u XOR %u = ", imageBytes[l], iv[l]);
        imageBytes[l] = iv[l]^imageBytes[l];
        PTF("%u\n", imageBytes[l]);

        // set iv for next decryption round
        iv[l] = lastC;
    }

    params->X = x;
    params->C = lastC;
}
