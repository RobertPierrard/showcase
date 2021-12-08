#!/bin/sh
njobs=100 #number of jobs in batch
declare -i jobid
jobid=30303524

for i in `seq 0 $(($njobs-1))`
do

scancel $jobid
let jobid=jobid+1

done
