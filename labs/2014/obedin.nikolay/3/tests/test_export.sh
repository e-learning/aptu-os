#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import $FS_DIR $FILES_DIR/file1 /file1
    run ./export $FS_DIR /file1 /tmp/file1
    run diff /tmp/file1 $FILES_DIR/file1
    run ./export $FS_DIR /non_existing_file /tmp/non_existing_file
}
