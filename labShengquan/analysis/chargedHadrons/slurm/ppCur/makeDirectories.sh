#!/bin/sh
njobs=50 #number of jobs in batch

for i in `seq 0 $(($njobs-1))`
do

mkdir jobs/jb$i
cp slurm_ptSpectraPp jobs/jb$i
cp jobsub_ptSpectraPp.slurm jobs/jb$i
sed "s/jb0/jb$i/g" jobs/jb$i/jobsub_ptSpectraPp.slurm > jobs/jb$i/jobsub_ptSpectraPp_jb$i.slurm
rm jobs/jb$i/jobsub_ptSpectraPp.slurm


done


