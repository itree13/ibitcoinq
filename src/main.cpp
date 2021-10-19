#include "okex/client.h"
#include "utils/logger.h"
#include "utils/concurrent_queue.h"
#include "utils/utils.h"
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif


ConcurrentQueueT<bool> g_console_break_noti;

#ifdef _WIN32
static BOOL WINAPI handle_console_routine(DWORD ctrl_type) {
    if (ctrl_type == CTRL_C_EVENT) {
        g_console_break_noti.push(true);
        return TRUE;
    }
    return FALSE;
}
#endif


int main() {
    init_logger();

#ifdef _WIN32
    // prevent computer to sleep
    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif

    if (!okex::g_client.start("setting.conf"))
        return -1;


#ifdef _WIN32
    ::SetConsoleCtrlHandler(handle_console_routine, TRUE);
    HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    for (;;) {
        std::cout << "> ";

        std::string op;
        for (;;) {
            std::getline(std::cin, op);
            if (!std::cin.good())
                std::cin.clear();
            else
                break;
        }
        trimString(op);

        if (op == "help") {
            std::cout << "commands:" << std::endl;
            std::cout << "\tshow position" << std::endl;
            std::cout << "\tshow balance" << std::endl;
            std::cout << "\tshow instruments" << std::endl;
        } else {

        }


        // TODO


        bool notiv;
        while (g_console_break_noti.tryPop(&notiv));

#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO scr;
        ::GetConsoleScreenBufferInfo(hout, &scr);
#endif
        do {
#ifdef _WIN32
            DWORD written;
            ::FillConsoleOutputCharacterA(
                hout, ' ', scr.dwSize.X * scr.dwSize.Y, scr.dwCursorPosition, &written
            );
            ::SetConsoleCursorPosition(hout, scr.dwCursorPosition);
#endif
            // TODO

        } while (!g_console_break_noti.pop(&notiv, std::chrono::seconds(5)));
    }

#ifdef _WIN32
    ::CloseHandle(hout);
#endif

    return 0;
}