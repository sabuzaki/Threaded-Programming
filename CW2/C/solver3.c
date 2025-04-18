/* Parallel version. multiple threads having a separate queue. Work-stealing mechanism. */
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include <unistd.h>
#include "function.h" 

#define MAXQUEUE 10000
#define NUM_THREADS 32

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
    if (queue_p->top == MAXQUEUE - 1) {
	    printf("Maximum queue size exceeded - exiting\n"); 
	    exit(1);
    } 
    omp_set_lock(&queue_p->lock); 
    queue_p->top++;
    queue_p->entry[queue_p->top].left = interval.left;
    queue_p->entry[queue_p->top].right = interval.right;
    queue_p->entry[queue_p->top].tol = interval.tol;
    queue_p->entry[queue_p->top].f_left = interval.f_left;
    queue_p->entry[queue_p->top].f_mid = interval.f_mid;
    queue_p->entry[queue_p->top].f_right = interval.f_right;
    omp_unset_lock(&queue_p->lock); 
}

struct Interval dequeue (struct Queue *queue_p) {

    struct Interval interval; 

    omp_set_lock(&queue_p->lock); 

    if (queue_p->top == -1) {
        interval.tol=-999;                              // an attempt to dequeue empty queue
        omp_unset_lock(&queue_p->lock); 
        return interval;
    } 

    interval.left = queue_p->entry[queue_p->top].left;
    interval.right = queue_p->entry[queue_p->top].right;
    interval.tol = queue_p->entry[queue_p->top].tol;
    interval.f_left = queue_p->entry[queue_p->top].f_left;
    interval.f_mid = queue_p->entry[queue_p->top].f_mid;
    interval.f_right = queue_p->entry[queue_p->top].f_right;
    queue_p->top--;
    omp_unset_lock(&queue_p->lock); 
    return interval;
}

int steal_work(struct Queue queue_p[NUM_THREADS], int thread_n,int active_t) {
    struct Interval interval;
    for(int i=0; i<NUM_THREADS; i++){
        if(i!=thread_n) {
            if(queue_p[i].top>=1) {                          // do not dequeue a queue containing 2 or less elements
                interval = dequeue(&queue_p[i]);
                if(interval.tol==-999){                     // an attempt to dequeue an empty queue
                    continue;
                } else {
                    enqueue(interval, &queue_p[thread_n]);
                    return 1;
                }
            }
        }
    }
    return 0;
}

void init (struct Queue * queue_p) {
    queue_p->top = -1;
    omp_init_lock(&queue_p->lock);
}

int isempty (struct Queue * queue_p) {
    return (queue_p->top == -1);
}

// get current number of queue entries 
/*int size (struct Queue * queue_p) {
    return (queue_p->top + 1);
}*/


double simpson(double (*func)(double), struct Queue queue_p[NUM_THREADS])
{
    double quad = 0.0; 
    short active_t = 0;

    printf("Utilizing %d threads\n",NUM_THREADS);
#pragma omp parallel default(none) shared(active_t,quad,queue_p,func) num_threads(NUM_THREADS) 
{
    struct Interval interval;
    int thread_n = omp_get_thread_num();
    #pragma omp atomic
        active_t++;
    /* In some occasion threads would jump off at the start if no barrier synchronization at this point due to work stealing
       happenig only on queues which having larger than 2 elements */
    #pragma omp barrier              
    while(1) {
        if(!isempty(&queue_p[thread_n])) {

            interval = dequeue(&queue_p[thread_n]); 

            if(interval.tol==-999){                     // an attempt to dequeue empty queue
                continue;
            }

            double h = interval.right - interval.left;   
            double c = (interval.left + interval.right)/2.0; 
            double d = (interval.left + c)/2.0; 
            double e = (c + interval.right)/2.0; 
            double fd = func(d); 
            double fe = func(e); 

            // Compute integral estimates using 3 and 5 points respectively
            double q1 = h/6.0 * (interval.f_left + 4.0 * interval.f_mid + interval.f_right); 
            double q2 = h/12.0 * (interval.f_left + 4.0*fd + 2.0*interval.f_mid + 4.0*fe + interval.f_right);

            if ( (fabs(q2-q1) < interval.tol) || ((interval.right-interval.left) < 1.0e-12) ){
                #pragma omp atomic
	                quad +=  q2 + (q2 - q1)/15.0; 
            }
            else {
                // Tolerance is not met, split interval in two and make recursive calls
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

                enqueue(i1,&queue_p[thread_n]);
                enqueue(i2,&queue_p[thread_n]);
            } 
        } else {                     // if current threads queue is empty, try to steal work from other queues.
            if(active_t>=1) {
            #pragma omp atomic
                active_t--;
            while(active_t>=1) {     // if thread is not alone, keep trying to steal work.
                if(steal_work(&queue_p[0],thread_n,active_t)) {
                    #pragma omp atomic
                        active_t++;      // work stolen, go back to action.
                    break;              
                }
            }
            } else {                // if there are no active threads and current threads queue is empty
                #pragma omp atomic
                    active_t--;
                break; // Thread_x exiting....
            } 
        } 
    } //while(1)
}     //pragma omp parallel
   return quad; 
}

int main(void) 
{

    struct Queue queue[NUM_THREADS]; 
    struct Interval whole; 

    printf("Parallel version: Multiple Queues & Work Stealing with Lock Mechanisms\n");
    for(int i=0; i<NUM_THREADS; i++){
        init(&queue[i]);
    }

    double start = omp_get_wtime(); 

    whole.left = 0.0; 
    whole.right = 10.0; 
    whole.tol = 1e-06; 
    whole.f_left = func1(whole.left); 
    whole.f_right = func1(whole.right); 
    whole.f_mid = func1((whole.left+whole.right)/2.0); 

    enqueue(whole, &queue[0]); 

    // Call queue-based quadrature routine
    double quad = simpson(func1, &queue[0]); 
    double time = omp_get_wtime() - start;
    printf("Result = %e\n", quad); 
    printf("Time(s) = %f\n", time); 
} 
