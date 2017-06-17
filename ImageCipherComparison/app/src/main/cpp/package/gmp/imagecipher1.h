#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../../distribution/gmp/include/gmp.h"

#define DEV 1
#define TEST 1

#ifdef DEV
    #define PTF(A,...) gmp_printf(A,##__VA_ARGS__);
#else
    #define PTF(A,...) ;
#endif

#ifdef TEST
    #define PTF_IMPT(A,...) gmp_printf(A,##__VA_ARGS__);
#else
    #define PTF_IMPT(A,...) ;
#endif

#ifndef ENC_MODE
#define ENC_MODE 1
#endif

#ifndef DEC_MODE
#define DEC_MODE 2
#endif

typedef struct PermutationSetups {
    mpf_t r; // 3.6 <= r <= 4.0
    mpf_t x; // 0 < x < 1
} PermutationSetup;

typedef struct DiffusionSetups {
    mpf_t miu; // 0.6 < miu <= 1.0
    mpf_t y; // 0 < y < 1
    mpf_t x; // 0 < x < 1
} DiffusionSetup;

// run the cipher the the selected mode
void runAlgorithm(int mode, unsigned char *imageBytes, long numberOfImageBytes, long sumOfAllImageBytes, PermutationSetup permutationSetups[4], DiffusionSetup diffusionSetups[2], int encryptionRounds);
