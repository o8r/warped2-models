#!/bin/bash

PROG=./airport_sim
STATS_FILE=stats
GVT_METHOD=synchronous
MAX_SIM_TIME=10000
NUM_WORKER_THREADS=1
X=100
Y=100
export LD_LIBRARY_PATH=$HOME/work/warped2/lib:$HOME/lib64:$HOME/lib:/usr/lib64:$LD_LIBRARY_PATH

cd $PBS_O_WORKDIR

MPI_PROCS=`wc -l $PBS_NODEFILE | awk '{print $1}'`

mpirun -n $MPI_PROCS -hostfile $PBS_NODEFILE $PROG --max-sim-time $MAX_SIM_TIME --time-warp-worker-threads $NUM_WORKER_THREADS --time-warp-gvt-calculation-method $GVT_METHOD --time-warp-statistics-file $STATS_FILE -x $X -y $Y

