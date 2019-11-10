#!/bin/bash

echo "--- Cleaning old build ---"
make clean
if [ $? -ne 0 ]; then
    echo "--- Build clean failed, exiting! ---"
    exit
fi

echo "--- Re-compiling with new changes ---"
make
if [ $? -ne 0 ]; then
    echo "--- Compilation with new changes failed, exiting! ---"
    exit
fi

echo "--- Running with 4 MPI processors ---"
mpirun -np 4 a1
if [ $? -ne 0 ]; then
    echo "--- Execution of MPI program failed ---"
    exit
fi
