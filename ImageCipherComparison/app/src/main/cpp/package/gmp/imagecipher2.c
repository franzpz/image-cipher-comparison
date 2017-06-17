#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../../distribution/gmp/include/gmp.h"
#include "imagecipher2.h"

// constant for running logistic map
static double LOGISTIC_R = 3.712345;

// used to cut of incorrect precision values on double values
// used (int)(value/PRECISION) / PRECISION
static double PRECISION = 1000000000000.0;

// used to convert the 0-255 value space to 0-1 value space
static int DIVIDER = 1000;

AlgorithmParameter generateInitialContitions(unsigned char key[KEY_SIZE]) {
    AlgorithmParameter param;

    mpf_t r, floored_r, divider, mpf_key;
    mpf_inits(r, floored_r, divider, mpf_key, param.X, NULL);

    mpf_set_ui(divider, (unsigned long int)DIVIDER);

    param.C = 0;

    for(int i = 0; i < KEY_SIZE; i++) {
        mpf_set_ui(mpf_key, (int)key[i]);
        mpf_div(mpf_key, mpf_key, divider);

        PTF("key = %.Ff\n", mpf_key);

        mpf_add(r, r, mpf_key);

        PTF("r = %.Ff\n", r);

        param.C = (param.C + key[i]) % 256;
    }

    mpf_floor(floored_r, r);
    PTF("floored_r = %.Ff\n", floored_r);
    mpf_sub(param.X, r, floored_r);

    mpf_clears(r, floored_r, divider, mpf_key, NULL);

    //param.X = r - floor(r);
    //double xn = param.X;

    // skip first 1000 Logistic Map repititions
    //for(int i = 0; i < 1000; i++)
    //    xn = LOGISTIC_R * xn * (1.0 - xn);

    //param.X = xn;

    return param;
}

void encrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char key[KEY_SIZE], unsigned char *iv) {

    if(numberOfImageBytes > BUFFER_SIZE)
        exit(1);

    mpf_t x, xn, mpf_key, logisticSum, mpf_logistic_r, logistic_tmp;
    mpf_inits(x, xn, mpf_key, logisticSum, mpf_logistic_r, logistic_tmp, NULL);

    mpf_set(x, params->X);
    mpf_set_d(mpf_logistic_r, LOGISTIC_R);

    unsigned char lastC = params->C;

    int numberOfLogisticMapRepititions;
    int nextKeyPos;

    for(int l = 0; l < numberOfImageBytes; l++) {
        // start at key pos 0 again after reaching end of key
        nextKeyPos = (l+1) % KEY_SIZE;

        mpf_set_ui(mpf_key, (int)key[l]);

        mpf_add_ui(mpf_key, mpf_key, (int)lastC);
        mpf_add(x, x, mpf_key);
        mpf_div_ui(x, x, DIVIDER);

        PTF("index: %d, x = %.Ff\n", l, x);

        //x = convertM1((double)x +(double)lastC+(double)key[l]);
        //x = round(x * PRECISION) / PRECISION;

        numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

        mpf_set(xn, x);
        mpf_set_ui(logisticSum, 0);
        for(int i = 0; i < numberOfLogisticMapRepititions; i++) {

            // xn = LOGISTIC_R * xn * (1.0 - xn);
            mpf_ui_sub(logistic_tmp, 1, xn);
            mpf_mul(xn, mpf_logistic_r, xn);
            mpf_mul(xn, xn, logistic_tmp);

            mpf_add(logisticSum, logisticSum, xn);
        }

        PTF("logisticSum = %.Ff\n", logisticSum);

        PTF("IV bytes %u XOR %u = ", imageBytes[l], iv[l]);
        // apply CBC before encryption
        imageBytes[l] = iv[l]^imageBytes[l];
        PTF("%u\n", imageBytes[l]);

        // apply encryption
        imageBytes[l] = (((int)imageBytes[l]) + ((int)mpf_get_ui(logisticSum)) % 256) % 256;
        lastC = imageBytes[l];

        // set iv for next encryption round
        iv[l] = lastC;
    }

    mpf_set(params->X, x);
    params->C = lastC;

    mpf_clears(x, xn, mpf_key, logisticSum, mpf_logistic_r, logistic_tmp, NULL);
}

// same as encryption, except convertedBytes = origBytes - convertM2(logisticSum)
// copied, to avoid if in every iteration = better performance
void decrypt(AlgorithmParameter *params, unsigned char *imageBytes, int numberOfImageBytes, unsigned char key[KEY_SIZE], unsigned char *iv) {

    if(numberOfImageBytes > BUFFER_SIZE)
        exit(1);

    mpf_t x, xn, mpf_key, logisticSum, mpf_logistic_r, logistic_tmp;
    mpf_inits(x, xn, mpf_key, logisticSum, mpf_logistic_r, logistic_tmp, NULL);

    mpf_set(x, params->X);
    mpf_set_d(mpf_logistic_r, LOGISTIC_R);

    unsigned char lastC = params->C;

    int numberOfLogisticMapRepititions;
    int nextKeyPos;

    for(int l = 0; l < numberOfImageBytes; l++) {
        // start at key pos 0 again after reaching end of key
        nextKeyPos = (l+1) % KEY_SIZE;

        mpf_set_ui(mpf_key, (int)key[l]);

        mpf_add_ui(mpf_key, mpf_key, (int)lastC);
        mpf_add(x, x, mpf_key);
        mpf_div_ui(x, x, DIVIDER);

        PTF("index: %d, x = %.Ff\n", l, x);

        //x = convertM1((double)x +(double)lastC+(double)key[l]);
        //x = round(x * PRECISION) / PRECISION;

        numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

        mpf_set(xn, x);
        mpf_set_ui(logisticSum, 0);
        for(int i = 0; i < numberOfLogisticMapRepititions; i++) {

            // xn = LOGISTIC_R * xn * (1.0 - xn);
            mpf_ui_sub(logistic_tmp, 1, xn);
            mpf_mul(xn, mpf_logistic_r, xn);
            mpf_mul(xn, xn, logistic_tmp);

            mpf_add(logisticSum, logisticSum, xn);
        }

        PTF("logisticSum = %.Ff\n", logisticSum);

        lastC = imageBytes[l];
        imageBytes[l] = (((int)imageBytes[l]) - ((int)mpf_get_ui(logisticSum)) % 256) % 256;

        // apply reverse cbc for decryption
        PTF("IV bytes %u XOR %u = ", imageBytes[l], iv[l]);
        imageBytes[l] = iv[l]^imageBytes[l];
        PTF("%u\n", imageBytes[l]);

        // set iv for next decryption round
        iv[l] = lastC;
    }

    mpf_set(params->X, x);
    params->C = lastC;

    mpf_clears(x, xn, mpf_key, logisticSum, mpf_logistic_r, logistic_tmp, NULL);
}
