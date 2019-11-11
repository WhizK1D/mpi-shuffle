#!/bin/bash

###### select partition (check CCR documentation)
#SBATCH --partition=debug --qos=debug

####### set memory that nodes provide (check CCR documentation, e.g., 32GB)
#SBATCH --mem=48000

####### make sure no other jobs are assigned to your nodes
#SBATCH --exclusive

####### further customizations
#SBATCH --job-name="mpi-shuffling"
#SBATCH --output=results/%j.stdout
#SBATCH --error=results/%j.stderr
#SBATCH --nodes=2
##SBATCH --ntasks-per-node=2
#SBATCH --time=00:15:00
##SBATCH --mail-type=END
##SBATCH --mail-user=amourya@buffalo.edu

export I_MPI_DEBUG=4
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi.so
module load intel-mpi/2018.3

srun -n 8 ./a1
