#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "imagecipher1.h"

void printSequence(double a[], long n);
void sort(double a[], long n);
int find(double array[], double data, long length);

void createPermutationSequence(int permutationSequence[], double r, double x, long sequenceLength);
double generateControlParametersLogisticMap(double basicR, double avgOfImageByteSum, long numberOfImageBytes);
void createDiffusionSequenceIkedaMap(double miu, double x, double y, unsigned char mOneSequence[], unsigned char mTwoSequence[], long sequenceLength);
double generateControlParametersIkedaMap(double miu, double avgOfImageByteSum, long numberOfImageBytes);

void runAlgorithm(int mode, unsigned char *imageBytes, long numberOfImageBytes, long sumOfAllImageBytes, PermutationSetup permutationSetups[4], DiffusionSetup diffusionSetups[2], int encryptionRounds) {

    // copy setups so they are not changed
    PermutationSetup permSetups[4];
    DiffusionSetup diffuSetups[2];

    for(int i = 0; i < 4; i ++) {
        permSetups[i].r = permutationSetups[i].r;
        permSetups[i].x = permutationSetups[i].x;
    }
    for(int i = 0; i < 2; i ++) {
        diffuSetups[i].miu = diffusionSetups[i].miu;
        diffuSetups[i].x = diffusionSetups[i].x;
        diffuSetups[i].y = diffusionSetups[i].y;
    }

    #ifdef TEST
    char *modeDesc = "encryption";
    if(mode == DEC_MODE)
        modeDesc = "decryption";

    PTF_IMPT("\n--- running %s mode ---\n", modeDesc);

    PTF_IMPT("\n----------- input Image [");
    for(long j = 0; j < numberOfImageBytes; j++) {
        PTF_IMPT("%u, ", imageBytes[j]);
    }
    PTF_IMPT("] -------------------\n");

    PTF_IMPT("Permutation setups: \n");
    for(int i = 0; i < 4; i ++) {
        PTF_IMPT("%d setup\n   r = %0.20f\n   x = %0.20f\n", i, permSetups[i].r, permSetups[i].x);
    }
    PTF_IMPT("-------------------\n");

    PTF_IMPT("Diffusion setups: \n");
    for(int i = 0; i < 2; i ++) {
        PTF_IMPT("%d setup\n   miu = %0.20f\n   x = %0.20f\n   y = %0.20f\n", i, diffuSetups[i].miu, diffuSetups[i].x, diffuSetups[i].y);
    }
    PTF_IMPT("-------------------\n");

    PTF_IMPT("Image size (number of bytes) = %ld\n", numberOfImageBytes);
    #endif

    int permutationSequenceLogisticMap[4][numberOfImageBytes];
    unsigned char diffustionSequenceIkedaMap[4][numberOfImageBytes];

    double avg = 0;
    PTF_IMPT("Sum of bytes = %ld\n", sumOfAllImageBytes);

    avg = ((double)sumOfAllImageBytes) / (double)(numberOfImageBytes * 63 * 10);
    PTF("Average = %.15f\n", avg);

    // 1. generate control parameters for logistic map based on image
    PTF("\n-------------Permutation Parameters\n");
    for(int i = 0; i < 4; i++) {
        permSetups[i].r = generateControlParametersLogisticMap(permSetups[i].r, avg, numberOfImageBytes);
        PTF("r%d = %.15f\n", i, permSetups[i].r);
    }
    PTF("-------------\n");

    // 2. create permutation = fill permutation array
    PTF("\n-------------Permutation Sequences \n");
    for(int i = 0; i < 4; i++) {
        createPermutationSequence(permutationSequenceLogisticMap[i], permSetups[i].r, permSetups[i].x, numberOfImageBytes);

        #ifdef DEV
        PTF("\nPermutation Sequence %d: \n [", i);
        for(int j = 0; j < 18; j++) {
            PTF("%d ", permutationSequenceLogisticMap[i][j]);
        }
        PTF(" ]-------------------\n");
        #endif
    }


    // 3. generate control parameters for ikeda map based on image
    PTF("\n-------------Diffustion Parameters\n");
    for(int i = 0; i < 2; i++) {
        diffuSetups[i].miu = generateControlParametersIkedaMap(diffuSetups[i].miu, avg, numberOfImageBytes);
        PTF("miu%d = %.15f\n", i, diffuSetups[i].miu);
    }
    PTF("-------------\n");


    // 4. create ikeda map diffusion sequence
    PTF("\n-------------Diffustion Sequences \n");
    for(int i = 0; i < 2; i++) {
        createDiffusionSequenceIkedaMap(diffuSetups[i].miu, diffuSetups[i].x, diffuSetups[i].y, diffustionSequenceIkedaMap[i*2], diffustionSequenceIkedaMap[(i*2)+1], numberOfImageBytes);

        #ifdef DEV
        PTF("\nDiffusion Sequence %d: \n [ ", i*2);
        for(int j = 0; j < 18; j++) {
            PTF("%d ", diffustionSequenceIkedaMap[i*2][j]);
        }
        PTF(" ]-------------------\n");

        PTF("\nDiffusion Sequence %d: \n [ ", (i*2)+1);
        for(int j = 0; j < 18; j++) {
            PTF("%d ", diffustionSequenceIkedaMap[(i*2)+1][j]);
        }
        PTF(" ]-------------------\n");
        #endif
    }

    if(mode == ENC_MODE) {
        // 5. Encryption rounds
        unsigned char *tmpImageBytes = (unsigned char*)malloc(sizeof(unsigned char)*numberOfImageBytes);
        //memcpy(tmpImageBytes, imageBytes, numberOfImageBytes * sizeof(unsigned char));

        long k, j;
        for(int i = 0; i < encryptionRounds; i++) {

            for(k = 0; k < 4; k++) {
                PTF("\n----------- round %d after permutation %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    tmpImageBytes[j] = imageBytes[permutationSequenceLogisticMap[k][j]];
                    PTF("%u, ", tmpImageBytes[j]);
                }
                PTF("] \n");

                PTF("\n----------- round %d after diffustion %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    imageBytes[j] = tmpImageBytes[j]^diffustionSequenceIkedaMap[k][j];
                    PTF("%u, ", imageBytes[j]);
                }
                PTF("] \n");

                //memcpy(tmpImageBytes, imageBytes, numberOfImageBytes * sizeof(unsigned char));
            }
        }

        free(tmpImageBytes);
    }
    else if(mode == DEC_MODE) {
        // 5. decryption rounds
        unsigned char *tmpImageBytes = (unsigned char*)malloc(sizeof(unsigned char)*numberOfImageBytes);
        //memcpy(tmpImageBytes, imageBytes, numberOfImageBytes * sizeof(unsigned char));

        long k, j;
        for(int i = 0; i < encryptionRounds; i++) {

            for(k = 3; k >= 0; k--) {

                PTF("\n----------- round %d after permutation %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    tmpImageBytes[j] = imageBytes[j]^diffustionSequenceIkedaMap[k][j];
                    PTF("%u, ", tmpImageBytes[j]);
                }
                PTF("] \n");

                PTF("\n----------- round %d after diffustion %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    imageBytes[permutationSequenceLogisticMap[k][j]] = tmpImageBytes[j];
                    PTF("%u, ", imageBytes[j]);
                }
                PTF("] \n");

                //memcpy(tmpImageBytes, imageBytes, numberOfImageBytes * sizeof(unsigned char));
            }
        }

        free(tmpImageBytes);
    }

    #ifdef TEST
    PTF_IMPT("\n----------- output Image [");
    for(int j = 0; j < numberOfImageBytes; j++) {
        PTF_IMPT("%u, ", imageBytes[j]);
    }
    PTF_IMPT("] -------------------\n");
    #endif
}

