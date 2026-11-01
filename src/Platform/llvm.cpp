#include "Platform/llvm.h"

__int64 llvm::ConvertCodePointToUTF8(__int64 *int64Ptr, char **charPtr)
{
  int intVal;
  unsigned int returnVal;
  int intVal2;
  _BYTE v8[8];
  unsigned int *intPtr;
  __int64 int64Val;

  intPtr = (unsigned int *)&intVal2;
  intVal2 = (int)this;
  int64Val = *int64Ptr;
  intVal = ConvertUTF32toUTF8(&intPtr, (unsigned __int64)v8, &int64Val, int64Val + 4, 0);
  returnVal = 0;
  if ( !intVal )
  {
    returnVal = 1;
    *int64Ptr = int64Val;
  }
  return returnVal;
}


__int64 llvm::ConvertUTF8toWide(int intVal, const void **voidPtr, void **voidPtr2, _QWORD *qwordPtr)
{
  __int64 returnVal;
  _WORD *wordPtr;
  __int64 int64Val;
  int intVal2;
  _WORD *wordPtr2;
  __int64 int64Val2;
  _WORD *wordPtr3;
  __int64 int64Val3;
  _QWORD *qwordPtr2;
  _WORD *wordPtr32;

  if ( intVal != 1 )
  {
    if ( intVal == 2 )
    {
      wordPtr3 = *voidPtr2;
      qwordPtr2 = *voidPtr;
      int64Val3 = *(qwordPtr2 - 3);
      wordPtr32 = wordPtr3;
      intVal2 = ConvertUTF8toUTF16(
              (unsigned __int64 *)&qwordPtr2,
              (unsigned __int64)qwordPtr2 + int64Val3,
              &wordPtr32,
              (unsigned __int64)&wordPtr3[2 * int64Val3],
              0);
      if ( !intVal2 )
      {
LABEL_5:
        *voidPtr2 = wordPtr32;
        return intVal2 == 0;
      }
    }
    else
    {
      returnVal = 1;
      if ( intVal != 4 )
        return returnVal;
      wordPtr = *voidPtr2;
      qwordPtr2 = *voidPtr;
      int64Val = *(qwordPtr2 - 3);
      wordPtr32 = wordPtr;
      intVal2 = ConvertUTF8toUTF32(
              (unsigned __int64 *)&qwordPtr2,
              (unsigned __int64)qwordPtr2 + int64Val,
              (unsigned __int64 *)&wordPtr32,
              (unsigned __int64)&wordPtr[8 * int64Val],
              0);
      if ( !intVal2 )
        goto LABEL_5;
    }
    *qwordPtr = qwordPtr2;
    return intVal2 == 0;
  }
  wordPtr2 = *voidPtr;
  int64Val2 = *((_QWORD *)wordPtr2 - 3);
  wordPtr32 = wordPtr2;
  returnVal = (unsigned __int8)isLegalUTF8String((const unsigned __int8 **)&wordPtr32, (const unsigned __int8 *)wordPtr2 + int64Val2);
  if ( (_BYTE)returnVal )
  {
    memcpy(*voidPtr2, *voidPtr, *((_QWORD *)*voidPtr - 3));
    returnVal = 1;
    *voidPtr2 = (char *)*voidPtr2 + *((_QWORD *)*voidPtr - 3);
  }
  else
  {
    *qwordPtr = wordPtr32;
  }
  return returnVal;
}


__int64 llvm::convertUTF16ToUTF8String(char **charPtr, int **intPtr)
{
  unsigned int returnVal;
  char *charPtr2;
  __int64 int64Val;
  int intVal;
  unsigned __int64 int64Val2;
  int *intPtr2;
  unsigned __int64 int64Val3;
  int *intPtr3;
  __int64 int64Val4;
  size_t size_t;
  char *charPtr3;
  __int64 int64Val5;
  __int64 int64Val6;
  char *charPtr4;
  char *charPtr22;
  int *intPtr22;
  void *p;
  __int64 int64Val7;
  __int64 int64Val8;

  returnVal = 1;
  charPtr2 = *charPtr;
  int64Val = *((_QWORD *)*charPtr - 3);
  if ( !int64Val )
    return returnVal;
  intVal = *(unsigned __int16 *)charPtr2;
  charPtr22 = charPtr2;
  int64Val2 = (unsigned __int64)&charPtr2[2 * int64Val];
  p = 0;
  int64Val7 = 0;
  int64Val8 = 0;
  if ( intVal == 65534 )
  {
    std::vector<unsigned short>::_M_range_insert<unsigned short const*>((__int64)&p, 0, charPtr2, &charPtr2[2 * int64Val]);
    charPtr3 = (char *)p;
    int64Val5 = 0;
    int64Val6 = (int64Val7 - (__int64)p) >> 1;
    if ( int64Val6 )
    {
      do
      {
        *(_WORD *)&charPtr3[2 * int64Val5] = __rev16(*(unsigned __int16 *)&charPtr3[2 * int64Val5]);
        ++int64Val5;
      }
      while ( int64Val5 != int64Val6 );
    }
    charPtr4 = *charPtr;
    int64Val2 = (unsigned __int64)&charPtr3[2 * int64Val6];
    charPtr22 = charPtr3;
    charPtr2 = charPtr3;
    intVal = *(unsigned __int16 *)charPtr3;
    int64Val = *((_QWORD *)charPtr4 - 3);
  }
  if ( intVal == 65279 )
  {
    charPtr22 = charPtr2 + 2;
    sub_D17E50(intPtr, 4 * int64Val + 1, 0);
    intPtr2 = *intPtr;
    if ( *(*intPtr - 2) < 0 )
      goto LABEL_6;
    goto LABEL_5;
  }
  sub_D17E50(intPtr, 4 * int64Val + 1, 0);
  intPtr2 = *intPtr;
  if ( (*(*intPtr - 2) & 0x80000000) == 0 )
  {
LABEL_5:
    sub_D176A8((__int64 *)intPtr);
    intPtr2 = *intPtr;
  }
LABEL_6:
  int64Val3 = (unsigned __int64)intPtr2 + *((_QWORD *)intPtr2 - 3);
  intPtr22 = intPtr2;
  if ( (unsigned int)ConvertUTF16toUTF8((unsigned __int64 *)&charPtr22, int64Val2, (__int64 *)&intPtr22, int64Val3, 0) )
  {
    returnVal = 0;
    sub_D174B8((__int64 *)intPtr, 0, *((_QWORD *)*intPtr - 3), 0);
  }
  else
  {
    intPtr3 = *intPtr;
    int64Val4 = (__int64)intPtr22;
    if ( (*(*intPtr - 2) & 0x80000000) == 0 )
    {
      sub_D176A8((__int64 *)intPtr);
      intPtr3 = *intPtr;
    }
    size_t = int64Val4 - (_QWORD)intPtr3;
    returnVal = 1;
    sub_D17E50(intPtr, size_t, 0);
  }
  if ( p )
    operator delete(p);
  return returnVal;
}


bool llvm::hasUTF16ByteOrderMark(const char *charPtr)
{
  int intVal;

  if ( (unsigned __int64)charPtr <= 1 )
    return 0;
  intVal = *(unsigned __int8 *)this;
  if ( intVal == 255 )
    return *((unsigned __int8 *)this + 1) == 254;
  if ( intVal == 254 )
    return *((unsigned __int8 *)this + 1) == 255;
  return 0;
}
