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

  // run your computations here (including MPI communication)
   int isPrime = 0, sumOfPrimes = 0;
   
   for (long i = myrank + 2; i * i <= inputArgument; i += nproc) {
        if (inputArgument % i == 0) {
            isPrime = 1;
            break;
        }
    }

  // synchronize/finalize your computations
  MPI_Reduce(&isPrime, &sumOfPrimes, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!myrank) {
    if (sumOfPrimes == 0){
      printf("\n %ld JEST pierwsza", inputArgument);
    } else {
      printf("\n %ld NIE JEST pierwsza", inputArgument);
    }
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }
  
  MPI_Finalize();

}
