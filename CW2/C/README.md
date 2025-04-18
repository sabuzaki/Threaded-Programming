# Parallel Adaptive Quadrature

## Description
This source code contains three versions of a parallel algorithm implementing an adaptive quadrature method. This method computes the integral of a function on a closed interval using a divide-and-conquer approach.

The algorithm starts by applying two quadrature rules (3-point and 5-point Simpson’s rules) to the whole interval. If the difference between the integral estimates from the two rules is small enough (or the interval is too short), the result is added to the total integral estimate. If it is not small enough, the interval is split into two equal halves, and the method is applied recursively to each half.

## Code Versions
The code contains three versions of the program:
- **solver1**: Threaded task-based parallelized version.
- **solver2**: Threaded single queue parallelized version.
- **solver3**: Threaded multiple queues with a work-stealing mechanism.

## Compilation Instructions
Before compiling the executables, edit the `Makefile` and update the compiler executable if needed. Then, run:
```sh
make
```

## Execution Instructions
### Running solver1 & solver2
Before executing `solver1` and `solver2`, declare the thread number variable:
```sh
export OMP_NUM_THREADS=<num_threads>
```
**Example:**
```sh
export OMP_NUM_THREADS=24
```

### Running solver3
To specify the number of threads for `solver3`, edit `solver3.c` and modify the definition of `NUM_THREADS`. Then, recompile the program.

## License
[Specify your license here]
