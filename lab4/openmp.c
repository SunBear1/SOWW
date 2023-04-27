#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>

int main(int argc,char **argv) {


  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  //set number of threads
  omp_set_num_threads(ins__args.n_thr);
  
  //program input argument
  long inputArgument = ins__args.arg; 

  struct timeval ins__tstart, ins__tstop;
  gettimeofday(&ins__tstart, NULL);

  int precision = inputArgument;
  double sum=0;
  int i;

  #pragma omp parallel for reduction(+:sum)
  for(i = 0; i < precision; i++) {
    sum += 4 * pow(-1,i) * 1/ (2*i+1);
  }

  printf("PI = %f", sum);

  // synchronize/finalize your computations
  gettimeofday(&ins__tstop, NULL);
  ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);

}
