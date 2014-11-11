#include "INode.h"

bool operator==(const INode& left, const INode& right) {
    return left.id == right.id;
}