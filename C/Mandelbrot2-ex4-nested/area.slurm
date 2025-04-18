#!/bin/bash

# Slurm job options (name, compute nodes, job time)
#SBATCH --job-name=Mandelbrot
#SBATCH --time=0:10:0
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=36

# Replace [budget code] below with your budget code (e.g. dc116-s1234567)
#SBATCH --account=[budget code]
# We use the "standard" partition as we are running on CPU nodes
#SBATCH --partition=standard
# We use the "short" QoS as our runtime is less than20 mins 
#SBATCH --qos=short

module load intel-20.4/compilers

# Change to the submission directory
cd $SLURM_SUBMIT_DIR

# Set the number of threads 
export OMP_NUM_THREADS=2,4
export OMP_NESTED=true

# Launch the parallel job
srun --cpu-bind=cores ./area
