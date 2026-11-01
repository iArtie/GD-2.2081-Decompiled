#include "cocos2d.h"

cocos2d::CCAffineTransform * cocos2d::CCAffineTransformMakeIdentity(cocos2d::CCAffineTransform *__return_ptr retstr)
{
  cocos2d::__CCAffineTransformMake(retstr, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
  return retstr;
}


void cocos2d::CCLuaLog(const char *p0)
{
  __android_log_print(3, "cocos2d-x debug info", "%s", p0);
}


cocos2d::CCPoint cocos2d::CCPointFromString(const char *p0)
{
  cocos2d::CCPoint *point;
  cocos2d::CCPoint *point2;
  float value;
  float value2;
  double value3;
  float value4;
  float value32;
  const char **charPtr;
  __int64 *int64Ptr;
  const char *charPtr2;
  char *charPtr3;
  unsigned int *intPtr;
  int intVal;
  const char **charPtr4;
  const char *charPtr5;
  char *charPtr6;
  unsigned int *intPtr2;
  int intVal2;
  cocos2d::CCPoint point3;
  const char **charPtr7;
  void *p;
  __int64 int64Val;
  cocos2d::CCPoint returnVal;

  point2 = point;
  cocos2d::CCPoint::CCPoint(point, &stru_1245070);
  charPtr7 = 0;
  p = 0;
  int64Val = 0;
  if ( (unsigned __int8)sub_AD18EC((char *)p0, (__int64 **)&charPtr7) )
  {
    value3 = atof(*charPtr7);
    value4 = atof(charPtr7[1]);
    value32 = value3;
    cocos2d::CCPoint::CCPoint(&point3, value32, value4);
    LODWORD(value) = cocos2d::CCPoint::operator=(point2, &point3).n128_u32[0];
    charPtr = charPtr7;
    int64Ptr = (__int64 *)p;
    if ( charPtr7 == p )
      goto LABEL_12;
    do
    {
      charPtr2 = *charPtr;
      charPtr3 = (char *)(*charPtr - 24);
      if ( charPtr3 != (char *)&qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr = (unsigned int *)(charPtr2 - 8);
          do
            intVal = __ldaxr(intPtr);
          while ( __stlxr(intVal - 1, intPtr) );
        }
        else
        {
          intVal = *((_DWORD *)charPtr2 - 2);
          *((_DWORD *)charPtr2 - 2) = intVal - 1;
        }
        if ( intVal <= 0 )
          operator delete(charPtr3);
      }
      ++charPtr;
    }
    while ( int64Ptr != (__int64 *)charPtr );
    goto LABEL_11;
  }
  charPtr4 = charPtr7;
  int64Ptr = (__int64 *)p;
  if ( charPtr7 != p )
  {
    do
    {
      charPtr5 = *charPtr4;
      charPtr6 = (char *)(*charPtr4 - 24);
      if ( charPtr6 != (char *)&qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr2 = (unsigned int *)(charPtr5 - 8);
          do
            intVal2 = __ldaxr(intPtr2);
          while ( __stlxr(intVal2 - 1, intPtr2) );
        }
        else
        {
          intVal2 = *((_DWORD *)charPtr5 - 2);
          *((_DWORD *)charPtr5 - 2) = intVal2 - 1;
        }
        if ( intVal2 <= 0 )
          operator delete(charPtr6);
      }
      ++charPtr4;
    }
    while ( int64Ptr != (__int64 *)charPtr4 );
LABEL_11:
    int64Ptr = (__int64 *)charPtr7;
  }
LABEL_12:
  if ( int64Ptr )
    operator delete(int64Ptr);
  returnVal.y = value2;
  returnVal.x = value;
  return returnVal;
}


void cocos2d::CCProfilingBeginTimingBlock(const char *p0)
{
  cocos2d::CCProfiler *cCProfiler;
  cocos2d::CCDictionary *cCDictionary;
  cocos2d::CCProfilingTimer *cCProfilingTimer;
  char *charPtr;
  unsigned int *intPtr;
  int intVal;
  std::string str;

  cCProfiler = cocos2d::CCProfiler::sharedProfiler();
  cCDictionary = (cocos2d::CCDictionary *)cCProfiler[1].__vftable;
  sub_D153B8(&str, (char *)p0);
  cCProfilingTimer = (cocos2d::CCProfilingTimer *)cocos2d::CCDictionary::objectForKey(cCDictionary, &str);
  charPtr = (char *)str.m_data.m_data - 24;
  if ( (__int64 *)str.m_data.m_data - 3 == &qword_124B6B0 )
    goto LABEL_2;
  if ( &pthread_create )
  {
    intPtr = (unsigned int *)((char *)str.m_data.m_data - 8);
    do
      intVal = __ldaxr(intPtr);
    while ( __stlxr(intVal - 1, intPtr) );
  }
  else
  {
    intVal = *((_DWORD *)str.m_data.m_data - 2);
    *((_DWORD *)str.m_data.m_data - 2) = intVal - 1;
  }
  if ( intVal > 0 )
  {
LABEL_2:
    if ( !cCProfilingTimer )
LABEL_9:
      cCProfilingTimer = cocos2d::CCProfiler::createAndAddTimerWithName(cCProfiler, p0);
  }
  else
  {
    operator delete(charPtr);
    if ( !cCProfilingTimer )
      goto LABEL_9;
  }
  ++cCProfilingTimer[1].m_uID;
  gettimeofday((struct timeval *)&cCProfilingTimer[1].m_unknown2, 0);
}


void cocos2d::CCProfilingEndTimingBlock(const char *p0)
{
  cocos2d::CCDictionary *cCDictionary;
  __int64 int64Val;
  char *charPtr;
  int intVal;
  __int64 int64Val2;
  int intVal2;
  int intVal22;
  int intVal3;
  __int64 int64Val3;
  __int64 int64Val4;
  unsigned int *intPtr;
  int intVal4;
  std::string str;
  struct timeval tv;

  gettimeofday(&tv, 0);
  cCDictionary = (cocos2d::CCDictionary *)cocos2d::CCProfiler::sharedProfiler()[1].__vftable;
  sub_D153B8(&str, (char *)p0);
  int64Val = cocos2d::CCDictionary::objectForKey(cCDictionary, &str);
  charPtr = (char *)str.m_data.m_data - 24;
  if ( (__int64 *)str.m_data.m_data - 3 != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr = (unsigned int *)((char *)str.m_data.m_data - 8);
      do
        intVal4 = __ldaxr(intPtr);
      while ( __stlxr(intVal4 - 1, intPtr) );
    }
    else
    {
      intVal4 = *((_DWORD *)str.m_data.m_data - 2);
      *((_DWORD *)str.m_data.m_data - 2) = intVal4 - 1;
    }
    if ( intVal4 <= 0 )
      operator delete(charPtr);
  }
  intVal = *(_DWORD *)(int64Val + 92);
  int64Val2 = *(int *)(int64Val + 64);
  intVal2 = LODWORD(tv.tv_usec) - *(_DWORD *)(int64Val + 104) + 1000000 * (LODWORD(tv.tv_sec) - *(_QWORD *)(int64Val + 96));
  intVal22 = *(_DWORD *)(int64Val + 88);
  intVal3 = intVal2 + *(_DWORD *)(int64Val + 68);
  if ( intVal2 >= intVal )
    intVal = LODWORD(tv.tv_usec) - *(_DWORD *)(int64Val + 104) + 1000000 * (LODWORD(tv.tv_sec) - *(_QWORD *)(int64Val + 96));
  int64Val3 = *(_QWORD *)(int64Val + 80) + intVal2;
  *(_QWORD *)(int64Val + 80) = int64Val3;
  int64Val4 = int64Val3 / int64Val2;
  if ( intVal2 <= intVal22 )
    intVal22 = intVal2;
  *(_DWORD *)(int64Val + 72) = int64Val4;
  *(_DWORD *)(int64Val + 88) = intVal22;
  *(_DWORD *)(int64Val + 92) = intVal;
  *(_DWORD *)(int64Val + 68) = (int)(float)((float)intVal3 * 0.5);
}


void cocos2d::CCProfilingResetTimingBlock(const char *p0)
{
  cocos2d::CCDictionary *cCDictionary;
  cocos2d::CCProfilingTimer *cCProfilingTimer;
  char *charPtr;
  unsigned int *intPtr;
  int intVal;
  std::string str;

  cCDictionary = (cocos2d::CCDictionary *)cocos2d::CCProfiler::sharedProfiler()[1].__vftable;
  sub_D153B8(&str, (char *)p0);
  cCProfilingTimer = (cocos2d::CCProfilingTimer *)cocos2d::CCDictionary::objectForKey(cCDictionary, &str);
  charPtr = (char *)str.m_data.m_data - 24;
  if ( (__int64 *)str.m_data.m_data - 3 != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr = (unsigned int *)((char *)str.m_data.m_data - 8);
      do
        intVal = __ldaxr(intPtr);
      while ( __stlxr(intVal - 1, intPtr) );
    }
    else
    {
      intVal = *((_DWORD *)str.m_data.m_data - 2);
      *((_DWORD *)str.m_data.m_data - 2) = intVal - 1;
    }
    if ( intVal <= 0 )
      operator delete(charPtr);
  }
  cocos2d::CCProfilingTimer::reset(cCProfilingTimer);
}


cocos2d::CCRect cocos2d::CCRectApplyAffineTransform(const cocos2d::CCRect *p0, const cocos2d::CCAffineTransform *p1)
{
  cocos2d::CCRect *rect;
  cocos2d::CCRect *rect2;
  float MinY;
  float MinX;
  float MaxX;
  float MaxY;
  float value;
  float value2;
  float value3;
  float value4;
  float value5;
  float value6;
  float value7;
  float value8;
  float value9;
  float value10;
  float value11;
  float value12;
  float value13;
  float value14;
  float value15;
  float value16;
  float value17;
  float value18;
  float value19;
  float value20;
  cocos2d::CCPoint point;
  cocos2d::CCRect returnVal;

  rect2 = rect;
  MinY = cocos2d::CCRect::getMinY((cocos2d::CCRect *)p0);
  MinX = cocos2d::CCRect::getMinX((cocos2d::CCRect *)p0);
  MaxX = cocos2d::CCRect::getMaxX((cocos2d::CCRect *)p0);
  MaxY = cocos2d::CCRect::getMaxY((cocos2d::CCRect *)p0);
  cocos2d::CCPoint::CCPoint(&point, MinX, MinY);
  cocos2d::__CCPointApplyAffineTransform(&point, p1);
  cocos2d::CCPoint::CCPoint(&point, MaxX, MinY);
  cocos2d::__CCPointApplyAffineTransform(&point, p1);
  cocos2d::CCPoint::CCPoint(&point, MinX, MaxY);
  cocos2d::__CCPointApplyAffineTransform(&point, p1);
  cocos2d::CCPoint::CCPoint(&point, MaxX, MaxY);
  cocos2d::__CCPointApplyAffineTransform(&point, p1);
  value = value15;
  value2 = value15;
  if ( value15 >= value13 )
    value2 = value13;
  value3 = value19;
  value4 = value19;
  if ( value19 >= value17 )
    value4 = value17;
  if ( value4 >= value2 )
    value4 = value2;
  if ( value15 <= value13 )
    value = value13;
  if ( value19 <= value17 )
    value3 = value17;
  if ( value >= value3 )
    value3 = value;
  value5 = value16;
  value6 = value16;
  if ( value16 >= value14 )
    value6 = value14;
  value7 = value20;
  value8 = value20;
  if ( value20 >= value18 )
    value8 = value18;
  if ( value8 >= value6 )
    value8 = value6;
  if ( value16 <= value14 )
    value5 = value14;
  if ( value20 <= value18 )
    value7 = value18;
  if ( value5 >= value7 )
    value7 = value5;
  cocos2d::CCRect::CCRect(rect2, value4, value8, value3 - value4, value7 - value8);
  returnVal.size.height = value12;
  returnVal.size.width = value11;
  returnVal.origin.y = value10;
  returnVal.origin.x = value9;
  return returnVal;
}


