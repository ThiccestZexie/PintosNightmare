#!/bin/bash

# Define a cleanup function
cleanup() {
    # Kill the Pintos process
    kill $pintos_pid
}

# Set the trap
trap cleanup SIGINT

# build the test program from tests/userprog into current directory
# and run it with pintos
# Usage: ./lab5.sh [test_name] [pintos_options]

# Example using gdb:
# ./lab5.sh exec-once --gdb

# exit on error
set -e

# compile
make -j

# run pintos in the background and get its PID
pintos $2 -v -k -T 60 --qemu --filesys-size=2 -p build/tests/userprog/$1 -a $1 -- -q -f run $1 &

# Save the PID of the Pintos process
pintos_pid=$!

# Wait for the Pintos process to finish
wait $pintos_pid

echo
echo "Test output: build/tests/userprog/$1.output"
echo "Test result: build/tests/userprog/$1.result"
