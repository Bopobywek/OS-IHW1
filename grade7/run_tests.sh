#!/bin/bash

make
rm tests/output/*

for file in ../tests/input/*
do
    if [ -f "$file" ]; then
        ./program "$file" "tests/output/$(basename "$file").a"
    fi
done

make clean
