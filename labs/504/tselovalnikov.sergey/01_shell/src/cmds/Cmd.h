#pragma once

#include <string>

using namespace std;

class Cmd {
public:
    Cmd(string text) {
        this->text = text;
    };

    virtual void exec() = 0;

    virtual ~Cmd() {
    };
protected:
    string text;
};