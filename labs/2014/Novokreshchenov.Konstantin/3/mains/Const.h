#ifndef CONST_H
#define CONST_H

#include <cstddef>
#include <string>
#include <cmath>

static const std::string CONFIG = "config";

static const size_t BITS_PER_BYTE = 8;
static const size_t MAX_BYTE = (size_t)(pow(2, BITS_PER_BYTE) - 1);

//root catalog
static const size_t ROOT_BLOCK_NUMBER = 0;
static const size_t BITMAP_FIRST_BLOCK_NUMBER = 1;
static const size_t BYTES_FOR_BLOCKS_COUNT = 2;
static const size_t BYTES_FOR_BLOCKSIZE = 4;
static const size_t BYTES_FOR_BITMAP_COUNT = 2;

//catalog
static const size_t BYTES_FOR_RECORDS_COUNT = 4;
static const size_t BYTES_FOR_ISDIR = 1;
static const size_t BYTES_FOR_NAME = 10;
static const size_t BYTES_FOR_FILESIZE = 4;
static const size_t BYTES_FOR_TIME = 4;
static const size_t BYTES_FOR_BLOCKNUMBER = 2;
static const size_t BYTES_FOR_RECORD = BYTES_FOR_ISDIR
                                     + BYTES_FOR_NAME
                                     + BYTES_FOR_FILESIZE
                                     + BYTES_FOR_TIME
                                     + BYTES_FOR_BLOCKNUMBER;
static const size_t BYTES_FOR_TIMEINFO = 80;

static const size_t ISDIR = 1;
static const size_t ISFILE = 0;
static const size_t LAST_BLOCK = 65535;

//errors
static const size_t SUCCESS = 0;
static const size_t ERROR = 59999;
static const size_t ERROR_NUMBER_NOT_FOUND_BLOCK = 60000;
static const size_t ERROR_NO_FREE_BLOCK = 60001;
static const size_t ERROR_NOT_ENOUGH_SPACE = 60002;
static const size_t ERROR_FEW_ARGS = 60003;
static const size_t ERROR_READ_CONFIG = 60004;
static const size_t ERROR_INCORRECT_PATH = 60005;
static const size_t ERROR_INIT = 60006;
static const size_t ERROR_INVALID_NAME = 60007;

#endif // CONST_H
