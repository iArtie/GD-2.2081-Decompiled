#pragma once

#include <cstddef>

class pugi {
public:
    static void* (*get_memory_allocation_function())(size_t byte_count);
    static void (*get_memory_deallocation_function())(void* p);
};