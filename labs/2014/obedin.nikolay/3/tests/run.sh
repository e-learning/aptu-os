#!/usr/bin/env bash


source $(dirname $0)/common.sh

echo "Testing $1... "

TEST_OUT=$OUT_DIR/test_$1.out
source $(dirname $0)/test_$1.sh
before
test 1>/dev/null 2>/dev/null
after

diff $TEST_OUT $(dirname $0)/test_$1.out 1>/dev/null 2>/dev/null

if [[ $? -ne 0 ]]; then
    echo -e "\tFAIL. Output saved into $TEST_OUT"
else
    echo -e "\tOK"
fi
