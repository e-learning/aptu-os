#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import    $FS_DIR $FILES_DIR/file1 /file1
    run ./import    $FS_DIR $FILES_DIR/file2 /file2
    run ./mkdir     $FS_DIR /test
    run ./copy      $FS_DIR /file1 /test/file1
    run ./copy      $FS_DIR /file2 /test/file2

    run ./ls        $FS_DIR /
    run ./rm        $FS_DIR /file1
    run ./rm        $FS_DIR /test/file2
    run ./ls        $FS_DIR /
    run ./ls        $FS_DIR /test

    run ./mkdir     $FS_DIR /test/test2
    run ./copy      $FS_DIR /file2 /test/test2/file2
    run ./ls        $FS_DIR /test
    run ./ls        $FS_DIR /test/test2
    run ./rm        $FS_DIR /test/
    run ./ls        $FS_DIR /
    run ./rm        $FS_DIR /
    run ./ls        $FS_DIR /

    run ./rm        $FS_DIR /non_existent_dir
    run ./import    $FS_DIR $FILES_DIR/file2 /file1
    run ./copy      $FS_DIR /file1 /file2
    run ./copy      $FS_DIR /file1 /file3
    run ./copy      $FS_DIR /file1 /file4
}
