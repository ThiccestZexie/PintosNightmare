#!/bin/bash
# Usage: ./lab6.sh [test_name] [pintos_options]
# Example using gdb:
# ./lab5.sh exec-once --gdb

# Define a cleanup function
cleanup() {
    # Kill the Pintos process
    kill $pintos_pid
}

# Set the trap
trap cleanup SIGINT

# exit on error
set -e

# compile
make -j

# find the test .c file in some subdirectory of tests. omit the ".." prefix and the ".c" suffix
test_file=$(find ../tests -name "$1.c" | sed 's/^\.\.\///' | sed 's/\.c$//')
echo $test_file

# if $2 is not empty, run pintos with the given options
if [ -n "$2" ]; then
    pintos $2 -v -k -T 60 --qemu --filesys-size=2 -p build/tests/userprog/child-simple -a child-simple -p build/$test_file -a $1 -- -q -f run $1 &
    pintos_pid=$!
    wait $pintos_pid
else 
    rm -f build/$test_file.*
    make build/$test_file.result
fi

echo
echo "Test source: ../$test_file.c"
echo "Test result: build/$test_file.result"
echo "Test output: build/$test_file.output"
