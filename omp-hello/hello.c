#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    #pragma omp parallel
    {
        int id = omp_get_num_threads();
        printf("Hello world from thread %d\n", id);
    }
    return 0;
}
