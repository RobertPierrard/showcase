#!/bin/bash
#SBATCH --nodes=1
#SBATCH --mem-per-cpu=2G
#SBATCH --time=1-00:00:00


echo $SLURM_JOB_ID
echo $SLURM_JOB_NAME
echo $SLURM_JOB_NODELIST

source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc700

cd /scratch/pierrarj/ROOT/CMSSW_10_3_0 

eval `scramv1 runtime -sh`

export PYTHIA8DATA=/scratch/pierrarj/pythia8305/share/Pythia8/xmldoc

cd /scratch/pierrarj/pythia8305/examples/angantyr/collisions_pp/ba100/jb65

./pp_angantyr_5tev 100 65

echo "Done!"

exit 0


