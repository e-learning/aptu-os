#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import $FS_DIR $FILES_DIR/file1 /file1
    rm -rf $FILES_DIR/non_existing_file
    run ./import $FS_DIR $FILES_DIR/non_existing_file /non_existing_file
}
