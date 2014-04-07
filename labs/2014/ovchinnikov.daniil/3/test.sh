#!/bin/bash
[ "$1" == "" ] && echo "Please specify file" && exit 1

if [ "$2" == "" ]; then 
        ROOT=. 
else
        ROOT=$2
fi

echo "ROOT=$ROOT"
echo "Test with file $1"

function assert {
    if [ "$1" != "$2" ]; then
            echo "'$1' not equals '$2'"
            exit 1
    fi
}

assert "`./init`" "Usage: init root"
#assert "`./init .`" "" 
assert "`./format`" "Usage: format root"
assert "`./format $ROOT`" ""
assert "`./mkdir`" "Usage: mkdir root path"
assert "`./mkdir $ROOT`" "Usage: mkdir root path"
assert "`./mkdir $ROOT lol 2>&1`" "Path should start with /"
assert "`./mkdir $ROOT lol/azaza 2>&1`" "Path should start with /"
./mkdir $ROOT /hell/yeah/fuckit
./mkdir $ROOT /hell/azaza
./mkdir $ROOT /lol/1
./mkdir $ROOT /lol/2
./mkdir $ROOT /lol/3
assert "`./import $ROOT $1 /lol 2>&1`" "'lol' is a directory"
assert "`./import $ROOT $1 /lol/file 2>&1`" ""
assert "`./move $ROOT /lol/ /hell 2>&1`" ""
assert "`./rm $ROOT /hel/azaza 2>&1`" "Cannot find 'hel'"
assert "`./rm $ROOT /hell/azaza 2>&1`" ""
assert "`./move $ROOT /hell/1 /hell/file 2>&1`" "'file' is a file" 
#assert "`./move $ROOT /hell/file /file 2>&1`" "" 
#assert "`./move $ROOT /algo /1/2/3/4/5/6 2>&1`" "" 
#assert "`./mkdir $ROOT /1/2/3/4/5/6 2>&1`" "'6' is a file" 
assert "`./export`" "Usage: export root fs_path host_path" 
assert "`./export $ROOT`" "Usage: export root fs_path host_path" 
assert "`./export $ROOT sdfg`" "Usage: export root fs_path host_path" 
assert "`./export $ROOT sdfg azaza 2>&1`" "Path should start with /" 
#assert "`./export $ROOT /1/2/3/4/5/6 /file 2>&1`" "Cannot open destination file"
assert "`./export $ROOT /hell/file file 2>&1`" ""
assert "`diff file $1`" ""
#assert "`./rm $ROOT /1 2>&1`" ""
#assert "`./mkdir $ROOT /1/2/3/4/5/6 2>&1`" "" 
assert "`./copy $ROOT /hell /hell_copy 2>&1`" ""
assert "`./rm $ROOT /hell 2>&1`" ""
assert "`./export $ROOT /hell_copy/file file 2>&1`" ""
assert "`diff file $1`" ""

rm file
