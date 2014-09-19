#include <iostream>
#include "src/Terminal.h"

using namespace std;

int main() {
    Terminal* terminal = new Terminal();
    terminal->run();
    delete terminal;
    return 0;
}