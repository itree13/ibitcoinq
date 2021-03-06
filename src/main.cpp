#include "okex/client.h"
#include "trades_manager.h"
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


void loopProcess(std::function<void()> func) {
    bool notiv;
    while (g_console_break_noti.tryPop(&notiv));

#ifdef _WIN32
    HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
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
        func();

    } while (!g_console_break_noti.pop(&notiv, std::chrono::seconds(5)));
}


int main() {
    init_logger();

#ifdef _WIN32
    // prevent computer from sleeping
    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif

    if (!okex::g_client.start("setting.conf"))
        return -1;


#ifdef _WIN32
    ::SetConsoleCtrlHandler(handle_console_routine, TRUE);
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
            std::cout << "\tshow tickers" << std::endl;
        } else if (op == "show position") {
            g_trades_man.process(op);
        } else if (op == "show balance") {
            g_trades_man.process(op);
        } else if (op == "show instruments") {
            g_trades_man.process(op);
        } else if (op == "show tickers") {
            loopProcess([op] { g_trades_man.process(op); });
        }

    }
    return 0;
}