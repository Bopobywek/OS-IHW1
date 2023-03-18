#!/bin/bash

make
rm tests/output/*

for file in ../tests/input/*
do
    if [ -f "$file" ]; then
        ./io_manipulator "$file" "tests/output/$(basename "$file").a" &
        ./transformer
        wait
    fi
done

make clean
