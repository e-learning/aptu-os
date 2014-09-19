#include <algorithm>
#include "PsCmd.h"

#include "../utils/ProcessUtils.h"

string PsCmd::exec() {
    return utils::listOfAllProcess();
}
