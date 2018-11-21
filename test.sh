#!/bin/bash
for i in {1..10000}; do
  ./rainfall 8 50 0.5 4096 measurement_4096x4096.in > result.out
  ./check.py 4096 sample_512x512.out result.out
done
