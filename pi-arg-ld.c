/* 
 * File: pi-arg.c by Gary Williams, 3/4/17
 *             based on mpi-hello.c by Mary Thomas, 2/12/15
 *             based on mpi-hello.c by James Otto, 1/31/11 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mpi.h"

long double pi_int(long double x);

int main (int argc, char* argv[])
{
  long double i, myarg;
  struct timeval startTime, stopTime;
  int rank, nprocs, ierr, error=0;
  long double subStep, approx, x, recval, a, b;
  const long double pi = 3.141592653589793238462643383279502884197;
  MPI_Status status;

  ierr = MPI_Init(&argc, &argv);
  if (ierr != MPI_SUCCESS) {
    printf("MPI initialization error\n");
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if(rank==0) gettimeofday(&startTime, NULL);

  /* get a number from the command line */
  if (argc != 2) {
      printf("usage: pi-arg <integer number>\n");
      MPI_Finalize();
      return 0;
  }
  /* run system commands */
  //system("env");

  myarg = strtol(argv[1], NULL, 10);
  /*printf("Hello From Processor: rank of  %d nprocs: %d;  CMDARG= %ld\n", 
rank, nprocs,myarg);*/
    
  //Time to start setting up local calc
  a = (long double)(1.0/nprocs);
  a = (long double) rank * a;
  b = (long double)(1.0/nprocs);
  b = (long double) (rank + 1.0) * b; 
  subStep = (b - a) / myarg;
  //subStep = 0.01;
  /*printf("a= %f, b= %f, myarg= %d and subStep= %f\n", a, b, myarg, 
      subStep);*/
 
  approx = (pi_int(a) + pi_int(b)) / 2.0;
  //printf("approx=%f before loop\n", approx);

  for(i = 1; i<myarg; i++){
      x = a + i*subStep;
      approx += pi_int(x);
  }
  //printf("approx=%f\n", approx);
  approx = subStep*approx;
  //printf("approx=%f\n", approx);

  //Send each local approx to 0
  if(rank!=0){
      MPI_Send(&approx, 1, MPI_LONG_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }
  else{
      for(i=1; i<nprocs; i++){
          MPI_Recv(&recval, 1, MPI_LONG_DOUBLE, i, 1, MPI_COMM_WORLD, 
              &status);
          approx += recval;
      }
      gettimeofday(&stopTime, NULL);
      printf("I am rank %d, and my final approx value is: %.20Lf.\n", rank, 
          approx);
      printf("The error from actual is: %.20Lf.\n", pi-approx);
      printf("In scientific: %.8Le.\n", pi-approx);
      printf("Time elapsed in microseconds(longformat): %ld microseconds\n",
         (stopTime.tv_sec - startTime.tv_sec)*1000000L + 
         stopTime.tv_usec - startTime.tv_usec);
  }

  MPI_Finalize();
  return 0;
}

long double pi_int(long double x){
  return 4.0/(1+x*x);
}
