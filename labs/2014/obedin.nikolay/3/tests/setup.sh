#!/usr/bin/env bash

source $(dirname $0)/common.sh

echo "Setup tests environment..."

rm -rf   $FS_DIR
mkdir -p $FS_DIR
echo "1024" >> $FS_DIR/config
echo "100"  >> $FS_DIR/config
./init   $FS_DIR

rm -rf   $FILES_DIR
mkdir -p $FILES_DIR
dd if=/dev/urandom of=$FILES_DIR/too_big_file bs=1024 count=101 1>/dev/null 2>/dev/null
dd if=/dev/urandom of=$FILES_DIR/file1 bs=1024 count=10 1>/dev/null 2>/dev/null
dd if=/dev/urandom of=$FILES_DIR/file2 bs=1024 count=20 1>/dev/null 2>/dev/null

rm -rf   $OUT_DIR
mkdir -p $OUT_DIR
