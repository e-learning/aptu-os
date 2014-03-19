#!/bin/bash

cur_path="$(dirname $0)"
results=$cur_path"/results/"
exe=$cur_path"/../scheduler"

for test in $(ls $cur_path/*.txt)
do
{
  echo -n "Launching $test..."
	test_name=$(basename $test)
	expected=$(cat $results$test_name)
  actual=$(cat $test | $exe)
  if [ "$actual" != "$expected" ]
    then 
      echo " failed"
      echo "Actual value: $actual"
      echo "Expected value: $expected"
    else
      echo " passed"
  fi
}
done