cocos2d::CCRect cocos2d::CCRectFromString(const char *p0)
{
  cocos2d::CCRect *rect;
  cocos2d::CCRect *rect2;
  float value;
  float value2;
  float value3;
  float value4;
  int intVal;
  int intVal2;
  int intVal3;
  int intVal4;
  unsigned __int64 int64Val;
  unsigned __int64 int64Val2;
  char *charPtr;
  int intVal5;
  int intVal6;
  int intVal7;
  __int64 intVal62;
  double value5;
  double value6;
  double value7;
  float value8;
  float value52;
  float value62;
  float value72;
  __int64 *int64Ptr;
  __int64 *int64Ptr2;
  __int64 int64Val3;
  __int64 *int64Ptr3;
  unsigned int *intPtr;
  int intVal8;
  __int64 *int64Ptr4;
  char *charPtr2;
  char *charPtr3;
  unsigned int *intPtr2;
  int intVal9;
  __int64 int64Val4;
  void *voidPtr;
  unsigned int *intPtr3;
  int intVal10;
  __int64 *int64Ptr5;
  __int64 *int64Ptr6;
  __int64 int64Val5;
  __int64 *int64Ptr7;
  unsigned int *intPtr4;
  int intVal11;
  unsigned int *intPtr5;
  int intVal12;
  __int64 *int64Ptr8;
  __int64 *int64Ptr9;
  __int64 int64Val6;
  __int64 *int64Ptr10;
  unsigned int *intPtr6;
  int intVal13;
  char *charPtr4;
  char *charPtr5;
  unsigned int *intPtr7;
  __int64 *int64Ptr11;
  __int64 int64Val7;
  __int64 *int64Ptr12;
  unsigned int *intPtr8;
  int intVal14;
  unsigned int *intPtr9;
  int intVal15;
  unsigned int *intPtr10;
  int intVal16;
  unsigned int *intPtr11;
  int intVal17;
  __int64 int64Val8;
  char *charPtr6;
  char *charPtr7;
  cocos2d::CCRect rect3;
  void *p;
  void *voidPtr2;
  __int64 int64Val9;
  void *voidPtr3;
  void *voidPtr4;
  __int64 int64Val10;
  cocos2d::CCRect returnVal;

  rect2 = rect;
  cocos2d::CCRect::CCRect(rect, &stru_1245078);
  if ( !p0 )
    goto LABEL_115;
  sub_D153B8(&int64Val8, (char *)p0);
  intVal = sub_D16674(&int64Val8, 0x7Bu, 0);
  intVal2 = sub_D16674(&int64Val8, 0x7Du, 0);
  if ( intVal2 == -1
    || (intVal3 = sub_D16674(&int64Val8, 0x7Du, intVal2 + 1), intVal3 == -1)
    || (intVal4 = sub_D16674(&int64Val8, 0x7Du, intVal3 + 1), intVal4 == -1)
    || intVal == -1 )
  {
    int64Val4 = int64Val8;
    voidPtr = (void *)(int64Val8 - 24);
    if ( (__int64 *)(int64Val8 - 24) == &qword_124B6B0 )
      goto LABEL_115;
    goto LABEL_33;
  }
  int64Val = intVal + 1;
  int64Val2 = *(_QWORD *)(int64Val8 - 24);
  if ( int64Val > int64Val2 )
    goto LABEL_114;
  sub_D14F84(&voidPtr3, &int64Val8, int64Val, intVal4 - intVal - 1);
  sub_D163D4(&int64Val8, (__int64 *)&voidPtr3);
  charPtr = (char *)voidPtr3 - 24;
  if ( (char *)voidPtr3 - 24 != (char *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr5 = (unsigned int *)((char *)voidPtr3 - 8);
      do
        intVal12 = __ldaxr(intPtr5);
      while ( __stlxr(intVal12 - 1, intPtr5) );
    }
    else
    {
      intVal12 = *((_DWORD *)voidPtr3 - 2);
      *((_DWORD *)voidPtr3 - 2) = intVal12 - 1;
    }
    if ( intVal12 <= 0 )
      operator delete(charPtr);
  }
  intVal5 = sub_D16674(&int64Val8, 0x7Du, 0);
  if ( intVal5 == -1 || (intVal6 = sub_D16674(&int64Val8, 0x2Cu, intVal5), intVal7 = intVal6, intVal6 == -1) )
  {
LABEL_32:
    int64Val4 = int64Val8;
    voidPtr = (void *)(int64Val8 - 24);
    if ( (__int64 *)(int64Val8 - 24) == &qword_124B6B0 )
      goto LABEL_115;
LABEL_33:
    if ( &pthread_create )
    {
      intPtr3 = (unsigned int *)(int64Val4 - 8);
      do
        intVal10 = __ldaxr(intPtr3);
      while ( __stlxr(intVal10 - 1, intPtr3) );
LABEL_36:
      if ( intVal10 <= 0 )
        operator delete(voidPtr);
      goto LABEL_115;
    }
LABEL_54:
    intVal10 = *(_DWORD *)(int64Val4 - 8);
    *(_DWORD *)(int64Val4 - 8) = intVal10 - 1;
    goto LABEL_36;
  }
  intVal62 = intVal6;
  sub_D14F84(&charPtr6, &int64Val8, 0, intVal6);
  int64Val = intVal7 + 1;
  int64Val2 = *(_QWORD *)(int64Val8 - 24);
  if ( int64Val > int64Val2 )
LABEL_114:
    sub_D13C5C("%s: __pos (which is %zu) > this->size() (which is %zu)", "basic_string::substr", int64Val, int64Val2);
  sub_D14F84(&charPtr7, &int64Val8, int64Val, int64Val2 - intVal62);
  p = 0;
  voidPtr2 = 0;
  int64Val9 = 0;
  if ( !(unsigned __int8)sub_AD18EC(charPtr6, (__int64 **)&p) )
  {
LABEL_44:
    int64Ptr6 = (__int64 *)p;
    int64Ptr4 = (__int64 *)voidPtr2;
    if ( p != voidPtr2 )
    {
      do
      {
        int64Val5 = *int64Ptr6;
        int64Ptr7 = (__int64 *)(*int64Ptr6 - 24);
        if ( int64Ptr7 != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr4 = (unsigned int *)(int64Val5 - 8);
            do
              intVal11 = __ldaxr(intPtr4);
            while ( __stlxr(intVal11 - 1, intPtr4) );
          }
          else
          {
            intVal11 = *(_DWORD *)(int64Val5 - 8);
            *(_DWORD *)(int64Val5 - 8) = intVal11 - 1;
          }
          if ( intVal11 <= 0 )
            operator delete(int64Ptr7);
        }
        ++int64Ptr6;
      }
      while ( int64Ptr4 != int64Ptr6 );
      int64Ptr4 = (__int64 *)p;
    }
    if ( int64Ptr4 )
      operator delete(int64Ptr4);
    charPtr2 = charPtr7 - 24;
    if ( charPtr7 - 24 != (char *)&qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr9 = (unsigned int *)(charPtr7 - 8);
        do
          intVal15 = __ldaxr(intPtr9);
        while ( __stlxr(intVal15 - 1, intPtr9) );
      }
      else
      {
        intVal15 = *((_DWORD *)charPtr7 - 2);
        *((_DWORD *)charPtr7 - 2) = intVal15 - 1;
      }
      if ( intVal15 <= 0 )
        operator delete(charPtr2);
    }
    charPtr3 = charPtr6 - 24;
    if ( charPtr6 - 24 != (char *)&qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr2 = (unsigned int *)(charPtr6 - 8);
        do
          intVal9 = __ldaxr(intPtr2);
        while ( __stlxr(intVal9 - 1, intPtr2) );
      }
      else
      {
        intVal9 = *((_DWORD *)charPtr6 - 2);
        *((_DWORD *)charPtr6 - 2) = intVal9 - 1;
      }
      if ( intVal9 <= 0 )
        operator delete(charPtr3);
    }
    goto LABEL_32;
  }
  voidPtr3 = 0;
  voidPtr4 = 0;
  int64Val10 = 0;
  if ( !(unsigned __int8)sub_AD18EC(charPtr7, (__int64 **)&voidPtr3) )
  {
    int64Ptr11 = (__int64 *)voidPtr3;
    int64Ptr5 = (__int64 *)voidPtr4;
    if ( voidPtr3 != voidPtr4 )
    {
      do
      {
        int64Val7 = *int64Ptr11;
        int64Ptr12 = (__int64 *)(*int64Ptr11 - 24);
        if ( int64Ptr12 != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr8 = (unsigned int *)(int64Val7 - 8);
            do
              intVal14 = __ldaxr(intPtr8);
            while ( __stlxr(intVal14 - 1, intPtr8) );
          }
          else
          {
            intVal14 = *(_DWORD *)(int64Val7 - 8);
            *(_DWORD *)(int64Val7 - 8) = intVal14 - 1;
          }
          if ( intVal14 <= 0 )
            operator delete(int64Ptr12);
        }
        ++int64Ptr11;
      }
      while ( int64Ptr5 != int64Ptr11 );
      int64Ptr5 = (__int64 *)voidPtr3;
    }
    if ( int64Ptr5 )
      operator delete(int64Ptr5);
    goto LABEL_44;
  }
  value5 = atof(*(const char **)p);
  value6 = atof(*((const char **)p + 1));
  value7 = atof(*(const char **)voidPtr3);
  value8 = atof(*(const char **)&byte_8[(_QWORD)voidPtr3]);
  value52 = value5;
  value62 = value6;
  value72 = value7;
  cocos2d::CCRect::CCRect(&rect3, value52, value62, value72, value8);
  cocos2d::CCRect::operator=(rect2, (float *)&rect3.origin);
  int64Ptr = (__int64 *)voidPtr3;
  int64Ptr2 = (__int64 *)voidPtr4;
  if ( voidPtr3 != voidPtr4 )
  {
    do
    {
      int64Val3 = *int64Ptr;
      int64Ptr3 = (__int64 *)(*int64Ptr - 24);
      if ( int64Ptr3 != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr = (unsigned int *)(int64Val3 - 8);
          do
            intVal8 = __ldaxr(intPtr);
          while ( __stlxr(intVal8 - 1, intPtr) );
        }
        else
        {
          intVal8 = *(_DWORD *)(int64Val3 - 8);
          *(_DWORD *)(int64Val3 - 8) = intVal8 - 1;
        }
        if ( intVal8 <= 0 )
          operator delete(int64Ptr3);
      }
      ++int64Ptr;
    }
    while ( int64Ptr2 != int64Ptr );
    int64Ptr2 = (__int64 *)voidPtr3;
  }
  if ( int64Ptr2 )
    operator delete(int64Ptr2);
  int64Ptr8 = (__int64 *)p;
  int64Ptr9 = (__int64 *)voidPtr2;
  if ( p != voidPtr2 )
  {
    do
    {
      int64Val6 = *int64Ptr8;
      int64Ptr10 = (__int64 *)(*int64Ptr8 - 24);
      if ( int64Ptr10 != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr6 = (unsigned int *)(int64Val6 - 8);
          do
            intVal13 = __ldaxr(intPtr6);
          while ( __stlxr(intVal13 - 1, intPtr6) );
        }
        else
        {
          intVal13 = *(_DWORD *)(int64Val6 - 8);
          *(_DWORD *)(int64Val6 - 8) = intVal13 - 1;
        }
        if ( intVal13 <= 0 )
          operator delete(int64Ptr10);
      }
      ++int64Ptr8;
    }
    while ( int64Ptr9 != int64Ptr8 );
    int64Ptr9 = (__int64 *)p;
  }
  if ( int64Ptr9 )
    operator delete(int64Ptr9);
  charPtr4 = charPtr7 - 24;
  if ( charPtr7 - 24 != (char *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr11 = (unsigned int *)(charPtr7 - 8);
      do
        intVal17 = __ldaxr(intPtr11);
      while ( __stlxr(intVal17 - 1, intPtr11) );
    }
    else
    {
      intVal17 = *((_DWORD *)charPtr7 - 2);
      *((_DWORD *)charPtr7 - 2) = intVal17 - 1;
    }
    if ( intVal17 <= 0 )
      operator delete(charPtr4);
  }
  charPtr5 = charPtr6 - 24;
  if ( charPtr6 - 24 != (char *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr10 = (unsigned int *)(charPtr6 - 8);
      do
        intVal16 = __ldaxr(intPtr10);
      while ( __stlxr(intVal16 - 1, intPtr10) );
    }
    else
    {
      intVal16 = *((_DWORD *)charPtr6 - 2);
      *((_DWORD *)charPtr6 - 2) = intVal16 - 1;
    }
    if ( intVal16 <= 0 )
      operator delete(charPtr5);
  }
  int64Val4 = int64Val8;
  voidPtr = (void *)(int64Val8 - 24);
  if ( (__int64 *)(int64Val8 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr7 = (unsigned int *)(int64Val8 - 8);
      do
        intVal10 = __ldaxr(intPtr7);
      while ( __stlxr(intVal10 - 1, intPtr7) );
      goto LABEL_36;
    }
    goto LABEL_54;
  }
LABEL_115:
  returnVal.size.height = value4;
  returnVal.size.width = value3;
  returnVal.origin.y = value2;
  returnVal.origin.x = value;
  return returnVal;
}


cocos2d::CCSize cocos2d::CCSizeFromString(const char *p0)
{
  cocos2d::CCSize *size;
  cocos2d::CCSize *size2;
  float value;
  float value2;
  double value3;
  float value4;
  float value32;
  const char **charPtr;
  __int64 *int64Ptr;
  const char *charPtr2;
  char *charPtr3;
  unsigned int *intPtr;
  int intVal;
  const char **charPtr4;
  const char *charPtr5;
  char *charPtr6;
  unsigned int *intPtr2;
  int intVal2;
  cocos2d::CCSize size3;
  const char **charPtr7;
  void *p;
  __int64 int64Val;
  cocos2d::CCSize returnVal;

  size2 = size;
  cocos2d::CCSize::CCSize(size, &stru_1245068);
  charPtr7 = 0;
  p = 0;
  int64Val = 0;
  if ( (unsigned __int8)sub_AD18EC((char *)p0, (__int64 **)&charPtr7) )
  {
    value3 = atof(*charPtr7);
    value4 = atof(charPtr7[1]);
    value32 = value3;
    cocos2d::CCSize::CCSize(&size3, value32, value4);
    cocos2d::CCSize::operator=(size2);
    charPtr = charPtr7;
    int64Ptr = (__int64 *)p;
    if ( charPtr7 == p )
      goto LABEL_12;
    do
    {
      charPtr2 = *charPtr;
      charPtr3 = (char *)(*charPtr - 24);
      if ( charPtr3 != (char *)&qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr = (unsigned int *)(charPtr2 - 8);
          do
            intVal = __ldaxr(intPtr);
          while ( __stlxr(intVal - 1, intPtr) );
        }
        else
        {
          intVal = *((_DWORD *)charPtr2 - 2);
          *((_DWORD *)charPtr2 - 2) = intVal - 1;
        }
        if ( intVal <= 0 )
          operator delete(charPtr3);
      }
      ++charPtr;
    }
    while ( int64Ptr != (__int64 *)charPtr );
    goto LABEL_11;
  }
  charPtr4 = charPtr7;
  int64Ptr = (__int64 *)p;
  if ( charPtr7 != p )
  {
    do
    {
      charPtr5 = *charPtr4;
      charPtr6 = (char *)(*charPtr4 - 24);
      if ( charPtr6 != (char *)&qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr2 = (unsigned int *)(charPtr5 - 8);
          do
            intVal2 = __ldaxr(intPtr2);
          while ( __stlxr(intVal2 - 1, intPtr2) );
        }
        else
        {
          intVal2 = *((_DWORD *)charPtr5 - 2);
          *((_DWORD *)charPtr5 - 2) = intVal2 - 1;
        }
        if ( intVal2 <= 0 )
          operator delete(charPtr6);
      }
      ++charPtr4;
    }
    while ( int64Ptr != (__int64 *)charPtr4 );
LABEL_11:
    int64Ptr = (__int64 *)charPtr7;
  }
LABEL_12:
  if ( int64Ptr )
    operator delete(int64Ptr);
  returnVal.height = value2;
  returnVal.width = value;
  return returnVal;
}


__int64 cocos2d::CGAffineToGL(_DWORD *dwordPtr, __int64 int64Val)
{
  __int64 returnVal;

  *(float *)(int64Val + 60) = 1.0;
  *(_DWORD *)(int64Val + 56) = 0;
  *(_DWORD *)(int64Val + 44) = 0;
  *(_DWORD *)(int64Val + 36) = 0;
  *(_DWORD *)(int64Val + 32) = 0;
  *(_DWORD *)(int64Val + 28) = 0;
  *(_DWORD *)(int64Val + 24) = 0;
  *(_DWORD *)(int64Val + 12) = 0;
  *(_DWORD *)(int64Val + 8) = 0;
  *(float *)(int64Val + 40) = 1.0;
  *(_DWORD *)int64Val = *dwordPtr;
  *(_DWORD *)(int64Val + 16) = dwordPtr[2];
  *(_DWORD *)(int64Val + 48) = dwordPtr[4];
  *(_DWORD *)(int64Val + 4) = dwordPtr[1];
  *(_DWORD *)(int64Val + 20) = dwordPtr[3];
  returnVal = (unsigned int)dwordPtr[5];
  *(_DWORD *)(int64Val + 52) = returnVal;
  return returnVal;
}


cocos2d::CCBMFontConfiguration * cocos2d::FNTConfigLoadFile(const char *p0)
{
  cocos2d::CCDictionary *cCDictionary;
  cocos2d::CCBMFontConfiguration *returnVal;
  char *charPtr;
  unsigned int *intPtr;
  int intVal;
  int intVal2;
  char *charPtr2;
  unsigned int *intPtr2;
  int intVal3;
  std::string str;

  cCDictionary = (cocos2d::CCDictionary *)qword_12452D8;
  if ( !qword_12452D8 )
  {
    cCDictionary = (cocos2d::CCDictionary *)operator new(0x48u);
    cocos2d::CCDictionary::CCDictionary(cCDictionary);
    qword_12452D8 = (__int64)cCDictionary;
  }
  sub_D153B8(&str, (char *)p0);
  returnVal = (cocos2d::CCBMFontConfiguration *)cocos2d::CCDictionary::objectForKey(cCDictionary, &str);
  charPtr = (char *)str.m_data.m_data - 24;
  if ( (__int64 *)str.m_data.m_data - 3 == &qword_124B6B0 )
    goto LABEL_4;
  if ( &pthread_create )
  {
    intPtr = (unsigned int *)((char *)str.m_data.m_data - 8);
    do
      intVal = __ldaxr(intPtr);
    while ( __stlxr(intVal - 1, intPtr) );
  }
  else
  {
    intVal = *((_DWORD *)str.m_data.m_data - 2);
    *((_DWORD *)str.m_data.m_data - 2) = intVal - 1;
  }
  if ( intVal > 0 )
  {
LABEL_4:
    if ( returnVal )
      return returnVal;
  }
  else
  {
    operator delete(charPtr);
    if ( returnVal )
      return returnVal;
  }
  returnVal = cocos2d::CCBMFontConfiguration::create(p0);
  if ( returnVal )
  {
    intVal2 = qword_12452D8;
    sub_D153B8(&str, (char *)p0);
    cocos2d::CCDictionary::setObject(intVal2, returnVal);
    charPtr2 = (char *)str.m_data.m_data - 24;
    if ( (__int64 *)str.m_data.m_data - 3 != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr2 = (unsigned int *)((char *)str.m_data.m_data - 8);
        do
          intVal3 = __ldaxr(intPtr2);
        while ( __stlxr(intVal3 - 1, intPtr2) );
      }
      else
      {
        intVal3 = *((_DWORD *)str.m_data.m_data - 2);
        *((_DWORD *)str.m_data.m_data - 2) = intVal3 - 1;
      }
      if ( intVal3 <= 0 )
        operator delete(charPtr2);
    }
  }
  return returnVal;
}


void cocos2d::FNTConfigRemoveCache()
{
  if ( qword_12452D8 )
  {
    cocos2d::CCDictionary::removeAllObjects((cocos2d::CCDictionary *)qword_12452D8);
    if ( qword_12452D8 )
    {
      cocos2d::CCObject::release((cocos2d::CCObject *)qword_12452D8);
      qword_12452D8 = 0;
    }
  }
}


__int64 cocos2d::GLToCGAffine(_DWORD *dwordPtr, _DWORD *dwordPtr2)
{
  __int64 returnVal;

  *dwordPtr2 = *dwordPtr;
  dwordPtr2[2] = dwordPtr[4];
  dwordPtr2[4] = dwordPtr[12];
  dwordPtr2[1] = dwordPtr[1];
  dwordPtr2[3] = dwordPtr[5];
  returnVal = (unsigned int)dwordPtr[13];
  dwordPtr2[5] = returnVal;
  return returnVal;
}


cocos2d::CCSize cocos2d::__CCSizeApplyAffineTransform(const cocos2d::CCSize *p0, const cocos2d::CCAffineTransform *p1)
{
  cocos2d::CCSize *size;
  double height;
  double value;
  double b;
  double value2;
  float value3;
  cocos2d::CCSize *size2;
  cocos2d::CCSize returnVal;

  size2 = size;
  cocos2d::CCSize::CCSize(size);
  height = p0->height;
  value = p1->c * height + p1->a * p0->width;
  b = p1->b;
  value2 = height * p1->d;
  *(float *)&value = value;
  size2->width = *(float *)&value;
  value3 = value2 + b * p0->width;
  size2->height = value3;
  returnVal.height = *(float *)&b;
  returnVal.width = value3;
  return returnVal;
}


