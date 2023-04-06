#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

// LICZBA_TEST = 62945422280294
#define STOP_MARK 1
#define MASTER_PROCCESS_TAG 0

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

   int stopCalculating = 0;
   MPI_Request stopRequest;

   MPI_Irecv(&stopCalculating, 1, MPI_INT, MPI_ANY_SOURCE, STOP_MARK, MPI_COMM_WORLD, &stopRequest);

   for (long i = myrank + 2; i * i <= inputArgument; i += nproc) {
        if (inputArgument % i == 0) {
            isPrime = 1;
            stopCalculating = 1;
            for (int j = 0; j < nproc; j++) {
                if (j != myrank) {
                    MPI_Send(&stopCalculating, 1, MPI_INT, j, STOP_MARK, MPI_COMM_WORLD);
                }
            }
            break;
        }
        int stopRequestReceived = 0;
        MPI_Test(&stopRequest, &stopRequestReceived, MPI_STATUS_IGNORE);
        if (stopRequestReceived == 1) {
            break;
        }
    }

  // synchronize/finalize your computations
  MPI_Reduce(&isPrime, &sumOfPrimes, 1, MPI_INT, MPI_SUM, MASTER_PROCCESS_TAG, MPI_COMM_WORLD);

  if (!myrank) {
    printf("\n");
    if (sumOfPrimes == 0){
      printf("\n%ld JEST pierwsza", inputArgument);
    } else {
      printf("\n%ld NIE JEST pierwsza", inputArgument);
    }
    printf("\n");
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }

  MPI_Finalize();

}
