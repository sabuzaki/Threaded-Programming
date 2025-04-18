#include <stdio.h>
#include <math.h>


#define N 2187
#define reps 1000 
#include <omp.h> 

double xx[N][N], yy[N][N], zz;
int len[N];  


void init1(void);
void init2(void);
void loop1(void);
void loop2(void);
void valid1(void);
void valid2(void);


int main(int argc, char *argv[]) { 

  double start1,start2,end1,end2;

  init1(); 
  #pragma omp parallel
    {
        #pragma omp single
        {
	    int num_threads=0;
	    int chunk_size =0;
            omp_sched_t kind;
            num_threads = omp_get_num_threads();
	    printf("########################################################\n");
            printf("Number of threads: %d.  ", num_threads);
	    omp_get_schedule(&kind, &chunk_size);
	    // Print the schedule type
    printf("Schedule type: ");
    switch (kind) {
        case omp_sched_static:
            printf("static");
            break;
        case omp_sched_dynamic:
            printf("dynamic");
            break;
        case omp_sched_guided:
            printf("guided");
            break;
        case omp_sched_auto:
            printf("auto");
            break;
        default:
            printf("unknown");
            break;
    }

    printf(", Chunk size: %d\n", chunk_size);
    }
    }

int thread_id = 0;

  start1 = omp_get_wtime(); 


  for (int r=0; r<reps; r++){ 
    loop1();
  } 

  end1  = omp_get_wtime();  

  valid1(); 

  printf("Total time for %d reps of loop 1 = %f\n",reps, (float)(end1-start1)); 


  init2(); 

  start2 = omp_get_wtime(); 

  //for (int r=0; r<reps; r++){ 
    loop2();
  //} 

  end2  = omp_get_wtime(); 

  valid2(); 

  printf("Total time for %d reps of loop 2 = %f\n",reps, (float)(end2-start2)); 
} 

void init1(void){

  for (int i=0; i<N; i++){ 
    for (int j=0; j<N; j++){ 
      xx[i][j] = 0.0; 
      yy[i][j] = 1.618*(i+j); 
    }
  }

}

void init2(void){ 
  int i,j, expr; 

  for (int i=0; i<N; i++){ 
    expr =  i%( 4*(i/70) + 1); 
    if ( expr == 0) { 
      len[i] = N/3;
    }
    else {
      len[i] = 3; 
    }
  }

  for (int i=0; i<N; i++){ 
    for (int j=0; j<N; j++){ 
      yy[i][j] = (double) (i*j+1) / (double) (N*N); 
    }
  }

  zz = 0.0;
 
} 

void loop1(void) { 

#pragma omp parallel for default(none) schedule(runtime) shared(xx,yy)
  for (int i=0; i<N; i++){ 
    for (int j=0; j<i; j++){
      xx[i][j] += log(yy[i][j]) / (double) N;
    } 
  }


} 



void loop2(void) {

  int tid;
  double start_time[omp_get_max_threads()], end_time[omp_get_max_threads()];
  double rNN = 1.0 / (double) (N*N);  
  double t[32];
  #pragma omp parallel private(tid) shared(rNN,yy,len)
    {
        tid = omp_get_thread_num();
        start_time[tid] = omp_get_wtime();

//#pragma omp parallel for default(none) shared(rNN,yy,len) reduction(+:zz) schedule(runtime)			//working one
#pragma omp for schedule(runtime) reduction(+:zz) 
  for (int i=0; i<N; i++){ 
    for (int j=0; j < len[i]; j++){
      for (int k=0; k<j; k++){ 
	for(int b=0;b<100000;b++)
	  zz += (k+1) * yy[i][j]* yy[i][j] * rNN;
      } 
    }
//    printf("tid %d\n",tid);
      end_time[tid] = omp_get_wtime();
  }
  //end_time[tid] = omp_get_wtime();
  }

  for (int t = 0; t < omp_get_max_threads(); t++) {
        printf("Thread %d execution time: %f seconds\n", t, end_time[t] - start_time[t]);
  }
}

void valid1(void) { 
  
  double sumxx= 0.0; 
  for (int i=0; i<N; i++){ 
    for (int j=0; j<N; j++){ 
      sumxx += xx[i][j];
    }
  }
  printf("Loop 1 check: Sum of xx is %lf\n", sumxx);

} 


void valid2(void) { 
  
  printf("Loop 2 check: zz is %lf\n", zz);

} 
 

