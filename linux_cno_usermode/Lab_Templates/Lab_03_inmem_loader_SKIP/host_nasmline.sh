#!/bin/sh

DIR="$( cd "$( dirname "$0")" ; pwd -P )"
PORT=19018
echo "hosting on port $PORT"
ncat -vvvv -k -l -c "$DIR/nasmline 2>&1" $PORT
