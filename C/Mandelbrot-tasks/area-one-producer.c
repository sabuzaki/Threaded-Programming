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
 *  For Tasks excercise, one can not use "recuction" clause
 *
 */
// One producer. Only master thread create tasks.

  start = omp_get_wtime();  
 #pragma omp parallel
 {
    #pragma omp master
    { 
      for (int i=0; i<NPOINTS; i++) {
        #pragma omp task firstprivate(i) shared(numoutside) default(none) private(c,z)
        {
          for (int j=0; j<NPOINTS; j++) {
            c.real = -2.0+2.5*(double)(i)/(double)(NPOINTS)+1.0e-7;
            c.imag = 1.125*(double)(j)/(double)(NPOINTS)+1.0e-7;
            z=c;
            for (int iter=0; iter<MAXITER; iter++){
              double ztemp=(z.real*z.real)-(z.imag*z.imag)+c.real;
              z.imag=z.real*z.imag*2+c.imag; 
              z.real=ztemp; 
              if ((z.real*z.real+z.imag*z.imag)>4.0e0) {
                #pragma omp atomic
                  numoutside++; 
                break;
              }
            }
          }
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
