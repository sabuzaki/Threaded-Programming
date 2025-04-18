/* One queue, parallelization using ciritical sections */
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include "function.h" 

#define MAXQUEUE 10000

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
}; 

// add an interval to the queue 
void enqueue (struct Interval interval, struct Queue *queue_p){
     
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
    if (queue_p->top == -1) {
        interval.tol=-999;
        return interval;
    } 

    interval.left = queue_p->entry[queue_p->top].left;
    interval.right = queue_p->entry[queue_p->top].right;
    interval.tol = queue_p->entry[queue_p->top].tol;
    interval.f_left = queue_p->entry[queue_p->top].f_left;
    interval.f_mid = queue_p->entry[queue_p->top].f_mid;
    interval.f_right = queue_p->entry[queue_p->top].f_right;
    queue_p->top--;
    return interval;
}

// initialise queue
void init (struct Queue * queue_p) {
    queue_p->top = -1;
}

// return whether queue is empty 
int isempty (struct Queue * queue_p) {
    return (queue_p->top == -1);
}

// get current number of queue entries 
/*int size (struct Queue * queue_p) {
    return (queue_p->top + 1);
}*/


double simpson(double (*func)(double), struct Queue * queue_p)
{
    double quad = 0.0; 
    short active_t = 0;
    int num_t = 0;
#pragma omp parallel default(none) shared(active_t,quad,func, queue_p, num_t)
{
    #pragma omp single
    {
        num_t = omp_get_num_threads();
        printf("Utilizing %d threads\n",num_t);
    }
    struct Interval interval;
    while (!isempty(queue_p) || active_t >0) {
        #pragma omp atomic
        active_t++;
        #pragma omp critical
            interval = dequeue(queue_p); 
        if(interval.tol==-999) {
	        #pragma omp atomic 		// somewhat performs better then critical. Must wait when others dequeueing?
	            active_t--;
            printf(".");		    // for unknown reasons, print is required. If missing, active_t not being reduced.
            continue;   
	}
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
            #pragma omp atomic
	            quad +=  q2 + (q2 - q1)/15.0; 
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
	        #pragma omp critical
            { 
	            enqueue(i1,queue_p);
           	    enqueue(i2,queue_p);
            }

            } 
        #pragma omp atomic
            active_t--;
   } 
}
   
    return quad; 
}

int main(void) 
{

    struct Queue queue; 
    struct Interval whole; 

    init(&queue);

    short active_t = 0;

    printf("Parallel version: Single Queue with Critical Sections.\n");
    double start = omp_get_wtime(); 

    whole.left = 0.0; 
    whole.right = 10.0; 
    whole.tol = 1e-06; 
    whole.f_left = func1(whole.left); 
    whole.f_right = func1(whole.right); 
    whole.f_mid = func1((whole.left+whole.right)/2.0); 

    enqueue(whole, &queue); 

    double quad = simpson(func1, &queue); 
    double time = omp_get_wtime() - start;
    printf("\nResult = %e\n", quad); 
    printf("Time(s) = %f\n", time); 
} 
