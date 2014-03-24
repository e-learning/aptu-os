#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import    $FS_DIR $FILES_DIR/file1 /file1
    run ./import    $FS_DIR $FILES_DIR/file2 /file2
    run ./mkdir     $FS_DIR /test

    run ./move      $FS_DIR /file1 /test/file1
    run ./move      $FS_DIR /file2 /test/file2
    run ./move      $FS_DIR /test /test2
    run ./ls        $FS_DIR /
    run ./ls        $FS_DIR /test2

    run ./move      $FS_DIR /test2 /test2

    run ./mkdir     $FS_DIR /test2/test
    run ./move      $FS_DIR /test2 /test2/test/test

    run ./move      $FS_DIR /test2 /non_existing_dir/whatever
    run ./move      $FS_DIR /non_existing_dir/whatever /whatever
    run ./move      $FS_DIR /non_existing_dir /whatever
}
