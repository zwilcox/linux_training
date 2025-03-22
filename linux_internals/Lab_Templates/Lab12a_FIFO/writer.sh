#!/bin/bash

fifo=$1


# If an argument was specified then send the argument
if [[ "$2" ]]; then
    echo "$2" > $fifo
else # else just send a default message
    echo "Hello from $$" > $fifo
fi
exit
