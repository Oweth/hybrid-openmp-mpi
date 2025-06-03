#include <stdio.h>

#define NX 102400

int main(void)
{
    long vecA[NX];
    long sum, psum, sumex;
    int i;

    /* Initialization of the vectors */
    for (i = 0; i < NX; i++) {
        vecA[i] = (long) i + 1;
    }

    sumex = (long) NX * (NX + 1) / ((long) 2);
    printf("Arithmetic sum formula (exact): %ld\n", sumex);

    sum = 0.0;
    /* TODO: Parallelize computation */
    int startTime = omp_get_wtime();
    #pragma omp parallel for reduction(+ : sum) default(shared) private(i)
    for (i = 0; i < NX; i++) {
        sum += vecA[i];
    }
    printf("Sum: %ld\n", sum);
    int endTime = omp_get_wtime();
    printf("Time taken: %d seconds\n", endTime - startTime);
    return 0;
}
