#include <omp.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    #pragma omp parallel
    {
        int id = omp_get_num_threads();
        printf("Hello from thread %d\n", id);
    }
}

