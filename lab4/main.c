#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

int threadnum=4;

int main(int argc,char **argv) {

    //set number of threads
    omp_set_num_threads(threadnum);

    unsigned long long prime_number = 0;
    int is_prime = 0;

    scanf("%llu", &prime_number);

    clock_t start, end;
    start = clock();

    double cpu_time_used;

    #pragma omp parallel shared(is_prime)
    for (unsigned long long i=2; i*i<prime_number; i++){
        if (prime_number % i == 0){
            is_prime = 1;
        }
        if (is_prime == 1)
            break;
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    if (is_prime)
        printf("Liczba %llu jest pierwsza\n", prime_number);
    else
        printf("Liczba %llu nie jest pierwsza\n", prime_number);

    printf("CPU time used: %f seconds\n", cpu_time_used);
}