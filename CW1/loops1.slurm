#!/bin/bash

# Slurm job options (name, compute nodes, job time)
#SBATCH --job-name=ADI
#SBATCH --time=0:10:00
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=32

# Replace [budget code] below with your budget code (e.g. t01)
#SBATCH --account=m24ol-s2618173
#SBATCH --partition=standard
#SBATCH --qos=short

#module load intel-tools-19
#module load intel-20.4/psxe
module load intel-20.4/compilers
#module load openmpi/4.1.6

# Change to the submission directory
cd $SLURM_SUBMIT_DIR

# Set the number of threads to 1
#   This prevents any threaded system libraries from automatically
#   using threading.

export OMP_DISPLAY_ENV=FALSE
export OMP_NUM_THREADS=32
export SRUN_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK
export OMP_SCHEDULE="dynamic,2"


srun --cpu-bind=cores ./loops
#free -h
#cat /proc/meminfo | grep MemTotal


#export OMP_SCHEDULE="dynamic,5"
#export OMP_SCHEDULE="guided,5"
