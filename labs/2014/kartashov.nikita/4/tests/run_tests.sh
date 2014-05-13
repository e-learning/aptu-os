#!/bin/bash

cur_path="$(dirname $0)"
results=$cur_path"/results/"
exe=$cur_path"/../allocator"
failed=0
passed=0
run=0
for test in $(ls $cur_path/*.txt)
do
{
  echo -n "Launching $test..."
	test_name=$(basename $test)
	expected=$(cat $results$test_name)
  actual=$(cat $test | $exe)
  let "run = run + 1"
	if [ "$actual" != "$expected" ]
    then
			let "failed = failed + 1"
      echo " failed"
      echo "Actual value: $actual"
      echo "Expected value: $expected"
    else
			let "passed = passed + 1"
      echo " passed"
		fi
}
done

echo "Launched $run tests, failed $failed, passed $passed" 
