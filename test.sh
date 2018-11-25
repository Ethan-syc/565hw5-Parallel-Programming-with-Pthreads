#!/bin/bash
j=1
for i in {1..4}; do
  ./rainfall $j 50 0.5 4096 measurement_4096x4096.in > result_$j.out
  echo "using $j threads:"
  ./check.py 4096 measurement_4096x4096.out result_$j.out
  let "j = j * 2"
done
