#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(){
omp_set_num_threads(2);
#pragma omp parallel
{
#pragma omp parallel for
    for (int j=0; j<10; j++){
        printf("thread %d calculating %d\n",omp_get_thread_num(), j);
    }
/*
#pragma omp critical 
     {
      printf("hello from thread %d\n",omp_get_thread_num());
     } 
}*/
}
}
