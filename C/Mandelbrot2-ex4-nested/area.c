#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

# define NPOINTS 2000
# define MAXITER 2000


struct complex{
  double real;
  double imag;
};

int main(){
  int numoutside = 0;
  double area, error;
  double start, finish;
  struct complex z, c;

/*
 *   
 *
 *     Outer loops run over npoints, initialise z=c
 *
 *     Inner loop has the iteration z=z*z+c, and threshold test
 */

#pragma omp parallel 
  {
	#pragma omp master 
	  {
		printf("Number of Threads: %d\n", omp_get_num_threads());
	}
}

  start = omp_get_wtime();  
///#pragma omp parallel for default(none) private(c,z) reduction(+:numoutside)
#pragma omp parallel for default(none) private(c,z) reduction(+:numoutside) collapse(2) schedule(dynamic,10)
  for (int i=0; i<NPOINTS; i++) {				// outer loop
//    #pragma omp parallel for default(none) private(c,z) reduction(+:numoutside) shared(i) 		// workd but no performance
    for (int j=0; j<NPOINTS; j++) {				// inner loop 1
      c.real = -2.0+2.5*(double)(i)/(double)(NPOINTS)+1.0e-7;	// L1
      c.imag = 1.125*(double)(j)/(double)(NPOINTS)+1.0e-7;	// L2
      z=c;							// L3
      for (int iter=0; iter<MAXITER; iter++){			// inner loop 2
	double ztemp=(z.real*z.real)-(z.imag*z.imag)+c.real;	// L4
	z.imag=z.real*z.imag*2+c.imag; 				// L5
	z.real=ztemp; 						// L6
	if ((z.real*z.real+z.imag*z.imag)>4.0e0) {		// L7
	  numoutside++; 					// L8
	  break;
	}
      }
    }
  }


  finish = omp_get_wtime();  

/*
 *  Calculate area and error and output the results
 */

      area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
      error=area/(double)NPOINTS;

      printf("Area of Mandlebrot set = %12.8f +/- %12.8f\n",area,error);
      printf("Time = %12.8f seconds\n",finish-start);

  }
