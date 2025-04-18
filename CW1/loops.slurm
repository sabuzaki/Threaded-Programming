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
#module load openmpi/4.1.6

# Change to the submission directory
cd $SLURM_SUBMIT_DIR

# Set the number of threads to 1
#   This prevents any threaded system libraries from automatically
#   using threading.

export OMP_DISPLAY_ENV=FALSE
export OMP_NUM_THREADS=1
export SRUN_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK

:'
export OMP_SCHEDULE="static"
#echo "OMP_SCHEDULE to $OMP_SCHEDULE"
  for ((i=1; i<=5; i++))
  do
    srun --cpu-bind=cores ./loops
  done

export OMP_SCHEDULE="auto"
#echo "OMP_SCHEDULE to $OMP_SCHEDULE"
  for ((i=1; i<=5; i++))
  do
    srun --cpu-bind=cores ./loops
  done
'

#schedules=("static,1" "static,2" "static,4" "static,8" "static,16" "static,32" "static,64" \
#           "dynamic,1" "dynamic,2" "dynamic,4" "dynamic,8" "dynamic,16" "dynamic,32" "dynamic,64" \
#           "guided,1" "guided,2" "guided,4" "guided,8" "guided,16" "guided,32" "guided,64")

schedules=("dynamic,64", "guided,1" "guided,2" "guided,4" "guided,8" "guided,16" "guided,32" "guided,64")

for schedule in "${schedules[@]}"
do
  export OMP_SCHEDULE=$schedule
#  echo "OMP_SCHEDULE to $OMP_SCHEDULE"

  # Inner loop to run the script 5 times per schedule
  for ((i=1; i<=5; i++))
  do
    srun --cpu-bind=cores ./loops
  done
done
