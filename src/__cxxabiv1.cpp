#include "__cxxabiv1.h"

void  __noreturn __cxxabiv1::__terminate(void (*a1)(void))
{
  a1();
  abort();
}


void  __noreturn __cxxabiv1::__unexpected(void (*a1)(void))
{
  a1();
  std::terminate();
}
