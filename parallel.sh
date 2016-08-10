#!/bin/bash

if [ -z $PARALLEL ]; then
  PARALLEL=${1-1}
fi

./shmem_server > /dev/null &
SERVER_PID=$!
echo Starting server. PID  $SERVER_PID


echo Creating $PARALLEL clients...

declare -a CLIENT_JOBS

for ((i=0;i<$PARALLEL;i++)); do
  ./shmem_client  &
  CLIENT_JOBS[$i]=$!
  echo "PID ${CLIENT_JOBS[$i]}"
  echo Started client $i
done

trap ctrl_c INT

function ctrl_c() {

for ((i=0;i<$PARALLEL;i++)); do
  echo Killing client $i 
  kill ${CLIENT_JOBS[$i]}
  if [ $? -eq 0 ]; then
    echo "Client $i completed successfully"
  else
    echo Client $i failed
  fi
done  

kill $SERVER_PID
trap - INT
}

for ((i=0;i<$PARALLEL;i++)); do
  echo Waiting for client $i 
  wait ${CLIENT_JOBS[$i]}
  if [ $? -eq 0 ]; then
    echo "Client $i completed successfully"
  else
    echo Client $i failed
  fi
done

echo Killing server
kill $SERVER_PID

trap - INT
