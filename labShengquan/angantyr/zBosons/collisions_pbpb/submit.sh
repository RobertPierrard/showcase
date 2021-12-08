#!/bin/sh
nbatch=1010 #batch number
njobs=100 #number of jobs in batch

ban=ba$nbatch
mkdir $ban
mkdir /store/user/pierrarj/collisions_pbpb/zBoson/$ban #create batch directory for storage

for i in `seq 0 $(($njobs-1))`
do

echo $i
jbi=jb$i
mkdir $ban/$jbi
mkdir /store/user/pierrarj/collisions_pbpb/zBoson/$ban/$jbi #create job directory for storage

cp pbpb_angantyr_5tev_zBoson $ban/$jbi
cp jobsub.slurm $ban/$jbi
sed "s/jb0/$jbi/g" $ban/$jbi/jobsub.slurm > $ban/$jbi/jobsub_${ban}_$jbi.slurm
sed -i "s/ba0/$ban/g" $ban/$jbi/jobsub_${ban}_$jbi.slurm
sed -i "s/batchNum/$nbatch/g" $ban/$jbi/jobsub_${ban}_$jbi.slurm
sed -i "s/jobNum/$i/g" $ban/$jbi/jobsub_${ban}_$jbi.slurm
rm $ban/$jbi/jobsub.slurm

cd $ban/$jbi
sbatch -J ${ban}jb$i -o out_job_${ban}_$jbi.out jobsub_${ban}_$jbi.slurm
cd ../..

done