__int64 cocos2d::_base64Encode(__int64 this, const unsigned __int8 *int8Ptr, __int64 int64Val, char *charPtr)
{
  __int64 int64Val2;
  unsigned int intVal;
  unsigned __int64 int64Val3;
  unsigned int intVal2;
  unsigned __int64 int64Val4;
  __int64 int64Val5;
  unsigned int intVal3;
  __int64 int64Val6;
  __int64 int64Val7;
  __int64 int64Val8;
  __int64 int64Val9;
  __int64 int64Val10;
  unsigned int intVal4;
  __int64 int64Val11;
  unsigned int intVal5;
  unsigned int intVal6;

  if ( !(_DWORD)int8Ptr )
  {
    int64Val11 = 0;
    goto LABEL_15;
  }
  int64Val2 = 0;
  intVal = 0;
  LODWORD(int64Val3) = 0;
  intVal2 = 0;
  do
  {
    while ( 1 )
    {
      ++intVal2;
      int64Val4 = (unsigned int)int64Val3 | *(unsigned __int8 *)(this + int64Val2);
      LODWORD(int64Val3) = (_DWORD)int64Val4 << 8;
      if ( intVal2 == 3 )
        break;
LABEL_3:
      if ( (unsigned int)int8Ptr <= (unsigned int)++int64Val2 )
        goto LABEL_7;
    }
    int64Val5 = int64Val4 & 0x3F;
    intVal3 = intVal + 3;
    int64Val6 = (int64Val4 >> 18) & 0x3F;
    int64Val7 = (int64Val4 >> 12) & 0x3F;
    int64Val8 = (int64Val4 >> 6) & 0x3F;
    int64Val9 = intVal + 1;
    int64Val10 = intVal + 2;
    intVal4 = intVal + 3;
    LODWORD(int64Val3) = 0;
    intVal2 = 0;
    if ( (_BYTE)charPtr )
    {
      *(_BYTE *)(int64Val + intVal) = cocos2d::urlAlphabet[int64Val6];
      intVal += 4;
      *(_BYTE *)(int64Val + int64Val9) = cocos2d::urlAlphabet[int64Val7];
      *(_BYTE *)(int64Val + int64Val10) = cocos2d::urlAlphabet[int64Val8];
      *(_BYTE *)(int64Val + intVal3) = cocos2d::urlAlphabet[int64Val5];
      goto LABEL_3;
    }
    ++int64Val2;
    *(_BYTE *)(int64Val + intVal) = cocos2d::alphabet[int64Val6];
    intVal += 4;
    *(_BYTE *)(int64Val + int64Val9) = cocos2d::alphabet[int64Val7];
    *(_BYTE *)(int64Val + int64Val10) = cocos2d::alphabet[int64Val8];
    *(_BYTE *)(int64Val + intVal4) = cocos2d::alphabet[int64Val5];
  }
  while ( (unsigned int)int8Ptr > (unsigned int)int64Val2 );
LABEL_7:
  int64Val11 = intVal;
  if ( intVal2 )
  {
    if ( intVal2 == 1 )
      int64Val3 = (unsigned int)((_DWORD)int64Val3 << 8);
    else
      int64Val3 = (unsigned int)int64Val3;
    if ( (_BYTE)charPtr )
    {
      intVal5 = intVal + 2;
      *(_BYTE *)(int64Val + intVal) = cocos2d::urlAlphabet[(int64Val3 >> 18) & 0x3F];
      *(_BYTE *)(int64Val + intVal + 1) = cocos2d::urlAlphabet[(int64Val3 >> 12) & 0x3F];
      if ( intVal2 > 1 )
      {
        intVal6 = intVal + 3;
        *(_BYTE *)(int64Val + intVal5) = cocos2d::urlAlphabet[(int64Val3 >> 6) & 0x3F];
        goto LABEL_14;
      }
LABEL_19:
      intVal6 = intVal5 + 1;
      *(_BYTE *)(int64Val + intVal5) = 61;
    }
    else
    {
      intVal5 = intVal + 2;
      *(_BYTE *)(int64Val + intVal) = cocos2d::alphabet[(int64Val3 >> 18) & 0x3F];
      *(_BYTE *)(int64Val + intVal + 1) = cocos2d::alphabet[(int64Val3 >> 12) & 0x3F];
      if ( intVal2 <= 1 )
        goto LABEL_19;
      intVal6 = intVal + 3;
      *(_BYTE *)(int64Val + intVal5) = cocos2d::alphabet[(int64Val3 >> 6) & 0x3F];
    }
LABEL_14:
    this = 61;
    int64Val11 = intVal6 + 1;
    *(_BYTE *)(int64Val + intVal6) = 61;
  }
LABEL_15:
  *(_BYTE *)(int64Val + int64Val11) = 0;
  return this;
}


__int64 cocos2d::call_zopen64(__int64 int64Val)
{
  if ( *(_QWORD *)int64Val )
    return (*(__int64 (__fastcall **)(_QWORD))int64Val)(*(_QWORD *)(int64Val + 56));
  else
    return (*(__int64 (__fastcall **)(_QWORD))(int64Val + 64))(*(_QWORD *)(int64Val + 56));
}


__int64 cocos2d::call_zseek64(__int64 int64Val)
{
  __int64 (__fastcall *v1)(_QWORD);

  v1 = *(__int64 (__fastcall **)(_QWORD))(int64Val + 32);
  if ( v1 )
    return v1(*(_QWORD *)(int64Val + 56));
  else
    return (*(__int64 (__fastcall **)(_QWORD))(int64Val + 80))(*(_QWORD *)(int64Val + 56));
}


__int64 cocos2d::call_ztell64(__int64 int64Val)
{
  if ( *(_QWORD *)(int64Val + 32) )
    return (*(__int64 (__fastcall **)(_QWORD))(int64Val + 24))(*(_QWORD *)(int64Val + 56));
  else
    return (*(__int64 (__fastcall **)(_QWORD))(int64Val + 72))(*(_QWORD *)(int64Val + 56));
}


__int64 cocos2d::ccArrayAppendArray(unsigned int *intPtr, unsigned int *intPtr2)
{
  __int64 returnVal;
  unsigned int i;
  cocos2d::CCObject *obj;

  returnVal = *intPtr2;
  for ( i = 0; (unsigned int)returnVal > i; returnVal = *intPtr2 )
  {
    obj = *(cocos2d::CCObject **)(*((_QWORD *)intPtr2 + 2) + 8LL * i++);
    cocos2d::ccArrayAppendObject(intPtr, obj);
  }
  return returnVal;
}


__int64 cocos2d::ccArrayAppendArrayWithResize(unsigned int *intPtr, int *intPtr2)
{
  cocos2d::ccArrayEnsureExtraCapacity(intPtr, *intPtr2);
  return cocos2d::ccArrayAppendArray(intPtr, (unsigned int *)intPtr2);
}


__int64 cocos2d::ccArrayAppendObject(unsigned int *intPtr, cocos2d::CCObject *this)
{
  __int64 returnVal;

  cocos2d::CCObject::retain(this);
  returnVal = *intPtr;
  *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * (unsigned int)returnVal) = this;
  *intPtr = returnVal + 1;
  return returnVal;
}


__int64 cocos2d::ccArrayAppendObjectBatch(unsigned int *intPtr, cocos2d::CCObject *this)
{
  __int64 returnVal;

  cocos2d::CCObject::retain(this);
  returnVal = *intPtr;
  *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * (unsigned int)returnVal) = this;
  this->m_unknown2 = returnVal;
  *intPtr = returnVal + 1;
  return returnVal;
}


__int64 cocos2d::ccArrayAppendObjectChild(unsigned int *intPtr, cocos2d::CCObject *obj)
{
  __int64 returnVal;

  cocos2d::ccArrayUpdateChildIndexes((__int64)intPtr);
  cocos2d::CCObject::retain(obj);
  returnVal = *intPtr;
  *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * (unsigned int)returnVal) = obj;
  obj->m_uUnknown = returnVal;
  *intPtr = returnVal + 1;
  return returnVal;
}


__int64 cocos2d::ccArrayAppendObjectNew(unsigned int *intPtr, cocos2d::CCObject *this)
{
  __int64 returnVal;

  cocos2d::CCObject::retain(this);
  returnVal = *intPtr;
  *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * (unsigned int)returnVal) = this;
  this->m_uIndexInArray = returnVal;
  *intPtr = returnVal + 1;
  return returnVal;
}


__int64 cocos2d::ccArrayAppendObjectWithResize(cocos2d::CCObject *obj)
{
  ccArrayEnsureExtraCapacity(1);
  return cocos2d::ccArrayAppendObject((unsigned int *)this, obj);
}


__int64 cocos2d::ccArrayAppendObjectWithResizeBatch(unsigned int *intPtr, cocos2d::CCObject *obj)
{
  cocos2d::ccArrayEnsureExtraCapacity(intPtr, 1);
  return cocos2d::ccArrayAppendObjectBatch(intPtr, obj);
}


__int64 cocos2d::ccArrayAppendObjectWithResizeChild(cocos2d::CCObject *obj)
{
  ccArrayEnsureExtraCapacity(1);
  return cocos2d::ccArrayAppendObjectChild((unsigned int *)this, obj);
}


__int64 cocos2d::ccArrayAppendObjectWithResizeNew(unsigned int *intPtr, cocos2d::CCObject *obj)
{
  cocos2d::ccArrayEnsureExtraCapacity(intPtr, 1);
  return cocos2d::ccArrayAppendObjectNew(intPtr, obj);
}


bool cocos2d::ccArrayContainsObject(int *intPtr, __int64 int64Val)
{
  return (unsigned int)cocos2d::ccArrayGetIndexOfObject(intPtr, int64Val) != -1;
}


void * cocos2d::ccArrayDoubleCapacity(__int64 int64Val)
{
  int intVal;
  void *voidPtr;
  __int64 int64Val2;
  void *returnVal;

  intVal = *(_DWORD *)(int64Val + 4);
  voidPtr = *(void **)(int64Val + 16);
  int64Val2 = (unsigned int)(2 * intVal);
  *(_DWORD *)(int64Val + 4) = int64Val2;
  returnVal = realloc(voidPtr, 8 * int64Val2);
  *(_QWORD *)(int64Val + 16) = returnVal;
  return returnVal;
}


__int64 cocos2d::ccArrayEnsureExtraCapacity(_DWORD *dwordPtr, int intVal)
{
  __int64 returnVal;

  while ( 1 )
  {
    returnVal = (unsigned int)(intVal + *dwordPtr);
    if ( dwordPtr[1] >= (unsigned int)returnVal )
      break;
    cocos2d::ccArrayDoubleCapacity((__int64)dwordPtr);
  }
  return returnVal;
}


__int64 cocos2d::ccArrayFastRemoveObject(int *intPtr, __int64 int64Val)
{
  __int64 returnVal;

  returnVal = cocos2d::ccArrayGetIndexOfObject(intPtr, int64Val);
  if ( (_DWORD)returnVal != -1 )
    return cocos2d::ccArrayFastRemoveObjectAtIndex((__int64)intPtr, returnVal);
  return returnVal;
}


__int64 cocos2d::ccArrayFastRemoveObjectAtIndex(__int64 int64Val, unsigned int intVal)
{
  __int64 int64Val2;
  __int64 int64Val3;
  cocos2d::CCObject *obj;
  _QWORD *qwordPtr;
  unsigned int intVal2;
  __int64 returnVal;

  int64Val2 = *(_QWORD *)(int64Val + 16);
  int64Val3 = 8LL * intVal;
  obj = *(cocos2d::CCObject **)(int64Val2 + int64Val3);
  qwordPtr = (_QWORD *)(int64Val2 + int64Val3);
  if ( obj )
  {
    cocos2d::CCObject::release(obj);
    int64Val2 = *(_QWORD *)(int64Val + 16);
    qwordPtr = (_QWORD *)(int64Val2 + int64Val3);
  }
  intVal2 = *(_DWORD *)int64Val - 1;
  *(_DWORD *)int64Val = intVal2;
  returnVal = *(_QWORD *)(int64Val2 + 8LL * intVal2);
  *qwordPtr = returnVal;
  return returnVal;
}


cocos2d::CCObject * cocos2d::ccArrayFastRemoveObjectAtIndexChild(__int64 int64Val, unsigned int intVal)
{
  __int64 int64Val2;
  unsigned int intVal2;
  __int64 int64Val3;
  _QWORD *qwordPtr;
  cocos2d::CCObject *returnVal;
  __int64 int64Val4;

  int64Val2 = *(_QWORD *)(int64Val + 16);
  intVal2 = *(_DWORD *)int64Val - 1;
  int64Val3 = 8LL * intVal;
  *(_DWORD *)int64Val = intVal2;
  qwordPtr = (_QWORD *)(int64Val2 + int64Val3);
  returnVal = *(cocos2d::CCObject **)(int64Val2 + int64Val3);
  int64Val4 = *(_QWORD *)(int64Val2 + 8LL * intVal2);
  *(_DWORD *)(int64Val4 + 36) = intVal;
  if ( returnVal )
  {
    cocos2d::CCObject::release(returnVal);
    returnVal = *(cocos2d::CCObject **)(int64Val + 16);
    qwordPtr = (cocos2d::CCObject_vtbl **)((char *)&returnVal->__vftable + int64Val3);
    int64Val4 = *((_QWORD *)&returnVal->__vftable + intVal2);
  }
  *qwordPtr = int64Val4;
  return returnVal;
}


cocos2d::CCObject * cocos2d::ccArrayFastRemoveObjectAtIndexNew(__int64 int64Val, unsigned int intVal)
{
  __int64 int64Val2;
  unsigned int intVal2;
  __int64 int64Val3;
  _QWORD *qwordPtr;
  cocos2d::CCObject *returnVal;
  __int64 int64Val4;

  int64Val2 = *(_QWORD *)(int64Val + 16);
  intVal2 = *(_DWORD *)int64Val - 1;
  int64Val3 = 8LL * intVal;
  *(_DWORD *)int64Val = intVal2;
  qwordPtr = (_QWORD *)(int64Val2 + int64Val3);
  returnVal = *(cocos2d::CCObject **)(int64Val2 + int64Val3);
  int64Val4 = *(_QWORD *)(int64Val2 + 8LL * intVal2);
  *(_DWORD *)(int64Val4 + 32) = intVal;
  if ( returnVal )
  {
    cocos2d::CCObject::release(returnVal);
    returnVal = *(cocos2d::CCObject **)(int64Val + 16);
    qwordPtr = (cocos2d::CCObject_vtbl **)((char *)&returnVal->__vftable + int64Val3);
    int64Val4 = *((_QWORD *)&returnVal->__vftable + intVal2);
  }
  *qwordPtr = int64Val4;
  return returnVal;
}


void cocos2d::ccArrayFree(void **voidPtr)
{
  unsigned int *intPtr;

  intPtr = (unsigned int *)*voidPtr;
  if ( intPtr )
  {
    cocos2d::ccArrayRemoveAllObjects(intPtr);
    free(*((void **)*voidPtr + 2));
    free(*voidPtr);
    *voidPtr = 0;
  }
}


__int64 cocos2d::ccArrayFullRemoveArray(unsigned int *intPtr, int *intPtr2)
{
  __int64 returnVal;
  unsigned int intVal;
  int intVal2;
  cocos2d::CCObject *obj;
  int intVal3;
  bool flag;
  unsigned int intVal4;
  __int64 int64Val;
  __int64 int64Val2;

  returnVal = *intPtr;
  if ( (_DWORD)returnVal )
  {
    intVal = 0;
    intVal2 = 0;
    do
    {
      while ( 1 )
      {
        flag = cocos2d::ccArrayContainsObject(intPtr2, *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * intVal));
        intVal4 = intVal - intVal2;
        if ( flag )
          break;
        int64Val = *((_QWORD *)intPtr + 2);
        int64Val2 = *(_QWORD *)(int64Val + 8LL * intVal++);
        *(_QWORD *)(int64Val + 8LL * intVal4) = int64Val2;
        intVal3 = *intPtr;
        if ( *intPtr <= intVal )
          goto LABEL_8;
      }
      ++intVal2;
      obj = *(cocos2d::CCObject **)(*((_QWORD *)intPtr + 2) + 8LL * intVal);
      if ( obj )
        cocos2d::CCObject::release(obj);
      intVal3 = *intPtr;
      ++intVal;
    }
    while ( *intPtr > intVal );
LABEL_8:
    returnVal = (unsigned int)(intVal3 - intVal2);
  }
  *intPtr = returnVal;
  return returnVal;
}


__int64 cocos2d::ccArrayGetIndexOfObject(int *intPtr, __int64 int64Val)
{
  int intVal;
  _QWORD *qwordPtr;
  __int64 returnVal;

  intVal = *intPtr;
  qwordPtr = (_QWORD *)*((_QWORD *)intPtr + 2);
  if ( *intPtr )
  {
    if ( *qwordPtr == int64Val )
      return 0;
    LODWORD(returnVal) = 0;
    while ( 1 )
    {
      returnVal = (unsigned int)(returnVal + 1);
      ++qwordPtr;
      if ( (_DWORD)returnVal == intVal )
        break;
      if ( *qwordPtr == int64Val )
        return returnVal;
    }
  }
  return 0xFFFFFFFFLL;
}


__int64 cocos2d::ccArrayInsertObjectAtIndex(int *intPtr, cocos2d::CCObject *obj, unsigned int intVal)
{
  int intVal2;
  __int64 returnVal;

  cocos2d::ccArrayEnsureExtraCapacity(intPtr, 1);
  if ( *intPtr != intVal )
    memmove(
      (void *)(*((_QWORD *)intPtr + 2) + 8LL * (intVal + 1)),
      (const void *)(*((_QWORD *)intPtr + 2) + 8LL * intVal),
      8LL * (*intPtr - intVal));
  cocos2d::CCObject::retain(obj);
  intVal2 = *intPtr;
  *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * intVal) = obj;
  returnVal = (unsigned int)(intVal2 + 1);
  *intPtr = returnVal;
  return returnVal;
}


__int64 cocos2d::ccArrayInsertObjectAtIndexBatch(unsigned int *intPtr, cocos2d::CCObject *obj, unsigned int intVal)
{
  unsigned int intVal2;
  __int64 returnVal;
  unsigned int intVal3;
  unsigned int intVal4;
  __int64 *int64Ptr;
  __int64 int64Val;

  cocos2d::ccArrayEnsureExtraCapacity(intPtr, 1);
  if ( *intPtr != intVal )
  {
    memmove(
      (void *)(*((_QWORD *)intPtr + 2) + 8LL * (intVal + 1)),
      (const void *)(*((_QWORD *)intPtr + 2) + 8LL * intVal),
      8LL * (*intPtr - intVal));
    intVal3 = *intPtr;
    intVal4 = intVal;
    if ( intVal < *intPtr )
    {
      int64Ptr = (__int64 *)(*((_QWORD *)intPtr + 2) + 8LL * (int)intVal);
      do
      {
        int64Val = *int64Ptr++;
        *(_DWORD *)(int64Val + 40) = intVal4++;
      }
      while ( intVal4 != intVal3 );
    }
  }
  cocos2d::CCObject::retain(obj);
  intVal2 = *intPtr;
  *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * intVal) = obj;
  returnVal = intVal2 + 1;
  obj->m_unknown2 = intVal;
  *intPtr = returnVal;
  return returnVal;
}


