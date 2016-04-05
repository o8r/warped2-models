#!/bin/bash

#############
# Constants #
#############
# MPI driver
MPIRUN="mpirun"
# Program name
PROG=./airport_sim
# GVT calculation method
GVT_CALC=synchronous
# Checkpointing method
CHECKPOINTING=gvt-synchronized
# Exit status file name
EXIT_STATUS_FILE=exit_status

##############
# Parameters #
##############
# MPI processes
NP=2
# Threads per process
NUM_THREADS=1
# Width of simulation grid
X=100
# Height of simulation grid
Y=100
# max-sim-time
MAX_SIM_TIME=0
# checkpoint interval in terms of GVT synchronization
CHECKPOINT_INTERVAL=10
# checkpoint count till rejuvenation
CHECKPOINT_COUNT=10
# stats file
STATS_FILE=stats
# virtual (dry-run)
VIRTUAL_MODE=""

# Parse command line
while getopts N:L:T:C:h:v OPT; do
    case $OPT in
	"N" ) NP=$OPTARG ;;
  	"n" ) NUM_THREADS=$OPTARG ;;
	"L" ) MAX_SIM_TIME=$OPTARG ;;
	"T" ) CHECKPOINT_INTERVAL=$OPTARG ;;
	"C" ) CHECKPOINT_COUNT=$OPTARG ;;
	"f" ) STATS_FILE=$OPTARG ;;
	"h" ) HOSTFILE=$OPTARG ;;
	"v" ) VIRTUAL_MODE="yes" ;;
    esac
done

if [ -n "$HOSTFILE" ]; then
  HOSTFILE="-hostfile $HOSTFILE"
fi

CMD="$PROG -x $X -y $Y --max-sim-time $MAX_SIM_TIME --time-warp-worker-threads $NUM_THREADS --time-warp-gvt-calculation-method $GVT_CALC --checkpointing-method $CHECKPOINTING --checkpointing-interval $CHECKPOINT_INTERVAL --checkpointing-count-till-termination $CHECKPOINT_COUNT --time-warp-statistics-file $STATS_FILE"
if [ "x$VIRTUAL_MODE" = "xyes" ]; then
    echo "$MPIRUN -n $NP $HOSTFILE $CMD"
    exit 0
fi

# The first run
rm $EXIT_STATUS_FILE
eval "printf '# $CMD\n' >> $STATS_FILE"
$MPIRUN -n $NP $HOSTFILE $CMD
eval "printf '\n' >> $STATS_FILE"

# Check exit status
if [ -e $EXIT_STATUS_FILE ]; then
    EXIT_STATUS=`cat $EXIT_STATUS_FILE`
else
    echo "Cannot obtain exit status"
    exit 1
fi

while [ $EXIT_STATUS -ne 0 ]; do
  rm $EXIT_STATUS_FILE
  eval "printf '# $CMD\n' >> $STATS_FILE"
  $MPIRUN -n $NP $HOSTFILE $CMD --checkpointing-restart on
  eval "printf '\n' >> $STATS_FILE"
  # Check exit status
  if [ -e $EXIT_STATUS_FILE ]; then
      EXIT_STATUS=`cat $EXIT_STATUS_FILE`
  else
      echo "Cannot obtain exit status"
      exit 1
  fi
done
