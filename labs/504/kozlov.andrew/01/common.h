#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

typedef const vector <string> arguments;

void print_with_prefix_and_suffix(const string &str, const string &message);

void print_error(const string &cmd);
