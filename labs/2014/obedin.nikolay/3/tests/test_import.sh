#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import $FS_DIR $FILES_DIR/too_big_file /whatever

    run ./import $FS_DIR $FILES_DIR/file1 /file1

    rm -rf $FILES_DIR/non_existing_file
    run ./import $FS_DIR $FILES_DIR/non_existing_file /non_existing_file

    run ./import $FS_DIR $FILES_DIR/file1 /very_long_name_here
    run ./import $FS_DIR $FILES_DIR/file1 /none/whatever
}
