#include "_JNIEnv.h"

__int64 _JNIEnv::CallStaticBooleanMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  return (*(__int64 (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 944LL))(int64Val, int64Val2, int64Val3, va);
}


__int64 _JNIEnv::CallStaticDoubleMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  (*(void (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 1112LL))(int64Val, int64Val2, int64Val3, va);
  return _stack_chk_guard;
}


__int64 _JNIEnv::CallStaticFloatMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  (*(void (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 1088LL))(int64Val, int64Val2, int64Val3, va);
  return _stack_chk_guard;
}


__int64 _JNIEnv::CallStaticIntMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  return (*(__int64 (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 1040LL))(
           int64Val,
           int64Val2,
           int64Val3,
           va);
}


__int64 _JNIEnv::CallStaticLongMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  return (*(__int64 (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 1064LL))(
           int64Val,
           int64Val2,
           int64Val3,
           va);
}


__int64 _JNIEnv::CallStaticObjectMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  return (*(__int64 (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 920LL))(int64Val, int64Val2, int64Val3, va);
}


__int64 _JNIEnv::CallStaticVoidMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...)
{
  gcc_va_list va;
  gcc_va_list va1;

  va_start(va, int64Val3);
  va_start(va1, int64Val3);
  (*(void (__fastcall **)(__int64, __int64, __int64, __va_list_tag *))(*(_QWORD *)int64Val + 1136LL))(int64Val, int64Val2, int64Val3, va);
  return _stack_chk_guard;
}
