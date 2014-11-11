#!/bin/bash
./init /tmp/fs
./format /tmp/fs
./import /tmp/fs /tmp/a /b
./export /tmp/fs /b /tmp/t
cmp /tmp/a /tmp/t
