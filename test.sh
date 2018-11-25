#!/bin/bash
./rainfall_seq 50 0.5 4096 measurement_4096x4096.in > result_seq.out
echo "sequential code:"
./check.py 4096 measurement_4096x4096.out result_seq.out
head -n3 result_seq.out
j=1  
for i in {1..4}; do
  ./rainfall_pt $j 50 0.5 4096 measurement_4096x4096.in > result_pt_$j.out
  echo "using $j threads:"
  ./check.py 4096 measurement_4096x4096.out result_pt_$j.out
  head -n3 result_pt_$j.out
  let "j = j * 2"
done
