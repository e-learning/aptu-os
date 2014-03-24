#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./mkdir     $FS_DIR /test
    run ./ls        $FS_DIR /
    run ./ls        $FS_DIR /test
    run ./mkdir     $FS_DIR /test/test
    run ./mkdir     $FS_DIR /test

    run ./import    $FS_DIR $FILES_DIR/file1 /file1
    run ./ls        $FS_DIR /
    run ./mkdir     $FS_DIR /file1
    run ./ls        $FS_DIR /test

    run ./mkdir     $FS_DIR /
    run ./mkdir     $FS_DIR /very_long_name_here
    run ./mkdir     $FS_DIR /test/test/test/very_long_name_here
    run ./ls        $FS_DIR /test
}
