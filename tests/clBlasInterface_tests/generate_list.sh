#!/bin/bash

rm -f test_list
#for test_file in `ls src`; do
#  echo $test_file | sed -e "s/\.cpp//g" >> test_list;
#done;

echo TestclBLASexceptions > test_list

for a in src/TestclBLAS*S*; do b=${a#src/}; echo ${b%.cpp} >> test_list; done
for a in src/TestclBLAS*C*; do b=${a#src/}; echo ${b%.cpp} >> test_list; done
for a in src/TestclBLAS*D*; do b=${a#src/}; echo ${b%.cpp} >> test_list; done

#for a in src/TestclBLASZ*; do b=${a#src/}; echo ${b%.cpp} >> test_list; done
