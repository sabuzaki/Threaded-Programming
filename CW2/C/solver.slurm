#!/bin/bash

# Slurm job options (name, compute nodes, job time)
#SBATCH --job-name=ADI
#SBATCH --time=0:30:00
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=32

# Replace [budget code] below with your budget code (e.g. t01)
#SBATCH --account=m24ol-s2618173
#SBATCH --partition=standard
#SBATCH --qos=standard

#module load intel-tools-19
#module load intel-20.4/psxe
module load intel-20.4/compilers
module load scalasca
#module load openmpi/4.1.6

# Change to the submission directory
cd $SLURM_SUBMIT_DIR

# Set the number of threads to 1
#   This prevents any threaded system libraries from automatically
#   using threading.

export OMP_DISPLAY_ENV=FALSE
#export OMP_NUM_THREADS=1
export SRUN_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK
SCOREP_TOTAL_MEMORY=16375808

#module load intel-20.4/psxe
#module load intel-20.4/compilers
#module load gcc/10.2.0

#srun --cpu-bind=cores scalasca -analyze ./solver1

#export OMP_NUM_THREADS=32
#srun --cpu-bind=cores ./solver3
#export OMP_NUM_THREADS=24
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=16
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=12
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=8
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=6
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=4
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=2
#srun --cpu-bind=cores ./solver2
#export OMP_NUM_THREADS=1
#srun --cpu-bind=cores ./solver2








