#!/bin/sh
njobs=50 #number of jobs in batch

mkdir jobs
for i in `seq 0 $(($njobs-1))`
do

cd jobs/jb$i
sbatch -J jb$i -o out_job_ptSpectraPp_jb$i.out jobsub_ptSpectraPp_jb$i.slurm
cd ../..

done