void createDiffusionSequenceIkedaMap(double miu, double x, double y, unsigned char mOneSequence[], unsigned char mTwoSequence[], long sequenceLength){
    int entriesToSkip = 1000;
    double multiply = pow(10.0, 16);
    double absX;
    double absY;
    double xn = x;
    double yn = y;
    double tn;
    double cosT;
    double sinT;

    PTF("--------- Creating Diffusion Sequence ---------\n")

    // calculate chaotic map sequences
    for(long i = 0; i < entriesToSkip + sequenceLength; i++) {
        tn = 0.4 - (6.0 / (1.0 + xn*xn + yn*yn));

        cosT = cos(tn);
        sinT = sin(tn);

        xn = 1.0 + miu * ((xn * cosT) - (yn * sinT));
        yn = miu * ((xn * sinT) - (yn * cosT));

        if(i >= entriesToSkip) {
            absX = fabs(xn);
            absY = fabs(yn);
            mOneSequence[i-entriesToSkip] = ((long long)((absX - ((double)floor(absX))) * multiply)) % 255;
            mTwoSequence[i-entriesToSkip] = ((long long)((absY - ((double)floor(absY))) * multiply)) % 255;

            //PTF("%d - xn: %.20f yn: %.20f m1: %d\n", i, xn, yn, mOneSequence[i-entriesToSkip]);
            //PTF("%d - xn: %.20f yn: %.20f m1: %d\n", i, xn, yn, mTwoSequence[i-entriesToSkip]);
        }
    }
}

double generateControlParametersIkedaMap(double miu, double avgOfImageByteSum, long numberOfImageBytes) {
    double r = 0.0;

    if(numberOfImageBytes <= 1000000)
        r = (miu+0.0)+(1.0-avgOfImageByteSum)/2;
    else if(numberOfImageBytes > 1000000 && numberOfImageBytes <= 4000000)
        r = (miu+0.1)+(1.0-avgOfImageByteSum)/2;
    else if(numberOfImageBytes > 4000000)
        r = (miu+0.2)+(1.0-avgOfImageByteSum)/2;

	return r;
}

