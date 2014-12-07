#!/bin/bash

current_path=$(pwd)
old_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
LD_LIBRARY_PATH="$current_path:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH
./shell
LD_LIBRARY_PATH=old_LD_LIBRARY_PATH
export LD_LIBRARY_PATH 
