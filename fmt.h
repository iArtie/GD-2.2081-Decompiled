#pragma once

#include <cstdio>
#include <cstring>
#include <cstdarg>

class fmt {
public:
    static void ReportSystemError();
    static __int64 print(const char* format, ...);
    static __int64 print_colored(char colorCode, int arg1, int arg2, int arg3);
    static __int64 printf(const char* format, ...);
};