_QWORD * cocos2d::ccArrayNew()
{
  int intVal;
  size_t size_t;
  _QWORD *qwordPtr;
  _QWORD *qwordPtr2;

  intVal = (int)this;
  size_t = (unsigned int)this;
  if ( !(_DWORD)this )
  {
    size_t = 1;
    intVal = 1;
  }
  qwordPtr = malloc(0x18u);
  *(_DWORD *)qwordPtr = 0;
  qwordPtr[2] = calloc(size_t, 8u);
  *((_DWORD *)qwordPtr + 1) = intVal;
  qwordPtr2 = qwordPtr;
  *((_DWORD *)qwordPtr + 2) = -1;
  return qwordPtr2;
}


__int64 cocos2d::ccArrayRecreateChildIndexes(__int64 int64Val)
{
  int intVal;
  __int64 int64Val2;

  intVal = *(_DWORD *)int64Val;
  if ( *(int *)int64Val > 0 )
  {
    int64Val2 = *(_QWORD *)(int64Val + 16);
    int64Val = 0;
    do
    {
      *(_DWORD *)(*(_QWORD *)(int64Val2 + 8 * int64Val) + 36LL) = int64Val;
      ++int64Val;
    }
    while ( intVal > (int)int64Val );
  }
  return int64Val;
}


__int64 cocos2d::ccArrayRecreateNewIndexes(__int64 int64Val)
{
  int intVal;
  __int64 int64Val2;

  intVal = *(_DWORD *)int64Val;
  if ( *(int *)int64Val > 0 )
  {
    int64Val2 = *(_QWORD *)(int64Val + 16);
    int64Val = 0;
    do
    {
      *(_DWORD *)(*(_QWORD *)(int64Val2 + 8 * int64Val) + 32LL) = int64Val;
      ++int64Val;
    }
    while ( intVal > (int)int64Val );
  }
  return int64Val;
}


__int64 cocos2d::ccArrayRemoveAllObjects(unsigned int *intPtr)
{
  __int64 returnVal;
  unsigned int intVal;
  __int64 int64Val;

  cocos2d::ccArrayUpdateChildIndexes((__int64)intPtr);
  for ( returnVal = *intPtr; (_DWORD)returnVal; returnVal = *intPtr )
  {
    intVal = returnVal - 1;
    int64Val = *((_QWORD *)intPtr + 2);
    *intPtr = intVal;
    cocos2d::CCObject::release(*(cocos2d::CCObject **)(int64Val + 8LL * intVal));
  }
  return returnVal;
}


__int64 cocos2d::ccArrayRemoveArray(int *intPtr, unsigned int *intPtr2)
{
  __int64 returnVal;
  unsigned int i;
  __int64 int64Val;

  returnVal = *intPtr2;
  for ( i = 0; (unsigned int)returnVal > i; returnVal = *intPtr2 )
  {
    int64Val = *(_QWORD *)(*((_QWORD *)intPtr2 + 2) + 8LL * i++);
    cocos2d::ccArrayRemoveObject(intPtr, int64Val, 1);
  }
  return returnVal;
}


void cocos2d::ccArrayRemoveObject(int *intPtr, __int64 int64Val, char charVal)
{
  unsigned int IndexOfObject;

  IndexOfObject = cocos2d::ccArrayGetIndexOfObject(intPtr, int64Val);
  if ( IndexOfObject != -1 )
    cocos2d::ccArrayRemoveObjectAtIndex((__int64)intPtr, IndexOfObject, charVal);
}


void cocos2d::ccArrayRemoveObjectAtIndex(__int64 int64Val, unsigned int intVal, char charVal)
{
  cocos2d::CCObject *obj;
  int intVal2;
  unsigned int intVal3;
  unsigned int intVal4;

  if ( charVal )
  {
    obj = *(cocos2d::CCObject **)(*(_QWORD *)(int64Val + 16) + 8LL * intVal);
    if ( obj )
    {
      intVal4 = intVal;
      cocos2d::CCObject::release(obj);
      intVal = intVal4;
    }
  }
  intVal2 = *(_DWORD *)int64Val - 1;
  *(_DWORD *)int64Val = intVal2;
  intVal3 = intVal2 - intVal;
  if ( intVal3 )
    memmove((void *)(*(_QWORD *)(int64Val + 16) + 8LL * intVal), (const void *)(*(_QWORD *)(int64Val + 16) + 8LL * (intVal + 1)), 8LL * intVal3);
}


void cocos2d::ccArrayRemoveObjectAtIndexBatch(int *intPtr, unsigned int intVal, char charVal)
{
  __int64 int64Val;
  cocos2d::CCObject *obj;
  int intVal2;
  unsigned int intVal3;
  int intVal4;
  unsigned int intVal5;
  __int64 *int64Ptr;
  __int64 int64Val2;

  int64Val = 8LL * intVal;
  obj = *(cocos2d::CCObject **)(*((_QWORD *)intPtr + 2) + int64Val);
  obj->m_unknown2 = -1;
  if ( charVal )
    cocos2d::CCObject::release(obj);
  intVal2 = *intPtr - 1;
  *intPtr = intVal2;
  intVal3 = intVal2 - intVal;
  if ( intVal3 )
  {
    memmove((void *)(*((_QWORD *)intPtr + 2) + int64Val), (const void *)(*((_QWORD *)intPtr + 2) + 8LL * (intVal + 1)), 8LL * intVal3);
    intVal4 = *intPtr;
    intVal5 = intVal;
    if ( intVal < *intPtr )
    {
      int64Ptr = (__int64 *)(*((_QWORD *)intPtr + 2) + 8LL * (int)intVal);
      do
      {
        int64Val2 = *int64Ptr++;
        *(_DWORD *)(int64Val2 + 40) = intVal5++;
      }
      while ( intVal5 != intVal4 );
    }
  }
}


__int64 cocos2d::ccArrayRemoveObjectAtIndexChild(int *intPtr, unsigned int intVal, char charVal)
{
  __int64 int64Val;
  cocos2d::CCObject **obj;
  cocos2d::CCObject *obj2;
  int intVal2;
  __int64 returnVal;

  int64Val = 8LL * intVal;
  obj = (cocos2d::CCObject **)(*((_QWORD *)intPtr + 2) + int64Val);
  obj2 = *obj;
  (*obj)->m_uUnknown = -1;
  if ( charVal )
  {
    cocos2d::CCObject::release(obj2);
    obj = (cocos2d::CCObject **)(*((_QWORD *)intPtr + 2) + int64Val);
  }
  intVal2 = *intPtr;
  if ( intPtr[2] == -1 )
    intPtr[2] = intVal2;
  returnVal = (unsigned int)(intVal2 - 1);
  *intPtr = returnVal;
  *obj = 0;
  return returnVal;
}


int * cocos2d::ccArrayShrink(int *intPtr)
{
  int intVal;
  size_t size_t;
  int *intPtr2;
  void *voidPtr;

  intVal = *intPtr;
  if ( intPtr[1] > (unsigned int)*intPtr && *(_QWORD *)intPtr != 0x100000000LL )
  {
    if ( intVal )
    {
      size_t = 8LL * (unsigned int)*intPtr;
    }
    else
    {
      size_t = 8;
      intVal = 1;
    }
    intPtr2 = intPtr;
    voidPtr = (void *)*((_QWORD *)intPtr + 2);
    intPtr2[1] = intVal;
    intPtr = (int *)realloc(voidPtr, size_t);
    *((_QWORD *)intPtr2 + 2) = intPtr;
  }
  return intPtr;
}


__int64 cocos2d::ccArraySwapObjectsAtIndexes(__int64 int64Val, unsigned int intVal, unsigned int intVal2)
{
  __int64 int64Val2;
  __int64 int64Val3;
  __int64 returnVal;

  int64Val2 = *(_QWORD *)(int64Val + 16);
  int64Val3 = *(_QWORD *)(int64Val2 + 8LL * intVal);
  *(_QWORD *)(int64Val2 + 8LL * intVal) = *(_QWORD *)(int64Val2 + 8LL * intVal2);
  returnVal = *(_QWORD *)(int64Val + 16);
  *(_QWORD *)(returnVal + 8LL * intVal2) = int64Val3;
  return returnVal;
}


__int64 cocos2d::ccArrayUpdateChildIndexes(__int64 int64Val)
{
  int intVal;
  __int64 int64Val2;
  __int64 int64Val3;
  int intVal2;
  int intVal3;
  __int64 int64Val4;
  int intVal4;

  intVal = *(_DWORD *)(int64Val + 8);
  if ( intVal != -1 )
  {
    if ( intVal > 0 )
    {
      int64Val2 = *(_QWORD *)(int64Val + 16);
      int64Val3 = 0;
      intVal2 = 0;
      intVal3 = 0;
      do
      {
        while ( 1 )
        {
          int64Val4 = *(_QWORD *)(int64Val2 + int64Val3);
          intVal4 = intVal2 - intVal3;
          if ( int64Val4 )
            break;
          ++intVal2;
          ++intVal3;
          int64Val3 += 8;
          if ( intVal == intVal2 )
            goto LABEL_9;
        }
        if ( intVal3 )
        {
          *(_QWORD *)(int64Val2 + 8LL * intVal4) = int64Val4;
          int64Val2 = *(_QWORD *)(int64Val + 16);
          *(_DWORD *)(*(_QWORD *)(int64Val2 + 8LL * intVal4) + 36LL) = intVal4;
        }
        ++intVal2;
        int64Val3 += 8;
      }
      while ( intVal != intVal2 );
    }
LABEL_9:
    *(_DWORD *)(int64Val + 8) = -1;
  }
  return int64Val;
}


__int64 cocos2d::ccCArrayAppendArray(unsigned int *intPtr, unsigned int *intPtr2)
{
  __int64 returnVal;
  unsigned int i;
  cocos2d::_ccCArray *_ccCArray;

  returnVal = *intPtr2;
  for ( i = 0; (unsigned int)returnVal > i; returnVal = *intPtr2 )
  {
    _ccCArray = *(cocos2d::_ccCArray **)(*((_QWORD *)intPtr2 + 2) + 8LL * i++);
    cocos2d::ccCArrayAppendValue(intPtr, _ccCArray);
  }
  return returnVal;
}


__int64 cocos2d::ccCArrayAppendArrayWithResize(unsigned int *intPtr, int *intPtr2)
{
  cocos2d::ccCArrayEnsureExtraCapacity(intPtr, *intPtr2);
  return cocos2d::ccCArrayAppendArray(intPtr, (unsigned int *)intPtr2);
}


unsigned int * cocos2d::ccCArrayAppendValue(unsigned int *this, cocos2d::_ccCArray *_ccCArray)
{
  unsigned int intVal;
  unsigned int intVal2;

  intVal = this[1];
  intVal2 = *this + 1;
  *(_QWORD *)(*((_QWORD *)this + 2) + 8LL * *this) = _ccCArray;
  *this = intVal2;
  if ( intVal2 >= intVal )
    return (unsigned int *)cocos2d::ccCArrayDoubleCapacity((__int64)this);
  return this;
}


unsigned int * cocos2d::ccCArrayAppendValueWithResize(unsigned int *intPtr, cocos2d::_ccCArray *_ccCArray)
{
  cocos2d::ccCArrayEnsureExtraCapacity(intPtr, 1);
  return cocos2d::ccCArrayAppendValue(intPtr, _ccCArray);
}


bool cocos2d::ccCArrayContainsValue(int *intPtr, __int64 int64Val)
{
  return (unsigned int)cocos2d::ccCArrayGetIndexOfValue(intPtr, int64Val) != -1;
}


__int64 cocos2d::ccCArrayFastRemoveValueAtIndex(__int64 int64Val, unsigned int intVal)
{
  __int64 int64Val2;
  unsigned int intVal2;
  __int64 returnVal;

  int64Val2 = *(_QWORD *)(int64Val + 16);
  intVal2 = *(_DWORD *)int64Val - 1;
  *(_DWORD *)int64Val = intVal2;
  returnVal = *(_QWORD *)(int64Val2 + 8LL * intVal2);
  *(_QWORD *)(int64Val2 + 8LL * intVal) = returnVal;
  return returnVal;
}


void cocos2d::ccCArrayFree(__int64 int64Val)
{
  if ( int64Val )
  {
    cocos2d::ccCArrayRemoveAllValues((_DWORD *)int64Val);
    free(*(void **)(int64Val + 16));
    free((void *)int64Val);
  }
}


__int64 cocos2d::ccCArrayFullRemoveArray(unsigned int *intPtr, int *intPtr2)
{
  __int64 returnVal;
  unsigned int intVal;
  int intVal2;
  int intVal3;
  bool flag;
  unsigned int intVal4;
  __int64 int64Val;
  __int64 int64Val2;

  returnVal = *intPtr;
  if ( (_DWORD)returnVal )
  {
    intVal = 0;
    intVal2 = 0;
    do
    {
      while ( 1 )
      {
        flag = cocos2d::ccCArrayContainsValue(intPtr2, *(_QWORD *)(*((_QWORD *)intPtr + 2) + 8LL * intVal));
        intVal4 = intVal - intVal2;
        if ( !flag )
          break;
        intVal3 = *intPtr;
        ++intVal;
        ++intVal2;
        if ( *intPtr <= intVal )
          goto LABEL_6;
      }
      int64Val = *((_QWORD *)intPtr + 2);
      int64Val2 = *(_QWORD *)(int64Val + 8LL * intVal++);
      *(_QWORD *)(int64Val + 8LL * intVal4) = int64Val2;
      intVal3 = *intPtr;
    }
    while ( *intPtr > intVal );
LABEL_6:
    returnVal = (unsigned int)(intVal3 - intVal2);
  }
  *intPtr = returnVal;
  return returnVal;
}


__int64 cocos2d::ccCArrayGetIndexOfValue(int *intPtr, __int64 int64Val)
{
  int intVal;
  _QWORD *qwordPtr;
  __int64 *int64Ptr;
  __int64 returnVal;
  __int64 int64Val2;

  intVal = *intPtr;
  if ( *intPtr )
  {
    qwordPtr = (_QWORD *)*((_QWORD *)intPtr + 2);
    if ( *qwordPtr == int64Val )
      return 0;
    int64Ptr = qwordPtr + 1;
    LODWORD(returnVal) = 0;
    while ( 1 )
    {
      returnVal = (unsigned int)(returnVal + 1);
      if ( (_DWORD)returnVal == intVal )
        break;
      int64Val2 = *int64Ptr++;
      if ( int64Val2 == int64Val )
        return returnVal;
    }
  }
  return 0xFFFFFFFFLL;
}


__int64 cocos2d::ccCArrayInsertValueAtIndex(__int64 int64Val, __int64 int64Val2, unsigned int intVal)
{
  unsigned int intVal2;
  __int64 int64Val3;
  __int64 int64Val4;
  __int64 returnVal;
  unsigned int intVal3;

  intVal2 = *(_DWORD *)int64Val - intVal;
  if ( *(_DWORD *)int64Val + 1 == *(_DWORD *)(int64Val + 4) )
  {
    intVal3 = intVal;
    cocos2d::ccCArrayDoubleCapacity(int64Val);
    intVal = intVal3;
  }
  int64Val3 = 8LL * intVal;
  if ( intVal2 )
    memmove((void *)(*(_QWORD *)(int64Val + 16) + 8LL * (intVal + 1)), (const void *)(*(_QWORD *)(int64Val + 16) + int64Val3), 8LL * intVal2);
  int64Val4 = *(_QWORD *)(int64Val + 16);
  returnVal = (unsigned int)(*(_DWORD *)int64Val + 1);
  *(_DWORD *)int64Val = returnVal;
  *(_QWORD *)(int64Val4 + int64Val3) = int64Val2;
  return returnVal;
}


_DWORD * cocos2d::ccCArrayNew()
{
  int intVal;
  size_t size_t;
  _DWORD *returnVal;
  void *voidPtr;

  intVal = (int)this;
  if ( (_DWORD)this )
  {
    size_t = 8LL * (unsigned int)this;
  }
  else
  {
    size_t = 8;
    intVal = 1;
  }
  returnVal = malloc(0x18u);
  *returnVal = 0;
  voidPtr = malloc(size_t);
  returnVal[1] = intVal;
  *((_QWORD *)returnVal + 2) = voidPtr;
  return returnVal;
}


_DWORD * cocos2d::ccCArrayRemoveAllValues(_DWORD *dwordPtr)
{
  *dwordPtr = 0;
  return dwordPtr;
}


__int64 cocos2d::ccCArrayRemoveArray(int *intPtr, unsigned int *intPtr2)
{
  __int64 returnVal;
  unsigned int i;
  __int64 int64Val;

  returnVal = *intPtr2;
  for ( i = 0; (unsigned int)returnVal > i; returnVal = *intPtr2 )
  {
    int64Val = *(_QWORD *)(*((_QWORD *)intPtr2 + 2) + 8LL * i++);
    cocos2d::ccCArrayRemoveValue(intPtr, int64Val);
  }
  return returnVal;
}


unsigned int * cocos2d::ccCArrayRemoveValue(int *intPtr, __int64 int64Val)
{
  unsigned int *returnVal;

  returnVal = (unsigned int *)cocos2d::ccCArrayGetIndexOfValue(intPtr, int64Val);
  if ( (_DWORD)returnVal != -1 )
    return cocos2d::ccCArrayRemoveValueAtIndex((unsigned int *)intPtr, (unsigned int)returnVal);
  return returnVal;
}


