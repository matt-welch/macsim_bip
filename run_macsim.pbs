#!/bin/bash
# filename: run_macsim.pbs
#PBS -l nodes=1:ppn=1
#PBS -j oe
#PBS -o output_macsim_test.log
#PBS -l walltime=00:10:00
#PBS -M mattw0308@gmail.com
TITLE=macsim_test
echo
echo "$TITLE run begin at $(date), PBS_JOBID=$PBS_JOBID"
#module load gcc/4.4.7
#module load python/2.7.5

# constatnts
MACSIM_HOME="/home/jwelch4/cse520/research/macsim"
MACSIM_BINDIR="$MACSIM_HOME/bin/"
PARAMS_IN="/home/jwelch4/cse520/research/macsim/bin/params.in"
TRACE_IN="/home/jwelch4/cse520/research/macsim/bin/trace_file_list"

# variables
PARAMS_CURRENT="/home/jwelch4/cse520/research/macsim/params/params_gtx580"
TRACE_CURRENT="/home/jwelch4/cse520/research/macsim/trace/trace_new"

echo
module list
echo
echo "macsim home is : $MACSIM_HOME"
echo "macsim bindir is : $MACSIM_BINDIR"
cd $MACSIM_HOME
echo "Current params file: $PARAMS_CURRENT"
echo "Current trace file: $TRACE_CURRENT"
cat $TRACE_CURRENT
cp $PARAMS_CURRENT $PARAMS_IN 
cp $TRACE_CURRENT $TRACE_IN
cd $MACSIM_BINDIR
echo "Current Directory is $(pwd)"
echo " Begin macsim...."
echo
echo
./macsim
echo
echo "$TITLE run end at $(date)"
