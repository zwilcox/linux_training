#!/bin/bash

fifo=/tmp/myfifo

# On exit run this command
trap "rm -f $fifo" EXIT

# Check if fifo exists. If not then create it
if [[ ! -p $fifo ]]; then
    mkfifo $fifo
fi

# Read in fifo data and echo
while true
do
    if read line < $fifo; then
        if [[ "$line" == quit ]]; then
            break
        fi
        echo $line
    fi
done
