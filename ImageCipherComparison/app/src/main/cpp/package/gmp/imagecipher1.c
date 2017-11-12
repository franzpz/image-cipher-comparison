#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../../../../../distribution/gmp/include/gmp.h"
#include "imagecipher1.h"

// helper functions
int find(mpf_t array[], mpf_t data, long length);

//  quickSort
//
//  This public-domain C implementation by Darel Rex Finley.
//
//  * Returns YES if sort was successful, or NO if the nested
//    pivots went too deep, in which case your array will have
//    been re-ordered, but probably not sorted correctly.
//
//  * This function assumes it is called with valid parameters.
//
//  * Example calls:
//    quickSort(&myArray[0],5); // sorts elements 0, 1, 2, 3, and 4
//    quickSort(&myArray[3],5); // sorts elements 3, 4, 5, 6, and 7
//    http://alienryderflex.com/quicksort/

int quickSort(mpf_t *arr, int elements) {

  #define  MAX_LEVELS  1000

  mpf_t piv;
  mpf_init(piv);
  int  beg[MAX_LEVELS], end[MAX_LEVELS], L, R, i = 0 ;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      mpf_set(piv, arr[L]);
      if (i==MAX_LEVELS-1) return -1;
      while (L<R) {
        while (mpf_cmp(arr[R], piv) >= 0 && L<R) R--; if (L<R) mpf_set(arr[L++], arr[R]);
        while (mpf_cmp(arr[L], piv) <= 0 && L<R) L++; if (L<R) mpf_set(arr[R--], arr[L]); }
      mpf_set(arr[L], piv); beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
    else {
      i--; }}

  mpf_clear(piv);
  return 0;
}

clock_t clockStart(char *message) {
    PTF_IMPT("\nstarting %s \n", message);
    return clock();
}

void clockStopAndWrite(char *message, clock_t begin) {
    PTF_IMPT("\n %s %0.15f seconds\n", message, (double)(clock() - begin) / CLOCKS_PER_SEC);
}

// actual cipher functions
// create permutation sequence based on logistic map
void createPermutationSequence(int permutationSequence[], mpf_t r, mpf_t x, long sequenceLength);
// generate permutation parameters based on image values
void generateControlParametersLogisticMap(mpf_t r, mpf_t avgOfImageByteSum, long numberOfImageBytes);

// create diffusion sequence based on Ikeda map, generates two sequences at a time mOneSequence and mTwoSequence
void createDiffusionSequenceIkedaMap(mpf_t miu, mpf_t x, mpf_t y, unsigned char mOneSequence[], unsigned char mTwoSequence[], long sequenceLength);

// generate diffustion parameters based on image values
void generateControlParametersIkedaMap(mpf_t miu, mpf_t avgOfImageByteSum, long numberOfImageBytes);

