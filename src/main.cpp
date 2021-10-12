#include "utils/logger.h"

#ifdef _WIN32
#include <Windows.h>
#endif



int main() {
    init_logger();

#ifdef _WIN32
    // prevent computer to sleep
    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif


    return 0;
}