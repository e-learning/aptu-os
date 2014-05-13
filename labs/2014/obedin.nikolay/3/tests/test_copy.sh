#!/usr/bin/env bash

source $(dirname $0)/common.sh

test() {
    run ./import    $FS_DIR $FILES_DIR/file1 /file1
    run ./import    $FS_DIR $FILES_DIR/file2 /file2
    run ./mkdir     $FS_DIR /test

    run ./copy      $FS_DIR /file1 /test/file1
    run ./copy      $FS_DIR /file2 /test/file2
    run ./mkdir     $FS_DIR /test/folder
    run ./mkdir     $FS_DIR /test/folder/another
    run ./copy      $FS_DIR /test /test2
    run fix_ls      $FS_DIR /
    run fix_ls      $FS_DIR /test
    run fix_ls      $FS_DIR /test2
    run fix_ls      $FS_DIR /test2/folder
    run ./mkdir     $FS_DIR /test3
    run ./copy      $FS_DIR /test3 /test2
    run fix_ls      $FS_DIR /test2

    run ./copy      $FS_DIR /test2 /non_existing_dir/whatever
    run ./copy      $FS_DIR /non_existing_dir/whatever /whatever
    run ./copy      $FS_DIR /non_existing_dir /whatever
    run ./copy      $FS_DIR /test2 /test2
    run ./copy      $FS_DIR /test2 /test3
}
