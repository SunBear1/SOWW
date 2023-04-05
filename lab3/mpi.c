#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

int main(int argc,char **argv) {

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  //program input argument
  long inputArgument = ins__args.arg;

  struct timeval ins__tstart, ins__tstop;

  int myrank,nproc;

  MPI_Init(&argc,&argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  if(!myrank)
      gettimeofday(&ins__tstart, NULL);

   int isPrime = 0, sumOfPrimes = 0;

   int stopCalculation = 0;
   MPI_Request request;

   MPI_Irecv(&stopCalculation, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);

   for (long i = myrank + 2; i * i <= inputArgument; i += nproc) {
        if (inputArgument % i == 0) {
            isPrime = 1;
            stopCalculation = 1;
            for (int j = 0; j < nproc; j++) {
                if (j != myrank) {
                    MPI_Send(&stopCalculation, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
                }
            }
            break;
        }
        int stopRequestReceived = 0;
        MPI_Test(&request, &stopRequestReceived, MPI_STATUS_IGNORE);
        if (stopRequestReceived == 1) {
            break;
        }
    }

  // synchronize/finalize your computations
  MPI_Reduce(&isPrime, &sumOfPrimes, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!myrank) {
    if (sumOfPrimes == 0){
      printf("%ld JEST pierwsza", inputArgument);
    } else {
      printf("%ld NIE JEST pierwsza", inputArgument);
    }
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }

  MPI_Finalize();

}
