#include "__gnu_cxx.h"

void __noreturn __gnu_cxx::__verbose_terminate_handler(void)
{
  struct type_info *type_info;
  const char *charPtr;
  int intVal;
  const char *charPtr2;
  char *charPtr3;
  FILE *fILE;
  size_t size_t;
  const char *charPtr4;
  int intVal2;

  if ( byte_125D338 )
  {
    size_t = 29;
    charPtr4 = "terminate called recursively\n";
  }
  else
  {
    byte_125D338 = 1;
    type_info = _cxa_current_exception_type();
    if ( type_info )
    {
      charPtr = (const char *)*((_QWORD *)type_info + 1);
      intVal = *(unsigned __int8 *)charPtr;
      intVal2 = -1;
      if ( intVal == 42 )
        charPtr2 = charPtr + 1;
      else
        charPtr2 = charPtr;
      charPtr3 = _cxa_demangle(charPtr2, 0, 0, &intVal2);
      fILE = (FILE *)((char *)&_sF + 304);
      fwrite("terminate called after throwing an instance of '", 1u, 0x30u, (FILE *)((char *)&_sF + 304));
      if ( intVal2 )
        fputs(charPtr2, fILE);
      else
        fputs(charPtr3, fILE);
      fwrite("'\n", 1u, 2u, (FILE *)((char *)&_sF + 304));
      if ( intVal2 )
        _cxa_rethrow();
      free(charPtr3);
      _cxa_rethrow();
    }
    charPtr4 = "terminate called without an active exception\n";
    size_t = 45;
  }
  fwrite(charPtr4, 1u, size_t, (FILE *)((char *)&_sF + 304));
  abort();
}
