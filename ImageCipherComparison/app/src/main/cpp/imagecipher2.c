#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "imagecipher2.h"

static double LOGISTIC_R = 3.712345;
static double DIVISOR_M1 = 1000.0;
static double PRECISION = 10000000000.0;

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

double convertM1(double x) {
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
        //r += ((double)key[i]) / DIVISOR_M1;
        param.C = (param.C + key[i]) % 256;
    }

    param.X = r - floor(r);
    double xn = param.X;

    // skip first 1000 Logistic Map repititions
    for(int i = 0; i < 1000; i++)
        xn = LOGISTIC_R * xn * (1.0 - xn);

    param.X = xn;

    return param;
}

void encrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char key[KEY_SIZE]) {

    if(numberOfImageBytes > BUFFER_SIZE)
        exit(1);

    double x = params->X;
    unsigned char lastC = params->C;

    double xn;
    double logisticSum;
    int numberOfLogisticMapRepititions;
    int nextKeyPos;

    for(int l = 0; l < numberOfImageBytes; l++) {
        nextKeyPos = (l+1) % KEY_SIZE;

        x = convertM1((double)x +(double)lastC+(double)key[l]);
        //x = ((double)x +(double)lastC+(double)key[l]) / DIVISOR_M1;
        x = round(x * PRECISION) / PRECISION;

        numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

        xn = x;
        logisticSum = 0.0;
        for(int i = 0; i < numberOfLogisticMapRepititions; i++) {
            xn = LOGISTIC_R * xn * (1.0 - xn);
            logisticSum += xn;
        }

        //imageBytes[l] = (((int)imageBytes[l]) + ((int)logisticSum % 256)) % 256;
        imageBytes[l] = (((int)imageBytes[l]) + convertM2(logisticSum)) % 256;
        lastC = imageBytes[l];
    }

    params->X = x;
    params->C = lastC;
}

void decrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char key[KEY_SIZE]) {

    if(numberOfImageBytes > BUFFER_SIZE)
        exit(1);

    double x = params->X;
    unsigned char lastC = params->C;

    double xn;
    double logisticSum;
    int numberOfLogisticMapRepititions;
    int nextKeyPos;

    for(int l = 0; l < BUFFER_SIZE; l++) {
        nextKeyPos = (l+1) % KEY_SIZE;

        x = ((double)x + (double)lastC + (double)key[l]) / DIVISOR_M1;
        x = round(x * PRECISION) / PRECISION;

        numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

        xn = x;
        logisticSum = 0.0;
        for(int i = 0; i < numberOfLogisticMapRepititions; i++) {
            xn = LOGISTIC_R * xn * (1.0 - xn);
            logisticSum += xn;
        }

        lastC = imageBytes[l];
        imageBytes[l] = (((int)imageBytes[l]) - (((int)logisticSum)%256)) % 256;
    }

    params->X = x;
    params->C = lastC;
}