unsigned int * cocos2d::ccCArrayRemoveValueAtIndex(unsigned int *intPtr, unsigned int intVal)
{
  unsigned int intVal2;
  unsigned int intVal3;
  __int64 int64Val;

  intVal2 = *intPtr;
  intVal3 = *intPtr - 1;
  *intPtr = intVal3;
  if ( intVal3 > intVal )
  {
    int64Val = 8LL * intVal;
    do
    {
      *(_QWORD *)(*((_QWORD *)intPtr + 2) + int64Val) = *(_QWORD *)(*((_QWORD *)intPtr + 2)
                                                            + 8 * (intVal + 1 - (unsigned __int64)intVal)
                                                            + int64Val);
      int64Val += 8;
    }
    while ( int64Val != 8 * (intVal + 1LL + intVal2 - 2 - intVal) );
  }
  return intPtr;
}


void cocos2d::ccDrawCardinalSpline(cocos2d::CCPointArray *p0, float p1, unsigned int p2)
{
  unsigned int intVal;
  float value;
  float value2;
  _DWORD *dwordPtr;
  unsigned int intVal2;
  unsigned int intVal3;
  float value3;
  float value4;
  _DWORD *dwordPtr2;
  cocos2d::CCPoint point;
  cocos2d::CCPoint point2;
  cocos2d::CCPoint point3;
  cocos2d::CCPoint point4;
  int intVal4;
  int intVal5;

  if ( !byte_12451A0 )
    sub_AD7B20();
  intVal = p2 + 1;
  dwordPtr2 = (_DWORD *)operator new[](8LL * (p2 + 1));
  value = 1.0 / (float)cocos2d::CCPointArray::count(p0);
  if ( p2 != -1 )
  {
    value2 = (float)p2;
    dwordPtr = dwordPtr2;
    intVal2 = 0;
    do
    {
      value4 = (float)intVal2 / value2;
      if ( value4 == 1.0 )
      {
        value3 = 1.0;
        intVal3 = cocos2d::CCPointArray::count(p0) - 1;
      }
      else
      {
        intVal3 = (unsigned int)(float)(value4 / value);
        value3 = (float)(value4 - (float)((float)intVal3 * value)) / value;
      }
      dwordPtr += 2;
      ++intVal2;
      cocos2d::CCPointArray::getControlPointAtIndex(p0, intVal3 - 1);
      cocos2d::CCPointArray::getControlPointAtIndex(p0, intVal3);
      cocos2d::CCPointArray::getControlPointAtIndex(p0, intVal3 + 1);
      cocos2d::CCPointArray::getControlPointAtIndex(p0, intVal3 + 2);
      cocos2d::ccCardinalSplineAt(&point, &point2, &point3, &point4, p1, value3);
      *(dwordPtr - 2) = intVal4;
      *(dwordPtr - 1) = intVal5;
    }
    while ( intVal != intVal2 );
  }
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CC8,
    (float *)&dword_1230CD0,
    1u);
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  glVertexAttribPointer();
  glDrawArrays();
  operator delete[](dwordPtr2);
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawCatmullRom(cocos2d::CCPointArray *p0, unsigned int p1)
{
  cocos2d::ccDrawCardinalSpline(p0, 0.5, p1);
}


void cocos2d::ccDrawColor4B(unsigned __int8 p0, unsigned __int8 p1, unsigned __int8 p2, unsigned __int8 p3)
{
  *(float *)&dword_1230CD0 = (float)p0 / 255.0;
  *(float *)&dword_1230CD4 = (float)p1 / 255.0;
  *(float *)&dword_1230CD8 = (float)p2 / 255.0;
  *(float *)&dword_1230CDC = (float)p3 / 255.0;
}


void cocos2d::ccDrawColor4F(float p0, float p1, float p2, float p3)
{
  dword_1230CD0 = LODWORD(p0);
  dword_1230CD4 = LODWORD(p1);
  dword_1230CD8 = LODWORD(p2);
  dword_1230CDC = LODWORD(p3);
}


void cocos2d::ccDrawFilledCircle(const cocos2d::CCPoint *p0, float p1, float p2, unsigned int p3)
{
  float value;
  float *value2;
  float x;
  unsigned int intVal;
  float y;
  unsigned int intVal2;
  float value3;
  float value4;
  unsigned int intVal3;
  unsigned int intVal4;
  cocos2d::CCGLProgram *cCGLProgram;

  if ( !byte_12451A0 )
    sub_AD7B20();
  value = 6.2832 / (float)p3;
  value2 = (float *)calloc(8LL * (p3 + 2), 1u);
  if ( value2 )
  {
    x = p0->x;
    intVal = 1;
    y = p0->y;
    intVal2 = 0;
    do
    {
      value3 = cosf(p2 + (float)((float)intVal2 * value));
      value4 = sinf(p2 + (float)((float)intVal2 * value));
      intVal3 = 2 * intVal2++;
      value2[intVal3] = x + (float)(value3 * p1);
      value2[intVal] = y + (float)(value4 * p1);
      intVal += 2;
    }
    while ( p3 >= intVal2 );
    intVal4 = p3 + 1;
    cCGLProgram = (cocos2d::CCGLProgram *)qword_1245198;
    value2[2 * intVal4] = x;
    value2[2 * intVal4 + 1] = y;
    cocos2d::CCGLProgram::use(cCGLProgram);
    cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
    cocos2d::CCGLProgram::setUniformLocationWith4fv(
      (cocos2d::CCGLProgram *)qword_1245198,
      dword_1230CC8,
      (float *)&dword_1230CD0,
      1u);
    cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
    glVertexAttribPointer();
    glDrawArrays();
    free(value2);
    ++g_uNumberOfDraws;
  }
}


void cocos2d::ccDrawFree()
{
  if ( qword_1245198 )
  {
    cocos2d::CCObject::release((cocos2d::CCObject *)qword_1245198);
    qword_1245198 = 0;
  }
  byte_12451A0 = 0;
}


void cocos2d::ccDrawInit()
{
  cocos2d::CCShaderCache *cCShaderCache;

  if ( !byte_12451A0 )
  {
    cCShaderCache = cocos2d::CCShaderCache::sharedShaderCache();
    qword_1245198 = (__int64)cocos2d::CCShaderCache::programForKey(cCShaderCache, "ShaderPosition_uColor");
    cocos2d::CCObject::retain((cocos2d::CCObject *)qword_1245198);
    dword_1230CC8 = glGetUniformLocation();
    dword_1230CCC = glGetUniformLocation();
    byte_12451A0 = 1;
  }
}


