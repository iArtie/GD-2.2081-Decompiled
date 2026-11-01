#include "pugi.h"

static void* default_allocation_function(size_t byte_count);
static void default_deallocation_function(void* p);

static void* (*s_allocationFunction)(size_t) = &default_allocation_function;
static void (*s_deallocationFunction)(void*) = &default_deallocation_function;

void* (*pugi::get_memory_allocation_function())(size_t byte_count)
{
    return s_allocationFunction;
}

void (*pugi::get_memory_deallocation_function())(void* p)
{
    return s_deallocationFunction;
}