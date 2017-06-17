#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "imagecipher1.h"

// helper functions
void printSequence(double a[], long n);
void sort(double a[], long n);
int find(double array[], double data, long length);

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

int quickSort(double *arr, int elements) {

  #define  MAX_LEVELS  1000

  double piv;
  int  beg[MAX_LEVELS], end[MAX_LEVELS], L, R, i = 0 ;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; if (i==MAX_LEVELS-1) return -1;
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
    else {
      i--; }}
  return 0;
}

clock_t clockStart(char *message) {
    PTF_IMPT("\nstarting %s \n", message);
    return clock();
}

void clockStopAndWrite(char *message, clock_t begin) {
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    PTF_IMPT("\n %s %0.15f seconds\n", message, time_spent);
}

// actual cipher functions
// create permutation sequence based on logistic map
void createPermutationSequence(int permutationSequence[], double r, double x, long sequenceLength);
// generate permutation parameters based on image values
double generateControlParametersLogisticMap(double basicR, double avgOfImageByteSum, long numberOfImageBytes);

// create diffusion sequence based on Ikeda map, generates two sequences at a time mOneSequence and mTwoSequence
void createDiffusionSequenceIkedaMap(double miu, double x, double y, unsigned char mOneSequence[], unsigned char mTwoSequence[], long sequenceLength);

// generate diffustion parameters based on image values
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

    #ifdef DEV
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

    /*int permutationSequenceLogisticMap[4][numberOfImageBytes];
    unsigned char diffustionSequenceIkedaMap[4][numberOfImageBytes];*/

    clock_t s = clockStart("start permutation");

    int **permutationSequenceLogisticMap = (int**)malloc(sizeof(int*)*4);
    unsigned char **diffustionSequenceIkedaMap = (unsigned char**)malloc(sizeof(unsigned char*)*4);

    for(int i = 0; i < 4; i++){
        permutationSequenceLogisticMap[i] = (int*)malloc(sizeof(int)*numberOfImageBytes);
        diffustionSequenceIkedaMap[i] = (unsigned char *)malloc(sizeof(unsigned char)*numberOfImageBytes);
    }

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

    clock_t s2 = clockStart("   start create permSeq");

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

    clockStopAndWrite("   done permSeq", s2);

    clockStopAndWrite("done permutation", s);

    s = clockStart("start diffusion");

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

    clockStopAndWrite("done diffusion", s);

    s = clockStart("encryption/decryption");

    if(mode == ENC_MODE) {
        // 5. Encryption rounds
        unsigned char *tmpImageBytes = (unsigned char*)malloc(sizeof(unsigned char)*numberOfImageBytes);
        //memcpy(tmpImageBytes, imageBytes, numberOfImageBytes * sizeof(unsigned char));

        long k, j;
        for(int i = 0; i < encryptionRounds; i++) {

            for(k = 0; k < 4; k++) {
                PTF("\n----------- round %d after permutation %ld [", i, k);
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
                PTF("] \n");*/

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

                PTF("\n----------- round %d after diffustion %ld [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    tmpImageBytes[j/*permutationSequenceLogisticMap[k][j]*/] = imageBytes[j]^diffustionSequenceIkedaMap[k][j];
                    PTF("%u, ", tmpImageBytes[j]);
                }
                PTF("] \n");

                PTF("\n----------- round %d after permutatio %d [", i, k);
                for(j = 0; j < numberOfImageBytes; j++) {
                    imageBytes[permutationSequenceLogisticMap[k][j]] = tmpImageBytes[j];
                }
                /*for(j = 0; j < numberOfImageBytes; j++) {
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

    for(int i = 0; i < 4; i++){
        free(permutationSequenceLogisticMap[i]);
        free(diffustionSequenceIkedaMap[i]);
    }

    free(permutationSequenceLogisticMap);
    free(diffustionSequenceIkedaMap);

    #ifdef DEV
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
    double absX, absY, tn, cosT, sinT;
    double xn = x, xn2;
    double yn = y, yn2;

    PTF("--------- Creating Diffusion Sequence ---------\n")

    // calculate chaotic map sequences
    for(long i = 0; i < entriesToSkip + sequenceLength; i++) {
        tn = 0.4 - (6.0 / (1.0 + xn*xn + yn*yn));

        cosT = cos(tn);
        sinT = sin(tn);

        xn2 = 1.0 + miu * ((xn * cosT) - (yn * sinT));
        yn2 = miu * ((xn * sinT) - (yn * cosT));

        xn = xn2;
        yn = yn2;

        if(i >= entriesToSkip) {
            absX = fabs(xn);
            absY = fabs(yn);
            mOneSequence[i-entriesToSkip] = ((long long)((absX - ((double)floor(absX))) * multiply)) % 255;
            mTwoSequence[i-entriesToSkip] = ((long long)((absY - ((double)floor(absY))) * multiply)) % 255;
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
    double *sequenceS = (double*)malloc(sizeof(double)*sequenceLength);
    double xn = x;

    clock_t z = clockStart("--- calc sequence s");

    // create original chaotic sequence (skip 1st 1000 entries)
    int transientResultsToSkip = 1000;
    for(long i = 0; i < transientResultsToSkip + sequenceLength; i++) {
        xn = r * xn * (1 - xn);
        if(i >= transientResultsToSkip)
            sequenceS[i-transientResultsToSkip] = xn;
    }

    clockStopAndWrite("--- done calc sequence s", z);
    /*
    PTF("original sequence C\n");
    printSequence(sequenceC, 10);
    */

    // create sorted sequence S based on sequence C
    //memcpy(sequenceS, sequenceC, sequenceLength * sizeof(double));

    z = clockStart("--- sort sequence s");

    quickSort(&sequenceS[0], sequenceLength);

    clockStopAndWrite("--- done sort sequence s", z);
    /*
    PTF("sorted sequence S\n");
    printSequence(sequenceS, 10);
    */

    z = clockStart("--- group sequence s");

    // better allocation (use malloc)
    int numberOfGroups = 10;
    double **groupedArrays = (double**)malloc(sizeof(double*)*numberOfGroups);
    int groupArrayLengths = (int)sequenceLength;
    int *lastGroupedArrayPosition = (int*)malloc(sizeof(int)*numberOfGroups);

    // initialize arrays
    int j;
    for(int i = 0; i < numberOfGroups; i++) {
        lastGroupedArrayPosition[i] = 0;
        groupedArrays[i] = (double*)malloc(sizeof(double)*groupArrayLengths);

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
        else if(tmpResult2 >= tmpResult1)
            groupNumber = tmpResult2-tmpResult1;

        if(groupNumber < 0 || groupNumber > 9)
            return;

        // set value into appropriate group array
        groupedArrays[groupNumber][lastGroupedArrayPosition[groupNumber]++] = sequenceS[i];
    }

    long permutationIndex = 0;

    clock_t groupS = clockStart("--- start find");

    for(int i = 0; i < numberOfGroups; i++) {
        if(permutationIndex >= sequenceLength)
            break;

        j = 0;
        while(groupedArrays[i][j] > 0) {
            permutationSequence[permutationIndex++] = find(sequenceS, groupedArrays[i][j], sequenceLength);
            j++;
        }
    }

    clockStopAndWrite("--- done find", groupS);

    clockStopAndWrite("--- done group sequence s", z);

    for(int i = 0; i < numberOfGroups; i++){
        free(groupedArrays[i]);
    }
    free(groupedArrays);
    free(lastGroupedArrayPosition);
    free(sequenceS);
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