void cocos2d::ccDrawLine(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1)
{
  if ( !byte_12451A0 )
    sub_AD7B20();
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CC8,
    (float *)&dword_1230CD0,
    1u);
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  glVertexAttribPointer();
  glDrawArrays();
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawLines(const cocos2d::CCPoint *p0, unsigned int p1)
{
  void *voidPtr;

  if ( !byte_12451A0 )
    sub_AD7B20();
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CC8,
    (float *)&dword_1230CD0,
    1u);
  cocos2d::CCGLProgram::setUniformLocationWith1f(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CCC,
    *(float *)&dword_1230CE0);
  voidPtr = (void *)operator new[](8LL * p1);
  glVertexAttribPointer();
  glDrawArrays();
  operator delete[](voidPtr);
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawPoint(const cocos2d::CCPoint *p0)
{
  if ( !byte_12451A0 )
    sub_AD7B20();
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CC8,
    (float *)&dword_1230CD0,
    1u);
  cocos2d::CCGLProgram::setUniformLocationWith1f(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CCC,
    *(float *)&dword_1230CE0);
  glVertexAttribPointer();
  glDrawArrays();
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawPoints(const cocos2d::CCPoint *p0, unsigned int p1)
{
  void *voidPtr;

  if ( !byte_12451A0 )
    sub_AD7B20();
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CC8,
    (float *)&dword_1230CD0,
    1u);
  cocos2d::CCGLProgram::setUniformLocationWith1f(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CCC,
    *(float *)&dword_1230CE0);
  voidPtr = (void *)operator new[](8LL * p1);
  glVertexAttribPointer();
  glDrawArrays();
  operator delete[](voidPtr);
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawPoly(const cocos2d::CCPoint *p0, unsigned int p1, bool p2)
{
  if ( !byte_12451A0 )
    sub_AD7B20();
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv(
    (cocos2d::CCGLProgram *)qword_1245198,
    dword_1230CC8,
    (float *)&dword_1230CD0,
    1u);
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  glVertexAttribPointer();
  glDrawArrays();
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawRect(cocos2d::CCPoint p0, cocos2d::CCPoint p1)
{
  float *value;
  float *value2;
  float *value3;
  float *value22;
  cocos2d::CCPoint point;
  cocos2d::CCPoint point2;

  value3 = value;
  value22 = value2;
  cocos2d::CCPoint::CCPoint(&point, *value, value[1]);
  cocos2d::CCPoint::CCPoint(&point2, *value22, value3[1]);
  cocos2d::ccDrawLine(&point, &point2);
  cocos2d::CCPoint::CCPoint(&point, *value22, value3[1]);
  cocos2d::CCPoint::CCPoint(&point2, *value22, value22[1]);
  cocos2d::ccDrawLine(&point, &point2);
  cocos2d::CCPoint::CCPoint(&point, *value22, value22[1]);
  cocos2d::CCPoint::CCPoint(&point2, *value3, value22[1]);
  cocos2d::ccDrawLine(&point, &point2);
  cocos2d::CCPoint::CCPoint(&point, *value3, value22[1]);
  cocos2d::CCPoint::CCPoint(&point2, *value3, value3[1]);
  cocos2d::ccDrawLine(&point, &point2);
}


void cocos2d::ccDrawSolidPoly(const cocos2d::CCPoint *p0, unsigned int p1, cocos2d::ccColor4F p2)
{
  void *voidPtr;
  cocos2d::ccColor4F p1a;

  p1a = p2;
  if ( !byte_12451A0 )
    sub_AD7B20();
  cocos2d::CCGLProgram::use((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformsForBuiltins((cocos2d::CCGLProgram *)qword_1245198);
  cocos2d::CCGLProgram::setUniformLocationWith4fv((cocos2d::CCGLProgram *)qword_1245198, dword_1230CC8, &p1a.r, 1u);
  cocos2d::ccGLEnableVertexAttribs((unsigned int)dword_0 + 1);
  voidPtr = (void *)operator new[](8LL * p1);
  glVertexAttribPointer();
  glDrawArrays();
  operator delete[](voidPtr);
  ++g_uNumberOfDraws;
}


void cocos2d::ccDrawSolidRect(cocos2d::CCPoint origin, cocos2d::CCPoint destination, cocos2d::ccColor4F color)
{
  const cocos2d::CCPoint *point;
  const cocos2d::CCPoint *point2;
  unsigned __int64 int64Val;
  const cocos2d::CCPoint *point22;
  float *value;
  unsigned __int64 int64Val2;
  cocos2d::CCPoint point3;
  cocos2d::CCPoint point4;
  cocos2d::CCPoint point5;
  cocos2d::CCPoint point6;
  cocos2d::ccColor4F ccColor4F;

  int64Val = LODWORD(destination.x) | ((unsigned __int64)LODWORD(destination.y) << 32);
  point22 = point2;
  value = (float *)point;
  int64Val2 = LODWORD(origin.x) | ((unsigned __int64)LODWORD(origin.y) << 32);
  cocos2d::CCPoint::CCPoint(&point3, point);
  cocos2d::CCPoint::CCPoint(&point4, point22->x, value[1]);
  cocos2d::CCPoint::CCPoint(&point5, point22);
  cocos2d::CCPoint::CCPoint(&point6, *value, point22->y);
  *(_QWORD *)&ccColor4F.r = int64Val2;
  *(_QWORD *)&ccColor4F.b = int64Val;
  cocos2d::ccDrawSolidPoly(&point3, 4u, ccColor4F);
}


void cocos2d::ccGLBindTexture2D(unsigned int p0)
{
  cocos2d::ccGLBindTexture2DN(0, p0);
}


void cocos2d::ccGLBindTexture2DN(unsigned int p0, unsigned int p1)
{
  if ( *((_DWORD *)&xmmword_1230DA0 + p0) != p1 )
  {
    *((_DWORD *)&xmmword_1230DA0 + p0) = p1;
    glActiveTexture();
    glBindTexture();
  }
}


void cocos2d::ccGLBindVAO(unsigned int p0)
{
  ;
}


void cocos2d::ccGLBlendFunc(unsigned int p0, unsigned int p1)
{
  if ( p0 != dword_1230DE0 || p1 != dword_1230DE4 )
  {
    dword_1230DE0 = p0;
    dword_1230DE4 = p1;
    if ( p0 != 1 || p1 )
    {
      glEnable();
      glBlendFunc();
    }
    else
    {
      glDisable();
    }
  }
}


void cocos2d::ccGLBlendResetToCache()
{
  glBlendEquation();
  if ( dword_1230DE4 || dword_1230DE0 != 1 )
  {
    glEnable();
    glBlendFunc();
  }
  else
  {
    glDisable();
  }
}


void cocos2d::ccGLDeleteProgram(unsigned int p0)
{
  if ( p0 == dword_1230D94 )
    dword_1230D94 = -1;
  glDeleteProgram();
}


void cocos2d::ccGLDeleteTexture(unsigned int p0)
{
  cocos2d::ccGLDeleteTextureN(0, p0);
}


void cocos2d::ccGLDeleteTextureN(unsigned int p0, unsigned int p1)
{
  if ( *((_DWORD *)&xmmword_1230DA0 + p0) == p1 )
    *((_DWORD *)&xmmword_1230DA0 + p0) = -1;
  glDeleteTextures();
}


void cocos2d::ccGLEnable(cocos2d::ccGLServerState p0)
{
  ;
}


void cocos2d::ccGLEnableVertexAttribs(unsigned int p0)
{
  unsigned __int64 int64Val;
  unsigned int intVal;
  __int64 int64Val2;
  __int64 int64Val3;

  int64Val = p0;
  intVal = p0 & 1;
  cocos2d::ccGLBindVAO(0);
  if ( (unsigned __int8)byte_1245A38 != intVal )
  {
    if ( intVal )
    {
      glEnableVertexAttribArray();
      byte_1245A38 = intVal;
    }
    else
    {
      glDisableVertexAttribArray();
      byte_1245A38 = 0;
    }
  }
  int64Val2 = (int64Val >> 1) & 1;
  if ( (unsigned __int8)byte_1245A39 != (_DWORD)int64Val2 )
  {
    if ( (_DWORD)int64Val2 )
      glEnableVertexAttribArray();
    else
      glDisableVertexAttribArray();
    byte_1245A39 = (int64Val & 2) != 0;
  }
  int64Val3 = (int64Val >> 2) & 1;
  if ( (unsigned __int8)byte_1245A3A != (_DWORD)int64Val3 )
  {
    if ( (_DWORD)int64Val3 )
      glEnableVertexAttribArray();
    else
      glDisableVertexAttribArray();
    byte_1245A3A = int64Val3;
  }
}


void cocos2d::ccGLInvalidateStateCache()
{
  kmGLFreeAll();
  xmmword_1230DA0 = 0u;
  xmmword_1230DC0 = 0u;
  dword_1230D94 = -1;
  xmmword_1230DD0 = 0u;
  byte_1245A39 = 0;
  byte_1245A3A = 0;
  xmmword_1230DB0 = 0u;
  dword_1230DE0 = -1;
  dword_1230DE4 = -1;
  dword_1245A3C = 0;
  dword_1230D90 = -1;
  byte_1245A38 = 0;
}


void cocos2d::ccGLUseProgram(unsigned int p0)
{
  if ( p0 != dword_1230D94 )
  {
    dword_1230D94 = p0;
    glUseProgram();
  }
}


unsigned __int64 cocos2d::ccNextPOT()
{
  unsigned __int64 int64Val;
  unsigned __int64 int64Val2;

  int64Val = ((unsigned __int64)this - 1)
     | (((unsigned __int64)this - 1) >> 1)
     | ((((unsigned __int64)this - 1) | (((unsigned __int64)this - 1) >> 1)) >> 2);
  int64Val2 = int64Val | (int64Val >> 4) | ((int64Val | (int64Val >> 4)) >> 8);
  return (int64Val2 | (int64Val2 >> 16)) + 1;
}


void cocos2d::ccPointSize(float p0)
{
  cocos2d::CCDirector *cCDirector;

  cCDirector = cocos2d::CCDirector::sharedDirector();
  *(float *)&dword_1230CE0 = cocos2d::CCDirector::getContentScaleFactor(cCDirector) * p0;
}


void cocos2d::ccSetProjectionMatrixDirty()
{
  dword_1230D90 = -1;
}


char * cocos2d::cc_utf16_to_utf8(const unsigned __int16 *p0, __int64 p1, __int64 *p2, __int64 *p3)
{
  int intVal;
  int intVal2;
  __int64 int64Val;
  int *intPtr;
  int *intPtr2;
  __int64 int64Val2;
  unsigned __int64 int64Val3;
  unsigned __int16 int16Val;
  char *charPtr;
  char *charPtr2;
  char *charPtr3;
  char *charPtr4;
  int *intPtr3;
  unsigned int *intPtr4;
  int intVal3;
  unsigned int *intPtr5;
  int intVal4;
  int *intPtr6;
  char *charPtr5;

  if ( !p0 )
    return 0;
  intPtr6 = (int *)&algn_1245BC4[4];
  intVal = cocos2d::cc_wcslen(p0);
  intVal2 = intVal;
  if ( intVal <= 0 )
  {
    charPtr5 = &byte_124B6C8;
    if ( !intVal )
    {
      charPtr2 = (char *)operator new[](qword_124B6B0 + 1);
      charPtr2[*((_QWORD *)charPtr5 - 3)] = 0;
LABEL_14:
      charPtr3 = charPtr5;
      if ( p1 )
        *(_DWORD *)p1 = *((_QWORD *)charPtr5 - 3);
      goto LABEL_17;
    }
  }
  else
  {
    int64Val = 0;
    intPtr = intPtr6;
    intPtr2 = intPtr6 - 6;
    do
    {
      while ( 1 )
      {
        int64Val2 = *(_QWORD *)intPtr2;
        int64Val3 = *(_QWORD *)intPtr2 + 1LL;
        int16Val = p0[int64Val];
        if ( int64Val3 > *((_QWORD *)intPtr2 + 1) || *(intPtr - 2) > 0 )
        {
          std::wstring::reserve(&intPtr6, *(_QWORD *)intPtr2 + 1LL);
          intPtr = intPtr6;
          intPtr2 = intPtr6 - 6;
          int64Val2 = *((_QWORD *)intPtr6 - 3);
        }
        *((_WORD *)intPtr + int64Val2) = int16Val;
        if ( intPtr2 != (int *)&std::wstring::_Rep::_S_empty_rep_storage )
          break;
        if ( intVal2 <= (int)++int64Val )
          goto LABEL_10;
      }
      *(intPtr - 2) = 0;
      ++int64Val;
      *(_QWORD *)intPtr2 = int64Val3;
      *((_WORD *)intPtr + int64Val3) = 0;
    }
    while ( intVal2 > (int)int64Val );
LABEL_10:
    charPtr5 = &byte_124B6C8;
  }
  if ( (unsigned __int8)llvm::convertUTF16ToUTF8String(&intPtr6, &charPtr5) )
  {
    charPtr = (char *)operator new[](*((_QWORD *)charPtr5 - 3) + 1LL);
    charPtr2 = charPtr;
    charPtr[*((_QWORD *)charPtr5 - 3)] = 0;
    if ( intVal2 > 0 )
      memcpy(charPtr, charPtr5, *((_QWORD *)charPtr5 - 3));
    goto LABEL_14;
  }
  charPtr3 = charPtr5;
  charPtr2 = 0;
LABEL_17:
  charPtr4 = charPtr3 - 24;
  if ( charPtr3 - 24 != (char *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr5 = (unsigned int *)(charPtr3 - 8);
      do
        intVal4 = __ldaxr(intPtr5);
      while ( __stlxr(intVal4 - 1, intPtr5) );
    }
    else
    {
      intVal4 = *((_DWORD *)charPtr3 - 2);
      *((_DWORD *)charPtr3 - 2) = intVal4 - 1;
    }
    if ( intVal4 <= 0 )
      operator delete(charPtr4);
  }
  intPtr3 = intPtr6 - 6;
  if ( intPtr6 - 6 != (int *)&std::wstring::_Rep::_S_empty_rep_storage )
  {
    if ( &pthread_create )
    {
      intPtr4 = (unsigned int *)(intPtr6 - 2);
      do
        intVal3 = __ldaxr(intPtr4);
      while ( __stlxr(intVal3 - 1, intPtr4) );
    }
    else
    {
      intVal3 = *(intPtr6 - 2);
      *(intPtr6 - 2) = intVal3 - 1;
    }
    if ( intVal3 <= 0 )
      operator delete(intPtr3);
  }
  return charPtr2;
}


unsigned int cocos2d::cc_utf8_find_last_not_char(std::vector<unsigned short> *p0, unsigned __int16 p1)
{
  unsigned __int16 *m_start;
  unsigned int returnVal;
  int intVal;
  unsigned __int16 *int16Ptr;
  int intVal2;

  m_start = p0->m_start;
  returnVal = p0->m_finish - p0->m_start - 1;
  if ( (returnVal & 0x80000000) == 0 )
  {
    intVal = m_start[returnVal];
    if ( intVal == p1 )
    {
      int16Ptr = &m_start[returnVal - 1];
      do
      {
        if ( --returnVal == -1 )
          break;
        intVal2 = *int16Ptr--;
      }
      while ( intVal2 == intVal );
    }
  }
  return returnVal;
}


__int64 cocos2d::cc_utf8_strlen(const char *p0, int p1)
{
  __int64 CharacterCountInUTF8String;
  char *charPtr;
  unsigned int *intPtr;
  int intVal;
  const char *charPtr2;

  if ( !p0 )
    return 0;
  sub_D153B8(&charPtr2, (char *)p0);
  CharacterCountInUTF8String = cocos2d::getCharacterCountInUTF8String(&charPtr2);
  charPtr = (char *)(charPtr2 - 24);
  if ( charPtr2 - 24 != (const char *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr = (unsigned int *)(charPtr2 - 8);
      do
        intVal = __ldaxr(intPtr);
      while ( __stlxr(intVal - 1, intPtr) );
    }
    else
    {
      intVal = *((_DWORD *)charPtr2 - 2);
      *((_DWORD *)charPtr2 - 2) = intVal - 1;
    }
    if ( intVal <= 0 )
      operator delete(charPtr);
  }
  return CharacterCountInUTF8String;
}


unsigned __int16 * cocos2d::cc_utf8_to_utf16(const char *p0, int *p1)
{
  unsigned __int64 int64Val;
  __int64 int64Val2;
  unsigned __int64 int64Val3;
  void *voidPtr;
  const unsigned __int8 **int8Ptr;
  void *voidPtr2;
  void *voidPtr3;
  unsigned int *intPtr;
  int intVal;
  size_t int64Val22;
  __int64 int64Val4;
  std::string str;
  char *charPtr;

  if ( !p0 )
    return 0;
  sub_D153B8(&int64Val4, (char *)p0);
  int64Val = *(_QWORD *)(int64Val4 - 24) + 1LL;
  if ( int64Val <= 0x3F80000000000000LL )
  {
    int64Val3 = 2 * int64Val;
    int64Val2 = 2 * int64Val;
  }
  else
  {
    int64Val2 = 2 * int64Val;
    int64Val3 = -1;
  }
  int64Val22 = int64Val2;
  voidPtr = (void *)operator new[](int64Val3);
  memset(voidPtr, 0, int64Val22);
  str.m_data.m_data = (stl::StringData<char>::Internal *)voidPtr;
  charPtr = 0;
  if ( !*(_QWORD *)(int64Val4 - 24)
    || (unsigned __int8)llvm::ConvertUTF8toWide((llvm *)((char *)dword_0 + 2), (unsigned int)&int64Val4, &str, &charPtr, int8Ptr) )
  {
    if ( p1 )
      *p1 = cocos2d::cc_wcslen((const unsigned __int16 *)voidPtr);
  }
  else
  {
    voidPtr3 = voidPtr;
    voidPtr = 0;
    operator delete[](voidPtr3);
  }
  voidPtr2 = (void *)(int64Val4 - 24);
  if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr = (unsigned int *)(int64Val4 - 8);
      do
        intVal = __ldaxr(intPtr);
      while ( __stlxr(intVal - 1, intPtr) );
    }
    else
    {
      intVal = *(_DWORD *)(int64Val4 - 8);
      *(_DWORD *)(int64Val4 - 8) = intVal - 1;
    }
    if ( intVal <= 0 )
      operator delete(voidPtr2);
  }
  return (unsigned __int16 *)voidPtr;
}


void cocos2d::cc_utf8_trim_ws(std::vector<unsigned short> *p0)
{
  signed __int64 int64Val;
  int intVal;
  int intVal2;
  __int64 intVal22;
  unsigned __int16 int16Val;
  unsigned __int16 *m_start;
  unsigned __int16 *m_finish;
  __int64 int64Val2;
  unsigned __int16 *int16Ptr;
  unsigned __int16 *int16Ptr2;
  signed __int64 int64Val3;

  int64Val = p0->m_finish - p0->m_start;
  if ( (int)int64Val > 0 )
  {
    intVal = int64Val - 1;
    if ( cocos2d::isspace_unicode(p0->m_start[(int)int64Val - 1]) )
    {
      intVal2 = int64Val - 2;
      if ( intVal2 != -1 )
      {
        intVal22 = intVal2;
        while ( 1 )
        {
          intVal = intVal2 + 1;
          int16Val = p0->m_start[intVal22--];
          if ( !cocos2d::isspace_unicode(int16Val) )
            break;
          if ( !intVal2 )
          {
            intVal = 0;
            m_finish = p0->m_finish;
            m_start = p0->m_start;
            int64Val2 = m_finish - p0->m_start;
            if ( (int)int64Val2 <= 0 )
              return;
            goto LABEL_10;
          }
          --intVal2;
        }
      }
      m_start = p0->m_start;
      m_finish = p0->m_finish;
      int64Val2 = m_finish - p0->m_start;
      if ( (int)int64Val2 > intVal )
      {
LABEL_10:
        int16Ptr = &m_start[intVal];
        int16Ptr2 = &m_start[(int)int64Val2];
        if ( int16Ptr != int16Ptr2 )
        {
          if ( m_finish == int16Ptr2 )
          {
            int64Val3 = 0;
          }
          else
          {
            int64Val3 = (char *)m_finish - (char *)int16Ptr2;
            if ( int64Val3 >> 1 )
            {
              memmove(int16Ptr, int16Ptr2, int64Val3);
              int64Val3 = (char *)p0->m_finish - (char *)int16Ptr2;
            }
          }
          p0->m_finish = (unsigned __int16 *)((char *)int16Ptr + int64Val3);
        }
      }
    }
  }
}


int cocos2d::cc_wcslen(const unsigned __int16 *p0)
{
  const unsigned __int16 *int16Ptr;
  int returnVal;
  int intVal;

  int16Ptr = p0 + 1;
  if ( !*p0 )
    return 0;
  returnVal = 0;
  do
  {
    intVal = *int16Ptr++;
    ++returnVal;
  }
  while ( intVal );
  return returnVal;
}


float cocos2d::ccpAngle(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1)
{
  float returnVal;
  float returnVal2;
  float value;
  float value2;
  float value3;
  float value4;

  cocos2d::ccpNormalize(p0);
  cocos2d::ccpNormalize(p1);
  returnVal = acosf((float)(value2 * value4) + (float)(value * value3));
  returnVal2 = 0.0;
  if ( fabsf(returnVal) >= 0.00000011921 )
    return returnVal;
  return returnVal2;
}


cocos2d::CCPoint cocos2d::ccpClamp(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1, const cocos2d::CCPoint *p2)
{
  cocos2d::CCPoint *point;
  cocos2d::CCPoint *point2;
  float value;
  float value2;
  float value3;
  float value4;
  cocos2d::CCPoint returnVal;

  point2 = point;
  value = cocos2d::clampf(p0->x, p1->x, p2->x);
  value2 = cocos2d::clampf(p0->y, p1->y, p2->y);
  cocos2d::CCPoint::CCPoint(point2, value, value2);
  returnVal.y = value4;
  returnVal.x = value3;
  return returnVal;
}


cocos2d::CCPoint cocos2d::ccpCompMult(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1)
{
  cocos2d::CCPoint *point;
  float value;
  float value2;
  cocos2d::CCPoint returnVal;

  cocos2d::CCPoint::CCPoint(point, p0->x * p1->x, p0->y * p1->y);
  returnVal.y = value2;
  returnVal.x = value;
  return returnVal;
}


float cocos2d::ccpDistance(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1)
{
  float value;
  float value2;

  cocos2d::CCPoint::operator-(p0, p1);
  return sqrtf((float)(value2 * value2) + (float)(value * value));
}


cocos2d::CCPoint cocos2d::ccpForAngle(float p0)
{
  cocos2d::CCPoint *point;
  cocos2d::CCPoint *point2;
  float value;
  float value2;
  float value3;
  float value4;
  cocos2d::CCPoint returnVal;

  point2 = point;
  value = cosf(p0);
  value2 = sinf(p0);
  cocos2d::CCPoint::CCPoint(point2, value, value2);
  returnVal.y = value4;
  returnVal.x = value3;
  return returnVal;
}


cocos2d::CCPoint cocos2d::ccpFromSize(const cocos2d::CCSize *size)
{
  _DWORD *dwordPtr;
  float value;
  float value2;
  cocos2d::CCPoint returnVal;

  cocos2d::CCPoint::CCPoint(dwordPtr, (int *)size);
  returnVal.y = value2;
  returnVal.x = value;
  return returnVal;
}


float cocos2d::ccpLength(const cocos2d::CCPoint *p0)
{
  return sqrtf((float)(p0->y * p0->y) + (float)(p0->x * p0->x));
}


cocos2d::CCPoint cocos2d::ccpLerp(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1, float p2)
{
  cocos2d::CCPoint *point;
  cocos2d::CCPoint *point2;
  float value;
  float value2;
  cocos2d::CCPoint point3;
  cocos2d::CCPoint point4;
  cocos2d::CCPoint returnVal;

  point2 = point;
  cocos2d::CCPoint::operator*((float *)p0, &point4, 1.0 - p2);
  cocos2d::CCPoint::operator*((float *)p1, &point3, p2);
  cocos2d::CCPoint::operator+((float *)&point4, (float *)&point3, point2);
  returnVal.y = value2;
  returnVal.x = value;
  return returnVal;
}


cocos2d::CCPoint cocos2d::ccpNormalize(const cocos2d::CCPoint *p0)
{
  cocos2d::CCPoint *point;
  float value;
  float value2;
  float value3;
  cocos2d::CCPoint returnVal;

  value = sqrtf((float)(p0->y * p0->y) + (float)(p0->x * p0->x));
  if ( value == 0.0 )
    cocos2d::CCPoint::CCPoint(point, 1.0, 0.0);
  else
    cocos2d::CCPoint::operator/((float *)p0, point, value);
  returnVal.y = value3;
  returnVal.x = value2;
  return returnVal;
}


cocos2d::CCPoint cocos2d::ccpRotateByAngle(const cocos2d::CCPoint *p0, const cocos2d::CCPoint *p1, float p2)
{
  cocos2d::CCPoint *point;
  float value;
  float value2;
  cocos2d::CCPoint returnVal;

  cocos2d::CCPoint::rotateByAngle((cocos2d::CCPoint *)p0, (cocos2d::CCPoint *)p1, p2, point);
  returnVal.y = value2;
  returnVal.x = value;
  return returnVal;
}


float cocos2d::ccpToAngle(const cocos2d::CCPoint *point)
{
  return atan2f(point->y, point->x);
}


float cocos2d::clampf(float p0, float p1, float p2)
{
  float value;

  if ( p1 > p2 )
  {
    value = p1;
    p1 = p2;
    p2 = value;
  }
  if ( p1 <= p0 && (p1 = p0, p2 <= p0) )
    return p2;
  else
    return p1;
}


const char * cocos2d::cocos2dVersion()
{
  return "2.2.3";
}


_QWORD * cocos2d::fill_fopen64_filefunc(_QWORD *qwordPtr)
{
  qwordPtr[7] = 0;
  *qwordPtr = sub_B2C040;
  qwordPtr[1] = sub_B2BFDC;
  qwordPtr[2] = sub_B2BFC4;
  qwordPtr[3] = sub_B2BFBC;
  qwordPtr[4] = sub_B2BF6C;
  qwordPtr[5] = sub_B2BF1C;
  qwordPtr[6] = sub_B2BF14;
  return qwordPtr;
}


_QWORD * cocos2d::fill_fopen_filefunc(_QWORD *qwordPtr)
{
  qwordPtr[7] = 0;
  *qwordPtr = sub_B2BFF4;
  qwordPtr[1] = sub_B2BFDC;
  qwordPtr[2] = sub_B2BFC4;
  qwordPtr[3] = sub_B2BFB4;
  qwordPtr[4] = sub_B2BF24;
  qwordPtr[5] = sub_B2BF1C;
  qwordPtr[6] = sub_B2BF14;
  return qwordPtr;
}


_QWORD * cocos2d::fill_zlib_filefunc64_32_def_from_filefunc32(_QWORD *qwordPtr, __int64 *int64Ptr)
{
  __int64 int64Val;
  __int64 int64Val2;
  __int64 int64Val3;
  __int64 int64Val4;
  __int64 int64Val5;
  __int64 int64Val6;
  __int64 int64Val7;

  int64Val = *int64Ptr;
  int64Val2 = int64Ptr[6];
  int64Val3 = int64Ptr[1];
  int64Val4 = int64Ptr[2];
  int64Val5 = int64Ptr[5];
  int64Val6 = int64Ptr[7];
  int64Val7 = int64Ptr[4];
  qwordPtr[9] = int64Ptr[3];
  *qwordPtr = 0;
  qwordPtr[8] = int64Val;
  qwordPtr[6] = int64Val2;
  qwordPtr[1] = int64Val3;
  qwordPtr[2] = int64Val4;
  qwordPtr[3] = 0;
  qwordPtr[4] = 0;
  qwordPtr[5] = int64Val5;
  qwordPtr[7] = int64Val6;
  qwordPtr[10] = int64Val7;
  return qwordPtr;
}


__int64 cocos2d::getCharacterCountInUTF8String(const char **charPtr)
{
  return (int)getUTF8StringLength(*charPtr);
}


bool cocos2d::iscjk_unicode()
{
  __int16 int16Val;
  __int16 int16Val2;
  _BOOL8 returnVal;

  int16Val = (__int16)this;
  int16Val2 = (_WORD)this - 0x2000;
  returnVal = ((unsigned __int16)((_WORD)this - 19968) <= 0x51BFu)
         | (unsigned int)((unsigned __int16)((_WORD)this - 11904) <= 0x15Fu);
  if ( !returnVal )
  {
    returnVal = 1;
    if ( (unsigned __int16)(int16Val2 - 4080) > 0x1CFu
      && (unsigned __int16)(int16Val + 21504) > 0x2BAFu
      && (unsigned __int16)(int16Val + 1792) > 0x1FFu
      && (unsigned __int16)(int16Val + 464) > 0x1Fu )
    {
      return (unsigned __int16)(int16Val - 12736) <= 0x1C3Fu;
    }
  }
  return returnVal;
}


bool cocos2d::isspace_unicode(unsigned __int16 p0)
{
  int intVal;
  _BOOL4 returnVal;

  intVal = p0;
  returnVal = (unsigned __int16)(p0 - 9) <= 4u || (p0 & 0xFF7F) == 32;
  if ( !returnVal )
  {
    returnVal = intVal == 133 || intVal == 5760;
    if ( !returnVal )
    {
      LOBYTE(returnVal) = 1;
      if ( (unsigned __int16)(intVal - 0x2000) > 0xAu && (unsigned __int16)(intVal - 8232) > 1u )
        LOBYTE(returnVal) = intVal == 12288 || intVal == 8287 || intVal == 8239;
    }
  }
  return returnVal;
}


__int64 cocos2d::qChildSort(__int64 *int64Ptr, const void *voidPtr)
{
  __int64 int64Val;
  __int64 int64Val2;
  int intVal;
  int intVal2;
  int intVal3;

  int64Val = *(_QWORD *)this;
  int64Val2 = *int64Ptr;
  intVal = (*(__int64 (__fastcall **)(_QWORD))(**(_QWORD **)this + 104LL))(*(_QWORD *)this);
  intVal2 = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)int64Val2 + 104LL))(int64Val2);
  if ( intVal != intVal2 )
    return (unsigned int)(intVal - intVal2);
  intVal3 = (*(__int64 (__fastcall **)(__int64))(*(_QWORD *)int64Val + 400LL))(int64Val) + intVal;
  return intVal3 - ((*(unsigned int (__fastcall **)(__int64))(*(_QWORD *)int64Val2 + 400LL))(int64Val2) + intVal2);
}


