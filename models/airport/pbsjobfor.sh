#!/bin/bash

export LD_LIBRARY_PATH=$HOME/work/warped2/lib:$HOME/lib64:$HOME/lib:/usr/lib64:$LD_LIBRARY_PATH

cd $HOME/work/warped2-models/models/airport

TIMES=10

#Ns="2 4 8 16 32 64 128 256"
MPI_PROCS=`wc -l $PBS_NODEFILE | awk '{print $1}'`

C=1
Tbegin=10
Tend=50
Tskip=10

T=$Tbegin
while [ $T -lt $Tend ]; do
  I=0
  while [ $I -lt $TIMES ]; do
    CMD="./test_rejuvenation.sh -C $C -T $T -N $MPI_PROCS -h $PBS_NODEFILE"
    eval $CMD
    I=$((I+1))
  done
  T=$((T+Tskip))
done
