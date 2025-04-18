/* One queue, multiple threads, locking mechanism of single queue. Bug: one or two threads exit early at the start of the loop */
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include <unistd.h> 
#include "function.h" 

#define MAXQUEUE 10000
#define NUM_THREADS 32

int lock_holder=-1;

struct Interval {
    double left; // left boundary 
    double right; // right boundary
    double tol; // tolerance
    double f_left; // function value at left boundary
    double f_mid; // function value at midpoint 
    double f_right; // function value at right boundary
}; 

struct Queue { 
    struct Interval entry[MAXQUEUE];  // array of queue entries
    int top; // index of last entry 
    omp_lock_t lock;
}; 

// add an interval to the queue 
void enqueue (struct Interval interval, struct Queue *queue_p){
    int thread_n = omp_get_thread_num();
    if (queue_p->top == MAXQUEUE - 1) {
	  printf("Maximum queue size exceeded - exiting\n"); 
	  exit(1);
    } 
     queue_p->top++;
     queue_p->entry[queue_p->top].left = interval.left;
     queue_p->entry[queue_p->top].right = interval.right;
     queue_p->entry[queue_p->top].tol = interval.tol;
     queue_p->entry[queue_p->top].f_left = interval.f_left;
     queue_p->entry[queue_p->top].f_mid = interval.f_mid;
     queue_p->entry[queue_p->top].f_right = interval.f_right;
}

// extract last interval from queue 
struct Interval dequeue (struct Queue *queue_p) {
    struct Interval interval; //original
    int thread_n = omp_get_thread_num();
   // printf("T_%d setting lock (dequeue)\n",thread_n);
    //sleep(1);
//    printf("T_%d trying to acquire lock to dequeue\n",thread_n);
    /*while(queue_p->top!=-1) {
      if(omp_test_lock(&queue_p->lock)) {
        lock_holder=thread_n;
        break;
      }
    }*/

    if (queue_p->top == -1) {
      interval.tol=-999;
      return interval;
    }

//     omp_set_lock(&queue_p->lock);
     interval.left = queue_p->entry[queue_p->top].left;
     interval.right = queue_p->entry[queue_p->top].right;
     interval.tol = queue_p->entry[queue_p->top].tol;
     interval.f_left = queue_p->entry[queue_p->top].f_left;
     interval.f_mid = queue_p->entry[queue_p->top].f_mid;
     interval.f_right = queue_p->entry[queue_p->top].f_right;
     queue_p->top--;

 //    omp_unset_lock(&queue_p->lock);
     lock_holder=-1;
     return interval;
}


// initialise queue
void init (struct Queue * queue_p) {
    queue_p->top = -1;
    omp_init_lock(&queue_p->lock);
}

// return whether queue is empty 
int isempty (struct Queue * queue_p) {
    return (queue_p->top == -1);
}

// get current number of queue entries 
int size (struct Queue * queue_p) {
    return (queue_p->top + 1);
}


double simpson(double (*func)(double), struct Queue * queue_p)
{
   double quad = 0.0; 
   short active_t = 0;
   printf("Utilizing %d threads\n",NUM_THREADS);

#pragma omp parallel default(none) shared(active_t,quad,func, queue_p,lock_holder) num_threads(NUM_THREADS)
{
   int thread_n = omp_get_thread_num();
   struct Interval interval;
   #pragma omp barrier
   while (1) {
    if(!isempty(queue_p)) {
      omp_set_lock(&queue_p->lock);
      interval = dequeue(queue_p); 
      if(interval.tol==-999) {
        omp_unset_lock(&queue_p->lock);
        continue;
      }
      omp_unset_lock(&queue_p->lock);
      #pragma omp atomic
        active_t++;
      double h = interval.right - interval.left;   
      double c = (interval.left + interval.right)/2.0; 
      double d = (interval.left + c)/2.0; 
      double e = (c + interval.right)/2.0; 
      double fd = func(d); 
      double fe = func(e); 

   // Calculate integral estimates using 3 and 5 points respectively 
      double q1 = h/6.0 * (interval.f_left + 4.0 * interval.f_mid + interval.f_right); 
      double q2 = h/12.0 * (interval.f_left + 4.0*fd + 2.0*interval.f_mid + 4.0*fe + interval.f_right);
      
      if ( (fabs(q2-q1) < interval.tol) || ((interval.right-interval.left) < 1.0e-12) ){
	 // Tolerance is met, add to total 
     #pragma omp atomic
	   quad +=  q2 + (q2 - q1)/15.0; 
     #pragma omp atomic
        active_t--;
      }
      else {
	 // Tolerance is not met, split interval in two and add both halves to queue
	 struct Interval i1,i2; 

	 i1.left = interval.left; 
	 i1.right = c; 
	 i1.tol = interval.tol; 
	 i1.f_left = interval.f_left; 
	 i1.f_mid = fd; 
	 i1.f_right = interval.f_mid; 
         
	 i2.left = c; 
	 i2.right = interval.right; 
	 i2.tol = interval.tol; 
	 i2.f_left = interval.f_mid; 
	 i2.f_mid = fe; 
	 i2.f_right = interval.f_right; 

     omp_set_lock(&queue_p->lock);
     lock_holder=thread_n;
	 enqueue(i1,queue_p);
	 enqueue(i2,queue_p);
     omp_unset_lock(&queue_p->lock);
     lock_holder=-1;
     #pragma omp atomic
       active_t--;
    } 
   } else if(active_t > 0){
        continue;       // go back to while loop and check if queue is still empty as there are threads active
   } else {
     break;             //exit while loop (done with work...)
   }
} 
   printf("T_%d terminating...active_t: %d\n",thread_n,active_t);
}
   printf("\n");
   return quad; 
}

int main(void) 
{

    struct Queue queue; 
    struct Interval whole; 

// Initialise queue
    init(&queue);

    short active_t = 0;

    printf("Parallel version: one queue & work-sharing\n");
    double start = omp_get_wtime(); 

// Add initial interval to the queue 
    whole.left = 0.0; 
    whole.right = 10.0; 
    whole.tol = 1e-06; 
    whole.f_left = func1(whole.left); 
    whole.f_right = func1(whole.right); 
    whole.f_mid = func1((whole.left+whole.right)/2.0); 

    enqueue(whole, &queue); 
    printf("Initial enqueue: top: %d\n",queue.top);

// Call queue-based quadrature routine
    double quad = simpson(func1, &queue); 
    double time = omp_get_wtime() - start;
    printf("Result = %e\n", quad); 
    printf("Time(s) = %f\n", time); 
} 
