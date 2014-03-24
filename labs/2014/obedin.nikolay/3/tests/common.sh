
# Configuration
TEST_DIR=/tmp
FS_DIR=$TEST_DIR/fs
FILES_DIR=$TEST_DIR/files
OUT_DIR=$TEST_DIR/out

# Auxilary function for redirecting output
run() {
    $@ 1>>$TEST_OUT 2>>$TEST_OUT
}

before() {
    ./format $FS_DIR
}

after() {
    ./format $FS_DIR
}
