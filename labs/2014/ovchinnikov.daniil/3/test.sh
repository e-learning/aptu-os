#!/bin/bash
[ "$1" == "" ] && echo "Please specify file" && exit 1

if [ "$2" == "" ]; then 
        ROOT=. 
else
        ROOT=$2
fi

function assert {
    if [ "$1" != "$2" ]; then
            echo "'$1' not equals '$2'"
            exit 1
    fi
}

assert "`./init`" "Usage: init root"
#assert "`./init .`" "" 
assert "`./format`" "Usage: format root"
assert "`./format .`" ""
assert "`./mkdir`" "Usage: mkdir root path"
assert "`./mkdir .`" "Usage: mkdir root path"
assert "`./mkdir . lol 2>&1`" "Path should start with /"
assert "`./mkdir . lol/azaza 2>&1`" "Path should start with /"
./mkdir . /hell/yeah/fuckit
./mkdir . /hell/azaza
./mkdir . /lol/1
./mkdir . /lol/2
./mkdir . /lol/3
assert "`./import . $1 /lol 2>&1`" "'lol' is a directory"
assert "`./import . $1 /lol/file 2>&1`" ""
assert "`./move . /lol/ /hell 2>&1`" ""
assert "`./rm . /hel/azaza 2>&1`" "Cannot find 'hel'"
assert "`./rm . /hell/azaza 2>&1`" ""
assert "`./move . /hell/1 /hell/file 2>&1`" "'file' is a file" 
#assert "`./move . /hell/file /file 2>&1`" "" 
#assert "`./move . /algo /1/2/3/4/5/6 2>&1`" "" 
#assert "`./mkdir . /1/2/3/4/5/6 2>&1`" "'6' is a file" 
assert "`./export`" "Usage: export root fs_path host_path" 
assert "`./export .`" "Usage: export root fs_path host_path" 
assert "`./export . sdfg`" "Usage: export root fs_path host_path" 
assert "`./export . sdfg azaza 2>&1`" "Path should start with /" 
#assert "`./export . /1/2/3/4/5/6 /file 2>&1`" "Cannot open destination file"
assert "`./export . /hell/file file 2>&1`" ""
assert "`diff file $1`" ""
#assert "`./rm . /1 2>&1`" ""
#assert "`./mkdir . /1/2/3/4/5/6 2>&1`" "" 
assert "`./copy . /hell /hell_copy 2>&1`" ""
assert "`./rm . /hell 2>&1`" ""
assert "`./export . /hell_copy/file file 2>&1`" ""
assert "`diff file $1`" ""

