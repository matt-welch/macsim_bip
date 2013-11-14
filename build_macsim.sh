#!/bin/bash

cd /home/jwelch4/cse520/research/macsim

module load gcc/4.4.7
module load python/2.7.5

export CXX=g++44

./build.py -d 

