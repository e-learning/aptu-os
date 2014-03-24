#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import    $FS_DIR $FILES_DIR/file1 /file1
    run ./import    $FS_DIR $FILES_DIR/file2 /file2
    run ./mkdir     $FS_DIR /test
    run ./copy      $FS_DIR /file1 /test/file1
    run ./copy      $FS_DIR /file2 /test/file2
    run ./copy      $FS_DIR /test /test2
    run ./ls        $FS_DIR /
    run ./ls        $FS_DIR /test
    run ./ls        $FS_DIR /test2
}
