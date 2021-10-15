#include "utils/logger.h"
#include "okex/client.h"

#ifdef _WIN32
#include <Windows.h>
#endif



int main() {
    init_logger();

#ifdef _WIN32
    // prevent computer to sleep
    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif

    if (!okex::g_client.start("setting.conf"))
        return -1;



    return 0;
}