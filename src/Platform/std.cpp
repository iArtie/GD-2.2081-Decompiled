#include "Platform/std.h"
#include <cxxabi.h>

namespace std {

unsigned long long get_new_handler() {
    return __atomic_load_n(&__new_handler_storage, __ATOMIC_RELAXED);
}

unsigned long long get_terminate() {
    return __atomic_load_n(&__cxxabiv1::__terminate_handler, __ATOMIC_RELAXED);
}

unsigned long long get_unexpected() {
    return __atomic_load_n(&__cxxabiv1::__unexpected_handler, __ATOMIC_RELAXED);
}

[[noreturn]] void rethrow_exception(std::__exception_ptr::exception_ptr* exception_ptr) {
    void* exception = exception_ptr->_M_get();
    __cxa_dependent_exception* dependent = (__cxa_dependent_exception*)__cxa_allocate_dependent_exception();
    dependent->primaryException = exception;
    __atomic_add_fetch((int*)((char*)exception - 128), 1, __ATOMIC_ACQ_REL);
    dependent->unexpectedHandler = (void*)get_unexpected();
    dependent->terminateHandler = (void*)get_terminate();
    dependent->exceptionType = 0x474E5543432B2B01LL;
    dependent->exceptionDestructor = (void*)__cxa_free_dependent_exception;
    _Unwind_RaiseException((_Unwind_Exception*)&dependent->unwindHeader);
    __cxa_begin_catch(&dependent->unwindHeader);
    std::terminate();
}

unsigned long long set_new_handler(void (*handler)(void)) {
    unsigned long long old = __atomic_load_n(&__new_handler_storage, __ATOMIC_RELAXED);
    __atomic_store_n(&__new_handler_storage, (unsigned long long)handler, __ATOMIC_RELEASE);
    return old;
}

unsigned long long set_terminate(void (*handler)(void)) {
    unsigned long long old = __atomic_load_n(&__cxxabiv1::__terminate_handler, __ATOMIC_RELAXED);
    __atomic_store_n(&__cxxabiv1::__terminate_handler, (unsigned long long)handler, __ATOMIC_RELEASE);
    return old;
}

unsigned long long set_unexpected(void (*handler)(void)) {
    unsigned long long old = __atomic_load_n(&__cxxabiv1::__unexpected_handler, __ATOMIC_RELAXED);
    __atomic_store_n(&__cxxabiv1::__unexpected_handler, (unsigned long long)handler, __ATOMIC_RELEASE);
    return old;
}

[[noreturn]] void terminate() {
    void (*handler)(void) = (void (*)(void))get_terminate();
    __cxxabiv1::__terminate(handler);
}

bool uncaught_exception() {
    return __cxa_get_globals()[2] != 0;
}

[[noreturn]] void unexpected() {
    void (*handler)(void) = (void (*)(void))get_unexpected();
    __cxxabiv1::__unexpected(handler);
}

}