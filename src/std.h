#pragma once

#include <exception>
#include <new>

namespace std {

unsigned long long get_new_handler();
unsigned long long get_terminate();
unsigned long long get_unexpected();
[[noreturn]] void rethrow_exception(std::__exception_ptr::exception_ptr* exception_ptr);
unsigned long long set_new_handler(void (*handler)(void));
unsigned long long set_terminate(void (*handler)(void));
unsigned long long set_unexpected(void (*handler)(void));
[[noreturn]] void terminate();
bool uncaught_exception();
[[noreturn]] void unexpected();

}