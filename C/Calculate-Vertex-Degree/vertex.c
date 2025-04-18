#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NEDGES 6  // Number of edges
#define NVERTICES 5  // Number of vertices

// Define an edge structure
typedef struct {
    int vertex1;
    int vertex2;
} Edge;

int main() {
    Edge edge[NEDGES];  // Array to store edges
    int degree[NVERTICES] = {0};  // Initialize degrees to 0

    // Define edges (using proper syntax)
    edge[0].vertex1 = 0; edge[0].vertex2 = 1;
    edge[1].vertex1 = 1; edge[1].vertex2 = 2;
    edge[2].vertex1 = 1; edge[2].vertex2 = 3;
    edge[3].vertex1 = 2; edge[3].vertex2 = 3;
    edge[4].vertex1 = 2; edge[4].vertex2 = 4;
    edge[5].vertex1 = 3; edge[5].vertex2 = 4;

    omp_set_num_threads(2);

    // Parallel computation of vertex degrees
    #pragma omp parallel for
    for (int j = 0; j < NEDGES; j++) {
        #pragma omp atomic
        degree[edge[j].vertex1]++;  // Increment degree of vertex1
        
        #pragma omp atomic
        degree[edge[j].vertex2]++;  // Increment degree of vertex2
    }

    // Print the degree of each vertex
    printf("Vertex Degrees:\n");
    for (int i = 0; i < NVERTICES; i++) {
        printf("Vertex %d: Degree %d\n", i, degree[i]);
    }

    return 0;
}
