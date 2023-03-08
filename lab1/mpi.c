#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

int is_prime(long n) {
    if (n <= 1) {
        return 0;
    }
    for (long i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

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


  // check if inputArgument is prime
  int is_input_prime = is_prime(inputArgument);

  // synchronize/finalize your computations
  int all_input_prime = 0;
  MPI_Allreduce(&is_input_prime, &all_input_prime, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);

  if (!myrank && all_input_prime) {
      printf("%ld is prime.\n", inputArgument);
  }

  if (!myrank) {
    gettimeofday(&ins__tstop, NULL);
    ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }
  
  MPI_Finalize();

}
