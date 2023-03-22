#include <stdio.h>
#include <math.h>
#include "utility.h"
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DATA 0
#define RESULT 1
#define FINISH 2

int main(int argc, char **argv) {
    Args ins__args;
    parseArgs(&ins__args, &argc, argv);

    // program input argument
    int quantity_of_numbers = ins__args.arg;
    int myrank, proccount;
    int counter = 0;

    MPI_Status status;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // find out my rank
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // find out the number of processes in MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &proccount);

    if (proccount < 2) {
        printf("Run with at least 2 processes");
        MPI_Finalize();
        return -1;
    }

    srand(time(NULL));
    int range_min = 0, range_max = 5;
    int size_of_histogram = range_max + 1;

    int number_of_job_parts = quantity_of_numbers / (proccount - 1);

    // now the master will distribute the data and slave processes will perform computations
    if (myrank == 0) {
        // master process

        // create an array to store the occurrences of each number
        int *histogram = (int *) calloc(range_max - range_min + 1, sizeof(int));

        // first distribute some sets of random numbers to all slaves
        for (int i = 1; i < proccount; i++) {
            int *set = (int *) malloc(
                    number_of_job_parts * sizeof(int)); // alokacja pamięci dla każdej paczki wysłanej dla slava
            // fill the set with random numbers
            for (int j = 0; j < number_of_job_parts; j++) {
                set[j] = rand() % (range_max - range_min + 1) + range_min;
                counter++;
            }

            MPI_Send(set, number_of_job_parts, MPI_INT, i, DATA, MPI_COMM_WORLD);
            free(set);
        }
        if (counter < quantity_of_numbers){
            int *received_set = (int *) malloc(size_of_histogram * sizeof(int));
            MPI_Recv(received_set, size_of_histogram, MPI_INT, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status);

            // update the histogram
            for (int j = 0; j < size_of_histogram; j++) {
                histogram[j - range_min] += received_set[j];
            }
            free(received_set);

           int *set = (int *) malloc(
                    number_of_job_parts * sizeof(int));

            // fill the set with random numbers
            for (int j = 0; j < number_of_job_parts; j++) {
                if (counter < quantity_of_numbers) {
                    set[j] = rand() % (range_max - range_min + 1) + range_min;
                    counter++;
                }
                else{
                    set[j] = range_max + 1;
                }
            }

            MPI_Send(set, number_of_job_parts, MPI_INT, status.MPI_SOURCE, DATA, MPI_COMM_WORLD);
            free(set);
        }

        // receive the results from the slave processes and update the histogram
        for (int i = 1; i < proccount; i++) {
            int *received_set = (int *) malloc(size_of_histogram * sizeof(int));
            MPI_Recv(received_set, size_of_histogram, MPI_INT, i, RESULT, MPI_COMM_WORLD, &status);

            // update the histogram
            for (int j = 0; j < size_of_histogram; j++) {
                histogram[j - range_min] += received_set[j];
            }
            free(received_set);
        }
        // shut down the slaves
        for (int i = 1; i < proccount; i++) {
            MPI_Send(NULL, 0, MPI_INT, i, FINISH, MPI_COMM_WORLD);
        }

        // print the histogram
        printf("\nHistogram:\n");
        for (int i = 0; i <= range_max - range_min; i++) {
            printf("%d: %d\n", i + range_min, histogram[i]);
        }
        printf("\nWALIDACJA OUTPUTU\n");
        int sum = 0;
        for (int i = 0; i <= range_max - range_min; i++) {
            sum = sum + histogram[i];
        }
        printf("SUMA WSZYSTKICH WARTOŚCI: %d \n", sum);
        printf("ILOŚĆ WYGENEROWANYCH LICZB: %d \n", counter);

        // free the memory
        free(histogram);
    } else { // slave
        // this is easy - just receive data and do the work do
        while (status.MPI_TAG == DATA) {

            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == DATA) {

                int *slave_result = (int *) malloc(size_of_histogram * sizeof(int));

                int *package_from_master = (int *) malloc(number_of_job_parts * sizeof(int));

                for (int j = 0; j < size_of_histogram; j++) {
                    slave_result[j] = 0;
                }

                MPI_Recv(package_from_master, number_of_job_parts, MPI_INT, 0, DATA, MPI_COMM_WORLD,
                         &status);

                // compute my part
                for (int j = 0; j < number_of_job_parts; j++) {
                    if (package_from_master[j] < range_max + 1) {
                        slave_result[package_from_master[j]]++;
                    }
                }

                // send the result back
                MPI_Send(slave_result, size_of_histogram, MPI_INT, 0, RESULT,
                         MPI_COMM_WORLD);

                free(slave_result);
                free(package_from_master);
            }
        }
    }

    // Shut down MPI
    MPI_Finalize();

    return 0;
}
