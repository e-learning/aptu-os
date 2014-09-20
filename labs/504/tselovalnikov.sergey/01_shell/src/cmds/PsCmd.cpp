#include <algorithm>
#include "PsCmd.h"

#include "../utils/ProcessUtils.h"

void PsCmd::exec() {
    cout <<  utils::listOfAllProcess() << endl << flush;
}
