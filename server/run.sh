#!/bin/bash

make clean

make

# ls -al

echo "get ups host addr"
echo $UPS_HOST_ADDR
while true
do
    ./server $UPS_HOST_ADDR
    sleep 2
done

# while true
# do
#     sleep 1
# done