void cocos2d::tgaDestroy(_QWORD *p)
{
  void *voidPtr;

  if ( p )
  {
    voidPtr = (void *)p[2];
    if ( voidPtr )
      free(voidPtr);
    free(p);
  }
}


void cocos2d::tgaFlipImage(__int64 int64Val)
{
  int intVal;
  void *voidPtr;
  int intVal2;
  __int64 int64Val2;
  int intVal3;
  __int64 int64Val3;
  int intVal4;
  void *voidPtr2;

  intVal = *(__int16 *)(int64Val + 6) * (*(unsigned __int8 *)(int64Val + 5) >> 3);
  voidPtr = malloc(intVal);
  if ( voidPtr )
  {
    intVal2 = *(__int16 *)(int64Val + 8);
    int64Val2 = 0;
    intVal3 = 0;
    if ( intVal2 > 1 )
    {
      do
      {
        int64Val3 = *(_QWORD *)(int64Val + 16);
        intVal4 = ~intVal3++;
        memcpy(voidPtr, (const void *)(int64Val3 + int64Val2), intVal);
        voidPtr2 = (void *)(*(_QWORD *)(int64Val + 16) + int64Val2);
        int64Val2 += intVal;
        memcpy(voidPtr2, (const void *)(int64Val3 + (intVal2 + intVal4) * intVal), intVal);
        memcpy((void *)(*(_QWORD *)(int64Val + 16) + (intVal4 + *(__int16 *)(int64Val + 8)) * intVal), voidPtr, intVal);
        intVal2 = *(__int16 *)(int64Val + 8);
      }
      while ( *(__int16 *)(int64Val + 8) / 2 > intVal3 );
    }
    free(voidPtr);
    *(_DWORD *)(int64Val + 24) = 0;
  }
}


_QWORD * cocos2d::tgaLoad(const char *charPtr)
{
  cocos2d::CCFileUtils *cCFileUtils;
  unsigned __int8 *int8Ptr;
  _QWORD *returnVal;
  int intVal;
  unsigned int intVal2;
  void *voidPtr;
  int intVal3;
  unsigned int intVal4;
  unsigned __int64 int64Val;
  int intVal5;
  int intVal6;
  unsigned __int64 int64Val2;
  char charVal;
  unsigned __int64 int64Val3;
  int intVal7;
  int intVal8;
  unsigned __int64 int64Val4;
  _BYTE v21[2];
  char charVal2;

  int64Val4 = 0;
  cCFileUtils = cocos2d::CCFileUtils::sharedFileUtils();
  int8Ptr = (unsigned __int8 *)((__int64 (__fastcall *)(cocos2d::CCFileUtils *, cocos2d *, char *, unsigned __int64 *))cCFileUtils->addSearchResolutionsOrder)(
                            cCFileUtils,
                            this,
                            "rb",
                            &int64Val4);
  if ( !int8Ptr )
    return 0;
  returnVal = malloc(0x20u);
  if ( (unsigned __int8)cocos2d::tgaLoadHeader((__int64)int8Ptr, int64Val4, (__int64)returnVal) )
  {
    intVal = *((unsigned __int8 *)returnVal + 4);
    if ( intVal == 1 )
    {
      *(_DWORD *)returnVal = 3;
      goto LABEL_4;
    }
    if ( (unsigned __int8)(intVal - 2) > 1u && intVal != 10 )
    {
      *(_DWORD *)returnVal = 5;
      goto LABEL_4;
    }
    intVal2 = *((unsigned __int8 *)returnVal + 5) >> 3;
    intVal8 = *((__int16 *)returnVal + 4) * *((__int16 *)returnVal + 3);
    voidPtr = malloc((int)(intVal8 * intVal2));
    returnVal[2] = voidPtr;
    if ( voidPtr )
    {
      if ( intVal != 10 )
      {
        if ( !(unsigned __int8)cocos2d::tgaLoadImageData((__int64)int8Ptr, int64Val4, (__int64)returnVal) )
        {
          *(_DWORD *)returnVal = 2;
          goto LABEL_4;
        }
        goto LABEL_20;
      }
      int64Val3 = int64Val4;
      if ( !intVal8 )
      {
LABEL_20:
        intVal6 = *((_DWORD *)returnVal + 6);
        *(_DWORD *)returnVal = 0;
        if ( !intVal6 )
          goto LABEL_4;
        cocos2d::tgaFlipImage((__int64)returnVal);
        if ( !*((_DWORD *)returnVal + 6) )
          goto LABEL_4;
        goto LABEL_3;
      }
      intVal3 = 0;
      intVal4 = 0;
      int64Val = 18;
      intVal7 = 0;
      intVal5 = 0;
      while ( 1 )
      {
        if ( intVal5 )
        {
          intVal5 = (unsigned __int8)(intVal5 - 1);
          if ( intVal7 )
            goto LABEL_19;
          int64Val2 = int64Val;
        }
        else
        {
          int64Val2 = int64Val + 1;
          if ( int64Val3 < int64Val + 1 )
            goto LABEL_20;
          intVal5 = int8Ptr[int64Val];
          intVal7 = 0;
          if ( (intVal5 & 0x80) != 0 )
          {
            intVal7 = intVal5 & 0x80;
            intVal5 = (unsigned __int8)(intVal5 + 0x80);
          }
        }
        int64Val = int64Val2 + (unsigned __int8)intVal2;
        if ( int64Val3 < int64Val )
          goto LABEL_20;
        memcpy(v21, &int8Ptr[int64Val2], (unsigned __int8)intVal2);
        if ( intVal2 > 2 )
        {
          charVal = v21[0];
          v21[0] = charVal2;
          charVal2 = charVal;
        }
LABEL_19:
        memcpy((void *)(returnVal[2] + intVal4), v21, (unsigned __int8)intVal2);
        ++intVal3;
        intVal4 += intVal2;
        if ( intVal8 == intVal3 )
          goto LABEL_20;
      }
    }
  }
LABEL_3:
  *(_DWORD *)returnVal = 4;
LABEL_4:
  operator delete[](int8Ptr);
  return returnVal;
}


__int64 cocos2d::tgaLoadHeader(__int64 int64Val, unsigned __int64 int64Val2, __int64 int64Val3)
{
  __int64 returnVal;

  if ( int64Val2 <= 2 )
    return 0;
  *(_BYTE *)(int64Val3 + 4) = *(_BYTE *)(int64Val + 2);
  if ( int64Val2 <= 0x10 )
    return 0;
  *(_WORD *)(int64Val3 + 6) = *(_WORD *)(int64Val + 12);
  *(_WORD *)(int64Val3 + 8) = *(_WORD *)(int64Val + 14);
  *(_BYTE *)(int64Val3 + 5) = *(_BYTE *)(int64Val + 16);
  if ( int64Val2 == 17 )
    return 0;
  if ( (*(_BYTE *)(int64Val + 17) & 0x20) != 0 )
  {
    returnVal = 1;
    *(_DWORD *)(int64Val3 + 24) = 1;
  }
  else
  {
    *(_DWORD *)(int64Val3 + 24) = 0;
    return 1;
  }
  return returnVal;
}


__int64 cocos2d::tgaLoadImageData(__int64 int64Val, unsigned __int64 int64Val2, __int64 int64Val3)
{
  unsigned int intVal;
  int intVal2;
  __int64 int64Val4;
  int intVal3;
  __int64 int64Val5;
  __int64 int64Val6;
  char charVal;

  intVal = *(unsigned __int8 *)(int64Val3 + 5) >> 3;
  intVal2 = *(__int16 *)(int64Val3 + 8) * *(__int16 *)(int64Val3 + 6) * intVal;
  if ( intVal2 + 18LL > int64Val2 )
    return 0;
  memcpy(*(void **)(int64Val3 + 16), (const void *)(int64Val + 18), intVal2);
  if ( intVal > 2 && intVal2 > 0 )
  {
    int64Val4 = 0;
    intVal3 = 0;
    do
    {
      int64Val5 = *(_QWORD *)(int64Val3 + 16);
      int64Val6 = int64Val4 + 2;
      intVal3 += intVal;
      charVal = *(_BYTE *)(int64Val5 + int64Val4);
      *(_BYTE *)(int64Val5 + int64Val4) = *(_BYTE *)(int64Val5 + int64Val4 + 2);
      int64Val4 += (unsigned __int8)intVal;
      *(_BYTE *)(*(_QWORD *)(int64Val3 + 16) + int64Val6) = charVal;
    }
    while ( intVal2 > intVal3 );
  }
  return 1;
}


unsigned __int8 * cocos2d::tgaRGBtogreyscale(unsigned __int8 *int8Ptr)
{
  unsigned int intVal;
  unsigned __int8 *int8Ptr2;
  unsigned int intVal2;
  int intVal3;
  unsigned __int8 *int8Ptr3;
  unsigned __int8 *int8Ptr4;
  unsigned __int8 *int8Ptr42;
  __int64 int64Val;
  int intVal4;
  int intVal5;
  int intVal6;

  intVal = int8Ptr[5];
  if ( intVal != 8 )
  {
    int8Ptr2 = int8Ptr;
    intVal2 = intVal >> 3;
    intVal3 = *((__int16 *)int8Ptr + 4) * *((__int16 *)int8Ptr + 3);
    int8Ptr = (unsigned __int8 *)malloc(intVal3);
    int8Ptr3 = int8Ptr;
    if ( int8Ptr )
    {
      if ( intVal3 <= 0 )
      {
        int8Ptr4 = (unsigned __int8 *)*((_QWORD *)int8Ptr2 + 2);
      }
      else
      {
        int8Ptr4 = (unsigned __int8 *)*((_QWORD *)int8Ptr2 + 2);
        int8Ptr42 = int8Ptr4;
        int64Val = 0;
        do
        {
          intVal4 = int8Ptr42[1];
          intVal5 = *int8Ptr42;
          intVal6 = int8Ptr42[2];
          int8Ptr42 += (unsigned __int8)intVal2;
          int8Ptr3[int64Val++] = (unsigned int)((double)intVal4 * 0.59 + (double)intVal5 * 0.3 + (double)intVal6 * 0.11);
        }
        while ( intVal3 > (int)int64Val );
      }
      free(int8Ptr4);
      *((_QWORD *)int8Ptr2 + 2) = int8Ptr3;
      int8Ptr2[5] = 8;
      int8Ptr = (_BYTE *)(dword_0 + 3);
      int8Ptr2[4] = 3;
    }
  }
  return int8Ptr;
}


__int64 cocos2d::unzClose(void *voidPtr)
{
  void *voidPtr2;

  if ( !this )
    return 4294967194LL;
  voidPtr2 = (void *)*((_QWORD *)this + 40);
  if ( voidPtr2 )
    unzCloseCurrentFile(voidPtr2);
  (*((void (__fastcall **)(_QWORD, _QWORD))this + 5))(*((_QWORD *)this + 7), *((_QWORD *)this + 12));
  free(this);
  return 0;
}


__int64 cocos2d::unzCloseCurrentFile(void *voidPtr)
{
  __int64 int64Val;
  unsigned int returnVal;
  __int64 int64Val2;

  if ( !this )
    return 4294967194LL;
  int64Val = *((_QWORD *)this + 40);
  if ( !int64Val )
    return 4294967194LL;
  returnVal = 0;
  if ( !*(_QWORD *)(int64Val + 192) && !*(_DWORD *)(int64Val + 312) )
  {
    if ( *(_QWORD *)(int64Val + 168) == *(_QWORD *)(int64Val + 176) )
      returnVal = 0;
    else
      returnVal = -105;
  }
  if ( *(_QWORD *)int64Val )
    free(*(void **)int64Val);
  int64Val2 = *(_QWORD *)(int64Val + 128);
  *(_QWORD *)int64Val = 0;
  if ( int64Val2 == 8 )
    inflateEnd((_QWORD *)(int64Val + 8));
  free((void *)int64Val);
  *((_QWORD *)this + 40) = 0;
  return returnVal;
}


__int64 cocos2d::unzGetCurrentFileZStreamPos64(void *voidPtr)
{
  __int64 int64Val;

  if ( this && (int64Val = *((_QWORD *)this + 40)) != 0 )
    return *(_QWORD *)(int64Val + 304) + *(_QWORD *)(int64Val + 120);
  else
    return 0;
}


__int64 cocos2d::unzGetFilePos(_QWORD *qwordPtr, _QWORD *qwordPtr2)
{
  __int64 returnVal;
  _QWORD v4[2];

  returnVal = cocos2d::unzGetFilePos64(qwordPtr, v4);
  if ( !(_DWORD)returnVal )
  {
    *qwordPtr2 = v4[0];
    qwordPtr2[1] = v4[1];
  }
  return returnVal;
}


__int64 cocos2d::unzGetFilePos64(_QWORD *qwordPtr, _QWORD *qwordPtr2)
{
  __int64 int64Val;
  unsigned int returnVal;

  if ( qwordPtr && qwordPtr2 )
  {
    if ( qwordPtr[18] )
    {
      int64Val = qwordPtr[17];
      returnVal = 0;
      qwordPtr2[1] = qwordPtr[16];
      *qwordPtr2 = int64Val;
    }
    else
    {
      return (unsigned int)-100;
    }
  }
  else
  {
    return (unsigned int)-102;
  }
  return returnVal;
}


__int64 cocos2d::unzGetGlobalComment(_BYTE *bytePtr, char *charPtr)
{
  char *charPtr2;
  unsigned __int64 int64Val;

  if ( !this )
    return 4294967194LL;
  charPtr2 = (char *)*((_QWORD *)this + 14);
  if ( charPtr <= charPtr2 )
    charPtr2 = charPtr;
  if ( !cocos2d::call_zseek64((__int64)this) )
  {
    if ( !charPtr2 )
    {
      if ( !bytePtr )
        return (unsigned int)charPtr2;
LABEL_7:
      int64Val = *((_QWORD *)this + 14);
      if ( (unsigned __int64)charPtr > int64Val )
        bytePtr[int64Val] = 0;
      return (unsigned int)charPtr2;
    }
    *bytePtr = 0;
    if ( (char *)(*((__int64 (__fastcall **)(_QWORD, _QWORD, _BYTE *, char *))this + 1))(
                   *((_QWORD *)this + 7),
                   *((_QWORD *)this + 12),
                   bytePtr,
                   charPtr2) == charPtr2 )
      goto LABEL_7;
  }
  return 0xFFFFFFFFLL;
}


__int64 cocos2d::unzGetGlobalInfo(__int64 int64Val, _QWORD *qwordPtr)
{
  __int64 int64Val2;
  unsigned int returnVal;

  if ( int64Val )
  {
    int64Val2 = *(_QWORD *)(int64Val + 104);
    returnVal = 0;
    qwordPtr[1] = *(_QWORD *)(int64Val + 112);
    *qwordPtr = int64Val2;
  }
  else
  {
    return (unsigned int)-102;
  }
  return returnVal;
}


__int64 cocos2d::unzGetGlobalInfo64(__int64 int64Val, _QWORD *qwordPtr)
{
  _QWORD *qwordPtr2;
  __int64 returnVal;
  __int64 int64Val2;

  if ( !int64Val )
    return 4294967194LL;
  qwordPtr2 = (_QWORD *)(int64Val + 104);
  returnVal = 0;
  int64Val2 = qwordPtr2[1];
  *qwordPtr = *qwordPtr2;
  qwordPtr[1] = int64Val2;
  return returnVal;
}


unsigned __int64 cocos2d::unzGetLocalExtrafield(void *voidPtr, void *voidPtr2)
{
  unsigned int returnVal;
  __int64 int64Val;
  unsigned __int64 returnVal2;

  returnVal = (unsigned int)voidPtr2;
  if ( !this )
    return 4294967194LL;
  int64Val = *((_QWORD *)this + 40);
  if ( !int64Val )
    return 4294967194LL;
  returnVal2 = *(unsigned int *)(int64Val + 144) - *(_QWORD *)(int64Val + 152);
  if ( voidPtr )
  {
    if ( returnVal2 < (unsigned int)voidPtr2 )
      returnVal = *(_DWORD *)(int64Val + 144) - *(_DWORD *)(int64Val + 152);
    returnVal2 = 0;
    if ( returnVal )
    {
      if ( cocos2d::call_zseek64(int64Val + 200)
        || returnVal != (*(__int64 (__fastcall **)(_QWORD, _QWORD, void *, _QWORD))(int64Val + 208))(
                   *(_QWORD *)(int64Val + 256),
                   *(_QWORD *)(int64Val + 288),
                   voidPtr,
                   returnVal) )
      {
        return 0xFFFFFFFFLL;
      }
      else
      {
        return returnVal;
      }
    }
  }
  return returnVal2;
}


