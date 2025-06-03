#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NX 102400

int main(void)
{
    double *vecA = malloc(NX*sizeof(double));
    double *vecB = malloc(NX*sizeof(double)); 
    double *vecC = malloc(NX*sizeof(double));
    double sum;
    int i;

    /* Initialization of the vectors */
    double start_timeS = omp_get_wtime();
    for (i = 0; i < NX; i++) {
        vecA[i] = 1.0 / ((double)(NX - i));
        vecB[i] = vecA[i] * vecA[i];
    }
    double end_timeS = omp_get_wtime();
    double elapsed_timeS = end_timeS - start_timeS;
    printf("Elapsed time for vector initialization: %f seconds\n", elapsed_timeS);

    /* TODO:
     *   Implement here a parallelized version of vector addition,
     *   vecC = vecA + vecB
    */
    double start_time = omp_get_wtime();
    #pragma omp parallel for default(shared) private(i)
    for (int i = 0; i < NX; i++)
    {
        /* code */
        vecC[i] = vecA[i] + vecB[i];
    }
    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;
    printf("Elapsed time for vector addition: %f seconds\n", elapsed_time);
    sum = 0.0;
    /* Compute the check value */
    for (i = 0; i < NX; i++) {
        sum += vecC[i];
    }
    printf("Reduction sum: %18.16f\n", sum);

    free(vecA);
    free(vecB);
    free(vecC);

    return 0;
}
