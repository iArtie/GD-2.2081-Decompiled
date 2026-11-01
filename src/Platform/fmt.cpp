#include "Platform/fmt.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>

void fmt::ReportSystemError() {
    // Empty implementation in decompiled code
}

__int64 fmt::print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[500];
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    va_end(args);
    
    // Output to stdout
    fwrite(buffer, 1, strlen(buffer), stdout);
    
    return 0;
}

__int64 fmt::print_colored(char colorCode, int arg1, int arg2, int arg3) {
    char colorStr[8];
    strcpy(colorStr, "\x1B[30m");
    colorStr[3] = colorCode + '0';
    
    fputs(colorStr, stdout);
    
    // Print the actual content
    fmt::print(arg1, arg2, arg3); // This would need the actual format implementation
    
    fwrite("\x1B[0m", 1, 4, stdout);
    
    return 0;
}

__int64 fmt::printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[500];
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    va_end(args);
    
    fwrite(buffer, 1, strlen(buffer), stdout);
    
    return 0;
}