cocos2d * cocos2d::unzGetOffset(void *voidPtr)
{
  if ( this )
    return (cocos2d *)unzGetOffset64(voidPtr);
  return this;
}


__int64 cocos2d::unzGetOffset64(void *voidPtr)
{
  __int64 returnVal;
  __int64 int64Val;

  if ( this )
  {
    returnVal = *((_QWORD *)this + 18);
    if ( !returnVal )
      return returnVal;
    int64Val = *((_QWORD *)this + 13);
    if ( int64Val == 0xFFFF || !int64Val || int64Val != *((_QWORD *)this + 16) )
      return *((_QWORD *)this + 17);
  }
  return 0;
}


__int64 cocos2d::unzGoToFilePos(_QWORD *qwordPtr, _QWORD *qwordPtr2)
{
  __int64 int64Val;
  _QWORD v4[2];

  if ( !qwordPtr2 )
    return 4294967194LL;
  int64Val = qwordPtr2[1];
  v4[0] = *qwordPtr2;
  v4[1] = int64Val;
  return cocos2d::unzGoToFilePos64(qwordPtr, v4);
}


__int64 cocos2d::unzGoToFilePos64(_QWORD *qwordPtr, _QWORD *qwordPtr2)
{
  __int64 int64Val;
  __int64 returnVal;

  if ( !qwordPtr || !qwordPtr2 )
    return 4294967194LL;
  int64Val = qwordPtr2[1];
  qwordPtr[17] = *qwordPtr2;
  qwordPtr[16] = int64Val;
  returnVal = sub_B2C634((__int64)qwordPtr, qwordPtr + 22, qwordPtr + 39, 0, 0, 0, 0, 0, 0);
  qwordPtr[18] = (_DWORD)returnVal == 0;
  return returnVal;
}


__int64 cocos2d::unzGoToFirstFile(void *voidPtr)
{
  return unzGoToFirstFile64(0, 0, 0);
}


__int64 cocos2d::unzGoToFirstFile64(_QWORD *qwordPtr, void *voidPtr, __int64 int64Val, unsigned __int64 int64Val2)
{
  const void *voidPtr2;
  unsigned int returnVal;

  if ( !qwordPtr )
    return 4294967194LL;
  qwordPtr[17] = qwordPtr[21];
  voidPtr2 = qwordPtr + 22;
  qwordPtr[16] = 0;
  returnVal = sub_B2C634((__int64)qwordPtr, qwordPtr + 22, qwordPtr + 39, int64Val, int64Val2, 0, 0, 0, 0);
  qwordPtr[18] = returnVal == 0;
  if ( !voidPtr )
    return returnVal;
  memcpy(voidPtr, voidPtr2, 0x88u);
  return returnVal;
}


__int64 cocos2d::unzGoToNextFile(void *voidPtr)
{
  return unzGoToNextFile64(0, 0, 0);
}


__int64 cocos2d::unzGoToNextFile64(_QWORD *qwordPtr, void *voidPtr, __int64 int64Val, unsigned __int64 int64Val2)
{
  __int64 int64Val3;
  __int64 int64Val4;
  __int64 int64Val5;
  __int64 int64Val6;
  __int64 int64Val7;
  unsigned int returnVal;

  if ( !qwordPtr )
    return 4294967194LL;
  if ( !qwordPtr[18] )
    return 4294967196LL;
  int64Val3 = qwordPtr[13];
  if ( int64Val3 != 0xFFFF )
  {
    int64Val4 = qwordPtr[16] + 1LL;
    if ( int64Val3 != int64Val4 )
      goto LABEL_5;
    return 4294967196LL;
  }
  int64Val4 = qwordPtr[16] + 1LL;
LABEL_5:
  int64Val5 = qwordPtr[31];
  int64Val6 = qwordPtr[32];
  int64Val7 = qwordPtr[17] + qwordPtr[30] + 46LL;
  qwordPtr[16] = int64Val4;
  qwordPtr[17] = int64Val7 + int64Val5 + int64Val6;
  returnVal = sub_B2C634((__int64)qwordPtr, qwordPtr + 22, qwordPtr + 39, int64Val, int64Val2, 0, 0, 0, 0);
  qwordPtr[18] = returnVal == 0;
  if ( !voidPtr )
    return returnVal;
  memcpy(voidPtr, qwordPtr + 22, 0x88u);
  return returnVal;
}


__int64 cocos2d::unzLocateFile(char *charPtr, const char *charPtr2)
{
  unsigned int intVal;
  __int64 int64Val;
  __int64 int64Val2;
  __int64 int64Val3;
  void *voidPtr;
  unsigned int i;
  __int64 returnVal;
  void *voidPtr2;
  unsigned int returnVal2;
  _BYTE v14[136];
  _BYTE v15[264];

  intVal = (unsigned int)charPtr2;
  if ( !this || strlen(charPtr) > 0xFF )
    return 4294967194LL;
  if ( !*((_QWORD *)this + 18) )
    return 4294967196LL;
  int64Val = *((_QWORD *)this + 16);
  int64Val2 = *((_QWORD *)this + 17);
  memcpy(v14, (char *)this + 176, sizeof(v14));
  int64Val3 = *((_QWORD *)this + 39);
  for ( i = unzGoToFirstFile(voidPtr); !i; i = unzGoToNextFile(voidPtr2) )
  {
    i = cocos2d::unzGetCurrentFileInfo64((__int64)this, 0, (__int64)v15, 0x100u, 0, 0, 0, 0);
    if ( i )
      break;
    returnVal = cocos2d::unzStringFileNameCompare((cocos2d *)v15, charPtr, (const char *)intVal);
    if ( !(_DWORD)returnVal )
      return returnVal;
  }
  *((_QWORD *)this + 16) = int64Val;
  *((_QWORD *)this + 17) = int64Val2;
  returnVal2 = i;
  memcpy((char *)this + 176, v14, 0x88u);
  *((_QWORD *)this + 39) = int64Val3;
  return returnVal2;
}


__int64 cocos2d::unzOpen(const char *charPtr)
{
  return sub_B2D010((__int64)this, 0, 0);
}


__int64 cocos2d::unzOpen2(__int64 int64Val, __int64 *int64Ptr)
{
  __int64 v4[11];

  if ( !int64Ptr )
    return sub_B2D010(int64Val, 0, 0);
  cocos2d::fill_zlib_filefunc64_32_def_from_filefunc32(v4, int64Ptr);
  return sub_B2D010(int64Val, v4, 0);
}


__int64 cocos2d::unzOpen2_64(__int64 int64Val, __int64 *int64Ptr)
{
  __int64 int64Val2;
  __int64 int64Val3;
  __int64 int64Val4;
  __int64 int64Val5;
  __int64 int64Val6;
  __int64 int64Val7;
  __int64 int64Val8;
  __int64 int64Val9;
  __int64 v11[11];

  if ( !int64Ptr )
    return sub_B2D010(int64Val, 0, 1);
  int64Val2 = *int64Ptr;
  int64Val3 = int64Ptr[1];
  int64Val4 = int64Ptr[2];
  int64Val5 = int64Ptr[3];
  int64Val6 = int64Ptr[4];
  int64Val7 = int64Ptr[5];
  int64Val8 = int64Ptr[6];
  int64Val9 = int64Ptr[7];
  v11[9] = 0;
  v11[10] = 0;
  v11[0] = int64Val2;
  v11[1] = int64Val3;
  v11[2] = int64Val4;
  v11[3] = int64Val5;
  v11[4] = int64Val6;
  v11[5] = int64Val7;
  v11[6] = int64Val8;
  v11[7] = int64Val9;
  return sub_B2D010(int64Val, v11, 1);
}


__int64 cocos2d::unzOpen64(const void *voidPtr)
{
  return sub_B2D010((__int64)this, 0, 1);
}


__int64 cocos2d::unzOpenCurrentFile2(_DWORD *dwordPtr, int *intPtr, int *intPtr2, int intVal, const char *charPtr)
{
  return unzOpenCurrentFile3(dwordPtr, intPtr, intPtr2, 0, charPtr);
}


__int64 cocos2d::unzReadCurrentFile(__int64 int64Val, void *voidPtr)
{
  __int64 *int64Ptr;
  __int64 returnVal;
  unsigned __int64 int64Val2;
  __int64 int64Val3;
  unsigned int intVal;
  unsigned int intVal2;
  unsigned int returnVal2;
  unsigned int intVal3;
  unsigned __int64 int64Val4;
  unsigned int intVal4;
  __int64 int64Val5;
  __int64 int64Val6;
  __int64 int64Val7;
  __int64 intVal22;
  char *charPtr;
  __int64 int64Val8;
  unsigned int intVal5;
  __int64 int64Val9;
  __int64 int64Val10;
  __int64 int64Val11;
  __int64 int64Val12;
  __int64 int64Val13;
  char *charPtr2;
  unsigned int returnVal3;
  __int64 int64Val14;
  __int64 int64Val15;

  if ( !this )
    return 4294967194LL;
  int64Ptr = (__int64 *)*((_QWORD *)this + 40);
  if ( !int64Ptr )
    return 4294967194LL;
  if ( !*int64Ptr )
    return 4294967196LL;
  returnVal = 0;
  if ( (_DWORD)voidPtr )
  {
    int64Val2 = int64Ptr[24];
    int64Ptr[4] = int64Val;
    *((_DWORD *)int64Ptr + 10) = (_DWORD)voidPtr;
    if ( (unsigned int)voidPtr > int64Val2 )
    {
      if ( *((_DWORD *)int64Ptr + 78) )
      {
        int64Val3 = int64Ptr[23];
        intVal = *((_DWORD *)int64Ptr + 4);
        if ( int64Val3 + (unsigned __int64)intVal >= (unsigned int)voidPtr )
        {
          intVal2 = (unsigned int)voidPtr;
LABEL_12:
          returnVal2 = 0;
          while ( 2 )
          {
            if ( !intVal2 )
              return returnVal2;
            while ( 1 )
            {
              intVal3 = *((_DWORD *)int64Ptr + 4);
              if ( !intVal3 )
                break;
              int64Val6 = int64Ptr[37];
              if ( int64Val6 )
                goto LABEL_22;
LABEL_32:
              if ( intVal2 > intVal3 )
                intVal2 = intVal3;
              if ( intVal2 )
              {
                int64Val12 = 0;
                do
                {
                  *(_BYTE *)(int64Ptr[4] + int64Val12) = *(_BYTE *)(int64Ptr[1] + int64Val12);
                  ++int64Val12;
                }
                while ( intVal2 > (unsigned int)int64Val12 );
              }
LABEL_26:
              intVal22 = intVal2;
              charPtr = (char *)int64Ptr[4];
              int64Val8 = int64Ptr[21];
              int64Ptr[20] += intVal2;
              returnVal2 += intVal2;
              int64Ptr[21] = crc32(int64Val8, charPtr, intVal2);
              intVal5 = *((_DWORD *)int64Ptr + 4) - intVal2;
              intVal2 = *((_DWORD *)int64Ptr + 10) - intVal2;
              int64Val9 = int64Ptr[6];
              int64Val10 = int64Ptr[4] + intVal22;
              int64Val11 = int64Ptr[1] + intVal22;
              int64Ptr[24] -= intVal22;
              *((_DWORD *)int64Ptr + 4) = intVal5;
              *((_DWORD *)int64Ptr + 10) = intVal2;
              int64Ptr[4] = int64Val10;
              int64Ptr[1] = int64Val11;
              int64Ptr[6] = int64Val9 + intVal22;
              if ( !intVal2 )
                return returnVal2;
            }
            int64Val4 = int64Ptr[23];
            if ( int64Val4 )
            {
              if ( int64Val4 > 0x3FFF )
                intVal4 = 0x4000;
              else
                intVal4 = int64Ptr[23];
              if ( cocos2d::call_zseek64((__int64)(int64Ptr + 25))
                || intVal4 != ((__int64 (__fastcall *)(__int64, __int64, _QWORD, _QWORD))int64Ptr[26])(int64Ptr[32], int64Ptr[36], *int64Ptr, intVal4) )
              {
                return 0xFFFFFFFFLL;
              }
              int64Val5 = *int64Ptr;
              int64Val6 = int64Ptr[37];
              int64Val7 = int64Ptr[23] - intVal4;
              int64Ptr[15] += intVal4;
              int64Ptr[23] = int64Val7;
              int64Ptr[1] = int64Val5;
              *((_DWORD *)int64Ptr + 4) = intVal4;
              if ( !int64Val6 )
              {
                intVal3 = intVal4;
LABEL_39:
                intVal2 = *((_DWORD *)int64Ptr + 10);
                goto LABEL_32;
              }
            }
            else
            {
              int64Val6 = int64Ptr[37];
              if ( !int64Val6 )
                return returnVal2;
            }
LABEL_22:
            if ( !*((_DWORD *)int64Ptr + 78) )
            {
              if ( int64Val6 == 12 )
              {
LABEL_48:
                intVal2 = *((_DWORD *)int64Ptr + 10);
                continue;
              }
              int64Val13 = int64Ptr[6];
              charPtr2 = (char *)int64Ptr[4];
              returnVal3 = inflate((__int64)(int64Ptr + 1), 2u);
              if ( (returnVal3 & 0x80000000) == 0 && int64Ptr[7] )
                returnVal3 = -3;
              int64Val14 = int64Ptr[6];
              int64Val15 = int64Ptr[21];
              int64Ptr[20] += int64Val14 - int64Val13;
              returnVal2 += int64Val14 - int64Val13;
              int64Ptr[21] = crc32(int64Val15, charPtr2, (int)int64Val14 - (int)int64Val13);
              int64Ptr[24] -= int64Val14 - int64Val13;
              if ( returnVal3 != 1 )
              {
                if ( !returnVal3 )
                  goto LABEL_48;
                return returnVal3;
              }
              return returnVal2;
            }
            break;
          }
          intVal3 = *((_DWORD *)int64Ptr + 4);
          if ( !intVal3 )
          {
            if ( !int64Ptr[23] )
              return returnVal2;
            intVal2 = 0;
            goto LABEL_26;
          }
          goto LABEL_39;
        }
        goto LABEL_30;
      }
      *((_DWORD *)int64Ptr + 10) = int64Val2;
    }
    int64Val3 = int64Ptr[23];
    intVal = *((_DWORD *)int64Ptr + 4);
    if ( (unsigned int)voidPtr <= int64Val3 + (unsigned __int64)intVal || !*((_DWORD *)int64Ptr + 78) )
    {
      intVal2 = *((_DWORD *)int64Ptr + 10);
      goto LABEL_12;
    }
LABEL_30:
    intVal2 = intVal + int64Val3;
    *((_DWORD *)int64Ptr + 10) = intVal2;
    goto LABEL_12;
  }
  return returnVal;
}


__int64 cocos2d::unzSetOffset64(void *voidPtr)
{
  __int64 returnVal;

  if ( !this )
    return 4294967194LL;
  *((_QWORD *)this + 16) = *((_QWORD *)this + 13);
  *((_QWORD *)this + 17) = voidPtr;
  returnVal = sub_B2C634((__int64)this, (char *)this + 176, (_QWORD *)this + 39, 0, 0, 0, 0, 0, 0);
  *((_QWORD *)this + 18) = (_DWORD)returnVal == 0;
  return returnVal;
}


__int64 cocos2d::unzStringFileNameCompare(const char *charPtr, const char *charPtr2)
{
  __int64 int64Val;
  unsigned int intVal;
  unsigned int intVal2;

  if ( (_DWORD)charPtr2 == 1 )
    return strcmp((const char *)this, charPtr);
  int64Val = 0;
  while ( 1 )
  {
    intVal2 = *((unsigned __int8 *)this + int64Val);
    intVal = (unsigned __int8)charPtr[int64Val++];
    if ( (unsigned __int8)(intVal2 - 97) <= 0x19u )
      intVal2 = (unsigned __int8)(intVal2 - 32);
    if ( (unsigned __int8)(intVal - 97) <= 0x19u )
    {
      intVal = (unsigned __int8)(intVal - 32);
      if ( !intVal2 )
        break;
      goto LABEL_5;
    }
    if ( !intVal2 )
      break;
    if ( !intVal )
      return 1;
LABEL_5:
    if ( intVal2 < intVal )
      return 0xFFFFFFFFLL;
    if ( intVal2 > intVal )
      return 1;
  }
  if ( intVal )
    return 0xFFFFFFFFLL;
  else
    return 0;
}


__int64 cocos2d::unzeof(void *voidPtr)
{
  __int64 int64Val;

  if ( this && (int64Val = *((_QWORD *)this + 40)) != 0 )
    return *(_QWORD *)(int64Val + 192) == 0;
  else
    return 4294967194LL;
}


__int64 cocos2d::unztell(void *voidPtr)
{
  __int64 int64Val;

  if ( this && (int64Val = *((_QWORD *)this + 40)) != 0 )
    return *(_QWORD *)(int64Val + 48);
  else
    return -102;
}


__int64 cocos2d::unztell64(void *voidPtr)
{
  __int64 int64Val;

  if ( this && (int64Val = *((_QWORD *)this + 40)) != 0 )
    return *(_QWORD *)(int64Val + 160);
  else
    return -1;
}
