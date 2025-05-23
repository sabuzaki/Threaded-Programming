#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h> 

#define mm 15
#define npart 4*mm*mm*mm
/*
 *  Function declarations
 */

  void
  dfill(int,double,double[],int);

  void
  domove(int,double[],double[],double[],double);

  void
  dscal(int,double,double[],int);

  void
  fcc(double[],int,int,double);

  void
  forces(int,double[],double[],double,double);

  double
  mkekin(int,double[],double[],double,double);

  void
  mxwell(double[],int,double,double);

  void
  prnout(int,double,double,double,double,double,double,int,double);

  double
  velavg(int,double[],double,double);

  double 
  secnds(void);

/*
 *  Variable declarations
 */

  double epot;
  double vir;
  double count;

#pragma omp threadprivate(epot,vir)
/*
 *  Main program : Molecular Dynamics simulation.
 */
int main(){
    int move;
    double x[npart*3], vh[npart*3], f[npart*3];
    double ekin;
    double vel;
    double sc;
    double start, time;


  /*
   *  Parameter definitions
   */

    double den    = 0.83134;
    double side   = pow((double)npart/den,0.3333333);
    double tref   = 0.722;
    double rcoff  = (double)mm/4.0;
    double h      = 0.064;
    int    irep   = 10;
    int    istop  = 20;
    int    iprint = 5;
    int    movemx = 20;

    double a      = side/(double)mm;
    double hsq    = h*h;
    double hsq2   = hsq*0.5;
    double tscale = 16.0/((double)npart-1.0);
    double vaver  = 1.13*sqrt(tref/24.0);

  /*
   *  Initial output
   */

    printf(" Molecular Dynamics Simulation example program\n");
    printf(" ---------------------------------------------\n");
    printf(" number of particles is ............ %6d\n",npart);
    printf(" side length of the box is ......... %13.6f\n",side);
    printf(" cut off is ........................ %13.6f\n",rcoff);
    printf(" reduced temperature is ............ %13.6f\n",tref);
    printf(" basic timestep is ................. %13.6f\n",h);
    printf(" temperature scale interval ........ %6d\n",irep);
    printf(" stop scaling at move .............. %6d\n",istop);
    printf(" print interval .................... %6d\n",iprint);
    printf(" total no. of steps ................ %6d\n",movemx);

  /*
   *  Generate fcc lattice for atoms inside box
   */
    fcc(x, npart, mm, a);
  /*
   *  Initialise velocities and forces (which are zero in fcc positions)
   */
    mxwell(vh, 3*npart, h, tref);
    dfill(3*npart, 0.0, f, 1);
  /*
   *  Start of md
   */
    printf("\n    i       ke         pe            e         temp   "
           "   pres      vel      rp\n  -----  ----------  ----------"
           "  ----------  --------  --------  --------  ----\n");

     start = secnds(); 
//     #pragma omp for reduction(+:epot) reduction(-:vir) shared(rcoffs,sideh,x,f) num_threads(8) schedule(runtime)
#pragma omp parallel private(move)
{
    #pragma omp single
    printf("threads: %d\n",omp_get_num_threads());
    for (move=1; move<=movemx; move++) {
    #pragma omp master 
    {
      domove(3*npart, x, vh, f, side);
    }
      #pragma omp barrier
      forces(npart, x, f, side, rcoff);
      #pragma omp barrier
      
      #pragma omp master
      {
       ekin=mkekin(npart, f, vh, hsq2, hsq);

      vel=velavg(npart, vh, vaver, h);
      if (move<istop && fmod(move, irep)==0) {
        sc=sqrt(tref/(tscale*ekin));
        dscal(3*npart, sc, vh, 1);
        ekin=tref/tscale;
      }
      if (fmod(move, iprint)==0)
        prnout(move, ekin, epot, tscale, vir, vel, count, npart, den);
      }
    }//for
}//parallel
    time = secnds() - start;  

    printf("Time =  %f\n",(float) time);  
  }

time_t starttime = 0; 

double secnds()
{

  return omp_get_wtime(); 

}

