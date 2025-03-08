#!/bin/bash
# exit on error
set -e

# Define a cleanup function
cleanup() {
    # Kill the Pintos process if it's still running
    kill $pintos_pid 2>/dev/null || true
}
trap cleanup SIGINT

# compile
make -j

# Get the test name from the first argument; additional arguments are passed as options
TEST_NAME=$1
shift

# Run pintos with the given test.
# Adjust the following command as necessary to match your Pintos run interface.
pintos -v -- run "build/tests/userprog/${TEST_NAME}.out" "$@" &
pintos_pid=$!

# Wait for the Pintos process to finish
wait $pintos_pid

echo
echo "Test output: build/tests/userprog/${TEST_NAME}.output"
echo "Test result: build/tests/userprog/${TEST_NAME}.result"
```  