void runAlgorithm(int mode, unsigned char *imageBytes, long numberOfImageBytes, long sumOfAllImageBytes, PermutationSetup permutationSetups[4], DiffusionSetup diffusionSetups[2], int encryptionRounds) {

    // copy setups so they are not changed
    PermutationSetup *permSetups = (PermutationSetup *) malloc(sizeof(PermutationSetup)*4);
    mpf_inits(permSetups[0].r, permSetups[0].x, permSetups[1].r, permSetups[1].x, permSetups[2].r, permSetups[2].x, permSetups[3].r, permSetups[3].x, NULL);

    DiffusionSetup *diffuSetups = (DiffusionSetup *) malloc(sizeof(DiffusionSetup)*2);
    mpf_inits(diffuSetups[0].miu, diffuSetups[0].x, diffuSetups[0].y, diffuSetups[1].miu, diffuSetups[1].x, diffuSetups[1].y, NULL);

    for(int i = 0; i < 4; i ++) {
        mpf_set(permSetups[i].r, permutationSetups[i].r);
        mpf_set(permSetups[i].x, permutationSetups[i].x);
    }
    for(int i = 0; i < 2; i ++) {
        mpf_set(diffuSetups[i].miu, diffusionSetups[i].miu);
        mpf_set(diffuSetups[i].x, diffusionSetups[i].x);
        mpf_set(diffuSetups[i].y, diffusionSetups[i].y);
    }

    #ifdef TEST
    char *modeDesc = "encryption";
    if(mode == DEC_MODE)
        modeDesc = "decryption";

    PTF_IMPT("\n--- running %s mode ---\n", modeDesc);

    PTF("\n----------- input Image [");
    for(long j = 0; j < numberOfImageBytes; j++) {
        PTF("%u, ", imageBytes[j]);
    }
    PTF("] -------------------\n");

    PTF_IMPT("Permutation setups: \n");
    for(int i = 0; i < 4; i ++) {
        PTF_IMPT("%d setup\n   r = %0.Ff\n   x = %0.Ff\n", i, permSetups[i].r, permSetups[i].x);
    }
    PTF_IMPT("-------------------\n");

    PTF_IMPT("Diffusion setups: \n");
    for(int i = 0; i < 2; i ++) {
        PTF_IMPT("%d setup\n   miu = %0.Ff\n   x = %0.Ff\n   y = %0.Ff\n", i, diffuSetups[i].miu, diffuSetups[i].x, diffuSetups[i].y);
    }
    PTF_IMPT("-------------------\n");

    PTF_IMPT("Image size (number of bytes) = %ld\n", numberOfImageBytes);
    #endif

    /*int permutationSequenceLogisticMap[4][numberOfImageBytes];
    unsigned char diffustionSequenceIkedaMap[4][numberOfImageBytes];*/

    clock_t s = clockStart("start permutation");

    int **permutationSequenceLogisticMap = (int**)malloc(sizeof(int*)*4);
    unsigned char **diffustionSequenceIkedaMap = (unsigned char**)malloc(sizeof(unsigned char*)*4);

    for(int i = 0; i < 4; i++){
        permutationSequenceLogisticMap[i] = (int*)malloc(sizeof(int)*numberOfImageBytes);
        diffustionSequenceIkedaMap[i] = (unsigned char *)malloc(sizeof(unsigned char)*numberOfImageBytes);
    }

    mpf_t byteAverage;
    mpf_inits(byteAverage, NULL);

    PTF_IMPT("Sum of bytes = %ld\n", sumOfAllImageBytes);

    mpf_set_ui(byteAverage, sumOfAllImageBytes);
    mpf_div_ui(byteAverage, byteAverage, numberOfImageBytes * 63 * 10);

    PTF("Average = %.Ff\n", byteAverage);

    // 1. generate control parameters for logistic map based on image
    PTF("\n-------------Permutation Parameters\n");
    for(int i = 0; i < 4; i++) {
        generateControlParametersLogisticMap(permSetups[i].r, byteAverage, numberOfImageBytes);
        PTF("r%d = %.Ff\n", i, permSetups[i].r);
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

    clockStopAndWrite("done permutation", s);

    s = clockStart("start diffusion");

    // 3. generate control parameters for ikeda map based on image
    PTF("\n-------------Diffustion Parameters\n");
    for(int i = 0; i < 2; i++) {
        generateControlParametersIkedaMap(diffuSetups[i].miu, byteAverage, numberOfImageBytes);
        PTF("miu%d = %.Ff\n", i, diffuSetups[i].miu);
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

    clockStopAndWrite("done diffusion", s);

    s = clockStart("encryption/decryption");

    if(mode == ENC_MODE) {
        // 5. Encryption rounds
        unsigned char *tmpImageBytes = (unsigned char*)malloc(sizeof(unsigned char)*numberOfImageBytes);
        //memcpy(tmpImageBytes, imageBytes, numberOfImageBytes * sizeof(unsigned char));

        long k, j;
        for(int i = 0; i < encryptionRounds; i++) {

            for(k = 0; k < 4; k++) {
                PTF("\n----------- round %d after permutation %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    tmpImageBytes[j] = imageBytes[permutationSequenceLogisticMap[k][j]]^diffustionSequenceIkedaMap[k][j];
                    PTF("%u, ", tmpImageBytes[j]);
                }
                PTF("] \n");

/*
                PTF("\n----------- round %d after diffustion %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    imageBytes[j] = tmpImageBytes[j]^diffustionSequenceIkedaMap[k][j];
                    PTF("%u, ", imageBytes[j]);
                }
                PTF("] \n");
*/
                memcpy(imageBytes, tmpImageBytes, numberOfImageBytes * sizeof(unsigned char));
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
                    tmpImageBytes[permutationSequenceLogisticMap[k][j]] = imageBytes[j]^diffustionSequenceIkedaMap[k][j];
                    PTF("%u, ", tmpImageBytes[j]);
                }
                PTF("] \n");
/*
                PTF("\n----------- round %d after diffustion %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    imageBytes[permutationSequenceLogisticMap[k][j]] = tmpImageBytes[j];
                    PTF("%u, ", imageBytes[j]);
                }
                PTF("] \n");
                */

                memcpy(imageBytes, tmpImageBytes, numberOfImageBytes * sizeof(unsigned char));
            }
        }

        free(tmpImageBytes);
    }

    clockStopAndWrite("done enc/dec", s);

    mpf_clears(permSetups[0].r, permSetups[0].x, permSetups[1].r, permSetups[1].x, permSetups[2].r, permSetups[2].x, permSetups[3].r, permSetups[3].x, NULL);
    mpf_clears(diffuSetups[0].miu, diffuSetups[0].x, diffuSetups[0].y, diffuSetups[1].miu, diffuSetups[1].x, diffuSetups[1].y, NULL);

    free(permSetups);
    free(diffuSetups);

    for(int i = 0; i < 4; i++){
        free(permutationSequenceLogisticMap[i]);
        free(diffustionSequenceIkedaMap[i]);
    }

    free(permutationSequenceLogisticMap);
    free(diffustionSequenceIkedaMap);

    #ifdef DEV
    PTF("\n----------- output Image [");
    for(int j = 0; j < numberOfImageBytes; j++) {
        PTF("%u, ", imageBytes[j]);
    }
    PTF("] -------------------\n");
    #endif
}

void createDiffusionSequenceIkedaMap(mpf_t miu, mpf_t x, mpf_t y, unsigned char mOneSequence[], unsigned char mTwoSequence[], long sequenceLength){
    int entriesToSkip = 1000;

    mpf_t multiply, absX, absY, xn, yn, xn2, yn2, tn, cosT, sinT, helper1, helper2, tnBase;
    mpf_inits(multiply, absX, absY, xn, yn, xn2, yn2, tn, cosT, sinT, helper1, helper2, tnBase, NULL);

    mpf_set_d(tnBase, 0.4);
    mpf_set_ui(multiply, (unsigned long int)10000000000000000);
    mpf_set(xn, x);
    mpf_set(yn, y);

    double doubleTn, doubleSinT, doubleCosT;

    PTF("--------- Creating Diffusion Sequence ---------\n")

    // calculate chaotic map sequences
    for(long i = 0; i < entriesToSkip + sequenceLength; i++) {

        mpf_mul(helper1, helper1, helper1);
        mpf_mul(helper2, helper2, helper2);

        mpf_add(tn, helper1, helper2);
        mpf_add_ui(tn, tn, 1);
        mpf_ui_div(tn, 6, tn);
        mpf_sub(tn, tnBase, tn);


        //tn = 0.4 - (6.0 / (1.0 + xn*xn + yn*yn));
        doubleTn = mpf_get_d(tn);
        doubleCosT = cos(doubleTn);
        doubleSinT = sin(doubleTn);

        mpf_set_d(cosT, doubleCosT);
        mpf_set_d(sinT, doubleSinT);

        //xn = 1.0 + miu * ((xn * cosT) - (yn * sinT));
        mpf_mul(helper1, xn, cosT);
        mpf_mul(helper2, yn, sinT);
        mpf_sub(xn2, helper1, helper2);
        mpf_mul(xn2, miu, xn2);
        mpf_add_ui(xn2, xn2, 1);

        //yn = miu * ((xn * sinT) - (yn * cosT));
        mpf_mul(helper1, xn, sinT);
        mpf_mul(helper2, yn, cosT);
        mpf_sub(yn2, helper1, helper2);
        mpf_mul(yn2, miu, yn2);

        mpf_set(xn, xn2);
        mpf_set(yn, yn2);

        if(i >= entriesToSkip) {
            mpf_abs(absX, xn);
            mpf_abs(absY, yn);

            mpf_floor(helper1, absX);
            mpf_sub(helper1, absX, helper1);
            mpf_mul(helper1, helper1, multiply);
            mOneSequence[i-entriesToSkip] = mpf_get_ui(helper1) % 255;

            mpf_floor(helper2, absY);
            mpf_sub(helper2, absY, helper2);
            mpf_mul(helper2, helper2, multiply);
            mTwoSequence[i-entriesToSkip] = mpf_get_ui(helper2) % 255;
        }
    }

    mpf_clears(multiply, absX, absY, xn, yn, xn2, yn2, tn, cosT, sinT, helper1, helper2, tnBase, NULL);
}

void generateControlParametersIkedaMap(mpf_t miu, mpf_t avgOfImageByteSum, long numberOfImageBytes) {
	mpf_t addValue;
    mpf_init(addValue);

    mpf_set_d(addValue, 0.0);

    if(numberOfImageBytes > 1000000 && numberOfImageBytes <= 4000000)
        mpf_set_d(addValue, 0.1);
    else if(numberOfImageBytes > 4000000)
        mpf_set_d(addValue, 0.2);

    mpf_add(miu, miu, addValue);

    mpf_ui_sub(addValue, 1, avgOfImageByteSum);
    mpf_div_ui(addValue, addValue, 2);

    mpf_add(miu, miu, addValue);

    mpf_clear(addValue);
}

void generateControlParametersLogisticMap(mpf_t r, mpf_t avgOfImageByteSum, long numberOfImageBytes) {

    mpf_t addValue;
    mpf_init(addValue);

    mpf_set_d(addValue, 0.4);

    if(numberOfImageBytes > 1000000 && numberOfImageBytes <= 4000000)
        mpf_set_d(addValue, 0.5);
    else if(numberOfImageBytes > 4000000)
        mpf_set_d(addValue, 0.6);

    mpf_add(r, r, addValue);
    mpf_sub(r, r, avgOfImageByteSum);

    mpf_clear(addValue);
}

void createPermutationSequence(int permutationSequence[], mpf_t r, mpf_t x, long sequenceLength) {
    mpf_t *sequenceS = (mpf_t*)malloc(sizeof(mpf_t)*sequenceLength);

    mpf_t xn, xhelper;
    mpf_inits(xn, xhelper, NULL);
    mpf_set(xn, x);

    // create original chaotic sequence (skip 1st 1000 entries)
    int transientResultsToSkip = 1000;
    for(long i = 0; i < transientResultsToSkip + sequenceLength; i++) {
        mpf_set(xhelper, xn);

        mpf_ui_sub(xhelper, 1, xhelper);

        mpf_mul(xn, r, xn);
        mpf_mul(xn, xn, xhelper);

        if(i >= transientResultsToSkip) {
            mpf_init(sequenceS[i-transientResultsToSkip]);
            mpf_set(sequenceS[i-transientResultsToSkip], xn);
        }
    }

    /*
    PTF("original sequence C\n");
    printSequence(sequenceC, 10);
    */

    // create sorted sequence S based on sequence C
    //memcpy(sequenceS, sequenceC, sequenceLength * sizeof(double));

    quickSort(sequenceS, sequenceLength);

    /*
    PTF("sorted sequence S\n");
    printSequence(sequenceS, 10);
    */

    // better allocation (use malloc)
    int numberOfGroups = 10;
    int groupArrayLengths = (int)sequenceLength;
    double emptyValue = -1;
    mpf_t tmpValue, tmpValue2;
    mpf_inits(tmpValue, tmpValue2, NULL);

    //mpf_t groupedArrays[numberOfGroups][groupArrayLengths];

    mpf_t **groupedArrays = (mpf_t**)malloc(sizeof(mpf_t)*numberOfGroups*groupArrayLengths);
    int *lastGroupedArrayPosition = (int*)malloc(sizeof(int)*numberOfGroups);

    // initialize arrays
    int j;
    for(int i = 0; i < numberOfGroups; i++) {
        lastGroupedArrayPosition[i] = 0;
        groupedArrays[i] = (mpf_t*)malloc(sizeof(mpf_t)*groupArrayLengths);

        for(j = 0; j < sequenceLength; j++) {
            mpf_init(groupedArrays[i][j]);
            mpf_set_d(groupedArrays[i][j], emptyValue);
        }
    }

    // create grouped arrays based on sequence s
    int groupNumber, tmpResult1, tmpResult2;

    for(long i = 0; i < sequenceLength; i++) {

        mpf_mul_ui(tmpValue, sequenceS[i], 1000000);

        // get group number
        //tmpResult1 = (((int)floor(tmpTimes)) % 10);
        mpf_floor(tmpValue2, tmpValue);
        tmpResult1 = mpf_get_ui(tmpValue2) % 10;

        //tmpResult2 = (((int)floor(tmpTimes * 1000)) % 10);
        mpf_mul_ui(tmpValue2, tmpValue, 1000);
        mpf_floor(tmpValue2, tmpValue2);
        tmpResult2 = mpf_get_ui(tmpValue2) % 10;

        if(tmpResult1 >= tmpResult2)
            groupNumber = tmpResult1-tmpResult2;
        if(tmpResult2 >= tmpResult1)
            groupNumber = tmpResult2-tmpResult1;

        if(groupNumber < 0 || groupNumber > 9)
            return;

        // set value into appropriate group array
        mpf_set(groupedArrays[groupNumber][lastGroupedArrayPosition[groupNumber]++], sequenceS[i]);
    }

    long permutationIndex = 0;

    for(int i = 0; i < numberOfGroups; i++) {
        if(permutationIndex >= sequenceLength)
            break;

        j = 0;
        while(mpf_cmp_d(groupedArrays[i][j], emptyValue) != 0) {
            permutationSequence[permutationIndex++] = find(sequenceS, groupedArrays[i][j], sequenceLength);
            mpf_clear(groupedArrays[i][j]);
            j++;
        }
    }

    for(int k = 0; k < numberOfGroups; k++) {
        free(groupedArrays[k]);
    }
    free(groupedArrays);

    free(lastGroupedArrayPosition);

    for(long i = 0; i < sequenceLength; i ++) {
        mpf_clear(sequenceS[i]);
    }
    free(sequenceS);

    mpf_clears(tmpValue, tmpValue2, NULL);
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

// binary search
int find(mpf_t array[], mpf_t data, long length) {
   int lowerBound = 0;
   int upperBound = length -1;
   int midPoint = -1;
   int comparisons = 0;
   int index = -1;
   int cmpResult = -2;

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
      //if(array[midPoint] == data) {
      cmpResult = mpf_cmp(array[midPoint],data);
      if(cmpResult == 0) {
         index = midPoint;
         break;
      } else {
         // if data is larger
         if(cmpResult < 0) {
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
