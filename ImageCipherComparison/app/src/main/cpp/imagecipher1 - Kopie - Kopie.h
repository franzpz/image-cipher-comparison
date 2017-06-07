#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//#define DEV 1
//#define TEST 1

#ifdef DEV
    #define PTF(A,...) printf(A,##__VA_ARGS__);
#else
    #define PTF(A,...) do {} while(0);
#endif

#ifdef TEST
    #define PTF_IMPT(A,...) printf(A,##__VA_ARGS__);
#else
    #define PTF_IMPT(A,...) do {} while(0);
#endif

static int ENC_MODE = 1;
static int DEC_MODE = 2;

typedef struct PermutationSetups {
    double r; // 3.6 <= r <= 4.0
    double x; // 0 < x < 1
} PermutationSetup;

typedef struct DiffusionSetups {
    double miu; // 0.6 < miu <= 1.0
    double y; // 0 < y < 1
    double x; // 0 < x < 1
} DiffusionSetup;

void runAlgorithm(int mode, unsigned char imageBytes[], long numberOfImageBytes, long sumOfAllImageBytes, PermutationSetup permutationSetups[4], DiffusionSetup diffusionSetups[2], int encryptionRounds);
