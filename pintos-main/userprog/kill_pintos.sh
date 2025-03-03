#!/bin/bash

# Define the process name or keyword to grep
port="1234"

# Find processes opening the port
process_ids=$(lsof -t -i:"$port")

# Kill the processes
if [ -z "$process_ids" ]; then
    echo "No process is opening the port $port"
    exit 1
fi;

# Display process information
ps -p "$process_ids" -o time,pid,cmd
echo "Killing the process $process_ids"
kill -9 "$process_ids"