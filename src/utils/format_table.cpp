#include "format_table.h"


#ifdef _WIN32
#include <Windows.h>
#endif


FormatTable::FormatTable(const std::string& name)
    : name_(name) {
#ifdef _WIN32
    HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO scr;
    ::GetConsoleScreenBufferInfo(hout, &scr);
    max_width_ = scr.dwMaximumWindowSize.X - 1;
#endif

}