double generateControlParametersLogisticMap(double basicR, double avgOfImageByteSum, long numberOfImageBytes) {
    double r = 0.0;

    if(numberOfImageBytes <= 1000000)
        r = (basicR+0.0)+(0.4-avgOfImageByteSum);
    else if(numberOfImageBytes > 1000000 && numberOfImageBytes <= 4000000)
        r = (basicR+0.1)+(0.4-avgOfImageByteSum);
    else if(numberOfImageBytes > 4000000)
        r = (basicR+0.2)+(0.4-avgOfImageByteSum);

	return r;
}

void createPermutationSequence(int permutationSequence[], double r, double x, long sequenceLength) {
    double sequenceC[sequenceLength];
    double sequenceS[sequenceLength];
    double xn = x;

    // create original chaotic sequence (skip 1st 1000 entries)
    int transientResultsToSkip = 1000;
    for(long i = 0; i < transientResultsToSkip + sequenceLength; i++) {
        xn = r * xn * (1 - xn);
        if(i >= transientResultsToSkip)
            sequenceC[i-transientResultsToSkip] = xn;
    }
    /*
    PTF("original sequence C\n");
    printSequence(sequenceC, 10);
    */

    // create sorted sequence S based on sequence C
    memcpy(sequenceS, sequenceC, sequenceLength * sizeof(double));

    sort(sequenceS, sequenceLength);

    /*
    PTF("sorted sequence S\n");
    printSequence(sequenceS, 10);
    */

    // better allocation (use malloc)
    double groupedArrays[10][sequenceLength];
    int lastGroupedArrayPosition[10];

    // initialize arrays
    int j;
    for(int i = 0; i < 10; i++) {
        lastGroupedArrayPosition[i] = 0;

        for(j = 0; j < sequenceLength; j++) {
            groupedArrays[i][j] = -1;
        }
    }

    // create grouped arrays based on sequence s
    double tmpTimes;
    int groupNumber, tmpResult1, tmpResult2;

    for(long i = 0; i < sequenceLength; i++) {

        tmpTimes = sequenceS[i]*1000000;
        // get group number
        tmpResult1 = (((int)floor(tmpTimes)) % 10);
        tmpResult2 = (((int)floor(tmpTimes * 1000)) % 10);

        if(tmpResult1 >= tmpResult2)
            groupNumber = tmpResult1-tmpResult2;
        if(tmpResult2 >= tmpResult1)
            groupNumber = tmpResult2-tmpResult1;

        if(groupNumber < 0 || groupNumber > 9)
            return;

        // set value into appropriate group array
        groupedArrays[groupNumber][lastGroupedArrayPosition[groupNumber]++] = sequenceS[i];
    }

    long permutationIndex = 0;

    for(int i = 0; i < 10; i++) {
        if(permutationIndex >= sequenceLength)
            break;

        j = 0;
        while(groupedArrays[i][j] > 0) {
            permutationSequence[permutationIndex++] = find(sequenceS, groupedArrays[i][j], sequenceLength);
            j++;
        }
    }
}

void printSequence(double a[], long n) {
    #ifdef DEV
    for(int i = 0; i < n; i++) {
        PTF("%d - %.15f\n", i, a[i]);
    }
    PTF("-------------------\n");
    #endif
}

void sort(double array[], long n){
    long c, d;
    double t;

    for (c = 1 ; c <= n - 1; c++) {
        d = c;

        while ( d > 0 && array[d] < array[d-1]) {
          t          = array[d];
          array[d]   = array[d-1];
          array[d-1] = t;

          d--;
        }
    }
}

int find(double array[], double data, long length) {
   int lowerBound = 0;
   int upperBound = length -1;
   int midPoint = -1;
   int comparisons = 0;
   int index = -1;

   while(lowerBound <= upperBound) {
      //PTF("Comparison %d\n" , (comparisons +1) );
      //PTF("lowerBound : %d, array[%d] = %.15f\n",lowerBound,lowerBound,
      //   array[lowerBound]);
      //PTF("upperBound : %d, array[%d] = %.15f\n",upperBound,upperBound,
      //   array[upperBound]);
      comparisons++;

      // compute the mid point
      // midPoint = (lowerBound + upperBound) / 2;
      midPoint = lowerBound + (upperBound - lowerBound) / 2;

      // data found
      if(array[midPoint] == data) {
         index = midPoint;
         break;
      } else {
         // if data is larger
         if(array[midPoint] < data) {
            // data is in upper half
            lowerBound = midPoint + 1;
         }
         // data is smaller
         else {
            // data is in lower half
            upperBound = midPoint -1;
         }
      }
   }
   //PTF("Total comparisons made: %d" , comparisons);
   return index;
}
