#ifndef FILE_H
#define FILE_H

#include "page.h"
#include <fstream>
#include <string>
#include <ctime>


void createDir(Config const &config, int pageNumber,
               std::string const &name);

std::string const getFileName(Page &page);

int getFileSize(Page &page);

time_t getFileDate(Page &page);

int getFirstFileNumber(Page &page);

void setFirstFileNumber(Page &page, int fileNumber);

int getNextFileNumber(Page &page);

int getFirstDataBlockNumber(Page &page);

void setFirstDataBlockNumber(Page &page, int number);

int getNextDataBlockNumber(Page &page);

void setNextFileNumber(Page &page, int fileNumber);

bool isDir(Page &page);

bool isEmptyDir(Page &page);

int findFileNumber(Config const &config, int dirNumber,
                   std::string const &name);


#endif // FILE_H
