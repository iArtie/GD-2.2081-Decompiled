#include "Utils/MultilineBitmapFont.h"

void MultilineBitmapFont::moveSpecialDescriptors(int index, int size)
{
  unsigned int intVal;
  int intVal2;
  int intVal3;
  cocos2d::CCObject *obj;
  int intVal4;
  bool flag;
  int intVal5;
  unsigned int intVal6;
  bool flag2;
  unsigned int intVal7;
  unsigned int intVal8;
  int m_uID;

  intVal = 0;
  intVal2 = cocos2d::CCArray::count(m_specialDescriptors);
  intVal3 = 0;
  if ( intVal2 )
  {
    do
    {
      obj = cocos2d::CCArray::objectAtIndex(m_specialDescriptors, intVal3);
      intVal4 = HIDWORD(obj[1].__vftable);
      flag = intVal4 <= index;
      intVal5 = intVal4 - size;
      if ( flag )
      {
        m_uID = obj[1].m_uID;
        if ( m_uID != -1 && index < m_uID )
          obj[1].m_uID = m_uID - size;
      }
      else
      {
        intVal6 = obj[1].m_uID;
        HIDWORD(obj[1].__vftable) = intVal5;
        flag2 = intVal6 == -1;
        intVal7 = intVal6 - size;
        if ( !flag2 )
          obj[1].m_uID = intVal7;
      }
      ++intVal;
      intVal8 = cocos2d::CCArray::count(m_specialDescriptors);
      intVal3 = intVal;
    }
    while ( intVal < intVal8 );
  }
}


std::string MultilineBitmapFont::readColorInfo(std::string text)
{
  _QWORD *qwordPtr;
  _QWORD *qwordPtr2;
  unsigned __int64 int64Val;
  int int64Val2;
  __int64 int64Val3;
  unsigned __int64 int64Val4;
  unsigned __int64 int64Val5;
  int intVal;
  __int64 *int64Ptr;
  unsigned __int64 int64Val6;
  int int64Val62;
  __int64 *int64Ptr2;
  int intVal2;
  int intVal3;
  cocos2d::CCArray *specialDescriptors2;
  TextStyleSection *ColoredSection;
  cocos2d::CCObject *obj;
  unsigned __int64 int64Val7;
  int int64Val72;
  unsigned __int64 int64Val8;
  unsigned __int64 int64Val9;
  char *charPtr;
  float value;
  char *charPtr2;
  float value2;
  __int64 *int64Ptr3;
  unsigned __int64 int64Val10;
  int int64Val102;
  __int64 *int64Ptr4;
  cocos2d::CCArray *specialDescriptors3;
  TextStyleSection *InstantSection;
  cocos2d::CCObject *obj2;
  unsigned __int64 int64Val11;
  int int64Val112;
  int intVal4;
  char *charPtr3;
  __int64 *int64Ptr5;
  unsigned __int64 int64Val12;
  int int64Val122;
  __int64 *int64Ptr6;
  cocos2d::CCArray *specialDescriptors4;
  TextStyleSection *ShakeSection;
  cocos2d::CCObject *obj3;
  __int64 int64Val13;
  unsigned int *intPtr;
  int intVal5;
  unsigned __int64 int64Val14;
  int int64Val142;
  bool flag;
  char *charPtr4;
  float value3;
  char *charPtr5;
  float value4;
  __int64 *int64Ptr7;
  TextStyleSection *DelaySection;
  cocos2d::CCObject *obj4;
  unsigned int *intPtr2;
  int intVal6;
  unsigned int *intPtr3;
  int intVal7;
  unsigned int *intPtr4;
  int intVal8;
  char *perSecond;
  int perSeconda;
  char *charPtr6;

  qwordPtr2 = qwordPtr;
  while ( 1 )
  {
    int64Val = sub_D1641C((__int64 *)text.m_data.m_data, "<c", 0, 2u);
    int64Val2 = int64Val;
    if ( int64Val != -1 )
      break;
LABEL_23:
    int64Val7 = sub_D1641C((__int64 *)text.m_data.m_data, "<i", 0, 2u);
    int64Val72 = int64Val7;
    if ( int64Val7 == -1 )
    {
      int64Val11 = sub_D1641C((__int64 *)text.m_data.m_data, "<s", 0, 2u);
      int64Val112 = int64Val11;
      if ( int64Val11 == -1 )
      {
        int64Val14 = sub_D1641C((__int64 *)text.m_data.m_data, "<d", 0, 2u);
        int64Val142 = int64Val14;
        flag = int64Val14 == -1;
        int64Val13 = *(_QWORD *)text.m_data.m_data;
        if ( flag )
          goto LABEL_43;
        int64Val9 = *(_QWORD *)(int64Val13 - 24);
        int64Val8 = int64Val142 + 2;
        if ( int64Val8 > int64Val9 )
LABEL_80:
          sub_D13C5C("%s: __pos (which is %zu) > size() (which is %zu)", "basic_string::substr", int64Val8, int64Val9);
        sub_D14F84(&charPtr6, (__int64 *)text.m_data.m_data, int64Val8, 3u);
        charPtr4 = charPtr6;
        value3 = (float)atoi(charPtr6);
        charPtr5 = charPtr4 - 24;
        value4 = value3 / 100.0;
        if ( charPtr4 - 24 != (char *)&qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr4 = (unsigned int *)(charPtr4 - 8);
            do
              intVal8 = __ldaxr(intPtr4);
            while ( __stlxr(intVal8 - 1, intPtr4) );
          }
          else
          {
            intVal8 = *((_DWORD *)charPtr4 - 2);
            *((_DWORD *)charPtr4 - 2) = intVal8 - 1;
          }
          if ( intVal8 <= 0 )
            operator delete(charPtr5);
        }
        int64Ptr7 = sub_D1861C((__int64 *)text.m_data.m_data, int64Val142, 6u, &byte_F288A0, 0);
        sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr7);
        moveSpecialDescriptors(int64Val142, 6);
        DelaySection = TextStyleSection::createDelaySection(int64Val142, value4);
        cocos2d::CCArray::addObject(m_specialDescriptors, (cocos2d::_ccArray *)DelaySection, obj4);
      }
      else
      {
        int64Val8 = (int)int64Val11 + 2;
        int64Val9 = *(_QWORD *)(*(_QWORD *)text.m_data.m_data - 24LL);
        if ( int64Val8 > int64Val9 )
          goto LABEL_80;
        sub_D14F84(&charPtr6, (__int64 *)text.m_data.m_data, int64Val8, 1u);
        perSecond = charPtr6;
        intVal4 = atoi(charPtr6);
        if ( perSecond - 24 != (char *)&qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr3 = (unsigned int *)(perSecond - 8);
            do
              intVal7 = __ldaxr(intPtr3);
            while ( __stlxr(intVal7 - 1, intPtr3) );
          }
          else
          {
            intVal7 = *((_DWORD *)perSecond - 2);
            *((_DWORD *)perSecond - 2) = intVal7 - 1;
          }
          if ( intVal7 <= 0 )
            operator delete(perSecond - 24);
        }
        int64Val8 = int64Val112 + 3;
        int64Val9 = *(_QWORD *)(*(_QWORD *)text.m_data.m_data - 24LL);
        if ( int64Val8 > int64Val9 )
          goto LABEL_80;
        sub_D14F84(&charPtr6, (__int64 *)text.m_data.m_data, int64Val8, 2u);
        charPtr3 = charPtr6;
        perSeconda = atoi(charPtr6);
        if ( charPtr3 - 24 != (char *)&qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr2 = (unsigned int *)(charPtr3 - 8);
            do
              intVal6 = __ldaxr(intPtr2);
            while ( __stlxr(intVal6 - 1, intPtr2) );
          }
          else
          {
            intVal6 = *((_DWORD *)charPtr3 - 2);
            *((_DWORD *)charPtr3 - 2) = intVal6 - 1;
          }
          if ( intVal6 <= 0 )
            operator delete(charPtr3 - 24);
        }
        int64Ptr5 = sub_D1861C((__int64 *)text.m_data.m_data, int64Val112, 6u, &byte_F288A0, 0);
        sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr5);
        moveSpecialDescriptors(int64Val112, 6);
        int64Val12 = sub_D1641C((__int64 *)text.m_data.m_data, "</s>", 0, 4u);
        int64Val122 = int64Val12;
        if ( int64Val12 == -1 )
          goto LABEL_56;
        int64Ptr6 = sub_D1861C((__int64 *)text.m_data.m_data, (int)int64Val12, 4u, &byte_F288A0, 0);
        sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr6);
        moveSpecialDescriptors(int64Val122, 4);
        specialDescriptors4 = m_specialDescriptors;
        ShakeSection = TextStyleSection::createShakeSection(int64Val112, int64Val122 - 1, intVal4, perSeconda);
        cocos2d::CCArray::addObject(specialDescriptors4, (cocos2d::_ccArray *)ShakeSection, obj3);
      }
    }
    else
    {
      int64Val8 = (int)int64Val7 + 2;
      int64Val9 = *(_QWORD *)(*(_QWORD *)text.m_data.m_data - 24LL);
      if ( int64Val8 > int64Val9 )
        goto LABEL_80;
      sub_D14F84(&charPtr6, (__int64 *)text.m_data.m_data, int64Val8, 3u);
      charPtr = charPtr6;
      value = (float)atoi(charPtr6);
      charPtr2 = charPtr - 24;
      value2 = value / 100.0;
      if ( charPtr - 24 != (char *)&qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr = (unsigned int *)(charPtr - 8);
          do
            intVal5 = __ldaxr(intPtr);
          while ( __stlxr(intVal5 - 1, intPtr) );
        }
        else
        {
          intVal5 = *((_DWORD *)charPtr - 2);
          *((_DWORD *)charPtr - 2) = intVal5 - 1;
        }
        if ( intVal5 <= 0 )
          operator delete(charPtr2);
      }
      int64Ptr3 = sub_D1861C((__int64 *)text.m_data.m_data, int64Val72, 6u, &byte_F288A0, 0);
      sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr3);
      moveSpecialDescriptors(int64Val72, 6);
      int64Val10 = sub_D1641C((__int64 *)text.m_data.m_data, "</i>", 0, 4u);
      int64Val102 = int64Val10;
      if ( int64Val10 == -1 )
      {
LABEL_56:
        *qwordPtr2 = *(_QWORD *)text.m_data.m_data;
        *(_QWORD *)text.m_data.m_data = &byte_124B6C8;
        return (std::string)qwordPtr2;
      }
      int64Ptr4 = sub_D1861C((__int64 *)text.m_data.m_data, (int)int64Val10, 4u, &byte_F288A0, 0);
      sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr4);
      moveSpecialDescriptors(int64Val102, 4);
      specialDescriptors3 = m_specialDescriptors;
      InstantSection = TextStyleSection::createInstantSection(int64Val72, int64Val102 - 1, value2);
      cocos2d::CCArray::addObject(specialDescriptors3, (cocos2d::_ccArray *)InstantSection, obj2);
    }
  }
  while ( 1 )
  {
    int64Val3 = *(_QWORD *)text.m_data.m_data;
    int64Val4 = (int)int64Val + 2;
    int64Val5 = *(_QWORD *)(*(_QWORD *)text.m_data.m_data - 24LL);
    if ( int64Val4 >= int64Val5 )
      sub_D13C5C("basic_string::at: __n (which is %zu) >= size() (which is %zu)", (int)int64Val + 2, int64Val5);
    if ( (*(_DWORD *)(int64Val3 - 8) & 0x80000000) == 0 )
    {
      sub_D176A8((__int64 *)text.m_data.m_data);
      int64Val3 = *(_QWORD *)text.m_data.m_data;
    }
    intVal = *(unsigned __int8 *)(int64Val3 + int64Val4);
    int64Ptr = sub_D1861C((__int64 *)text.m_data.m_data, int64Val2, 4u, &byte_F288A0, 0);
    sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr);
    moveSpecialDescriptors(int64Val2, 4);
    int64Val6 = sub_D1641C((__int64 *)text.m_data.m_data, "</c>", 0, 4u);
    int64Val62 = int64Val6;
    if ( int64Val6 == -1 )
      break;
    int64Ptr2 = sub_D1861C((__int64 *)text.m_data.m_data, (int)int64Val6, 4u, &byte_F288A0, 0);
    sub_D18EA8((__int64 *)text.m_data.m_data, int64Ptr2);
    moveSpecialDescriptors(int64Val62, 4);
    intVal2 = int64Val62 - 1;
    switch ( intVal )
    {
      case 'b':
        LOWORD(charPtr6) = 21066;
        BYTE2(charPtr6) = -31;
        break;
      case 'g':
        LOWORD(charPtr6) = -7360;
        BYTE2(charPtr6) = 72;
        break;
      case 'l':
        LOWORD(charPtr6) = -21664;
        BYTE2(charPtr6) = -17;
        break;
      case 'j':
        LOWORD(charPtr6) = -14286;
        BYTE2(charPtr6) = -1;
        break;
      case 'y':
        LOBYTE(charPtr6) = -1;
        *(_WORD *)((char *)&charPtr6 + 1) = 255;
        break;
      case 'o':
        LOWORD(charPtr6) = -23041;
        BYTE2(charPtr6) = 75;
        break;
      case 'r':
        LOWORD(charPtr6) = 23295;
        BYTE2(charPtr6) = 90;
        break;
      case 'p':
        LOWORD(charPtr6) = 255;
        BYTE2(charPtr6) = -1;
        break;
      case 'a':
        LOWORD(charPtr6) = 12950;
        BYTE2(charPtr6) = -1;
        break;
      case 'd':
        LOWORD(charPtr6) = -26881;
        BYTE2(charPtr6) = -1;
        break;
      case 'c':
        LOWORD(charPtr6) = -1;
        BYTE2(charPtr6) = -106;
        break;
      case 'f':
        LOWORD(charPtr6) = -106;
        BYTE2(charPtr6) = -1;
        break;
      case 's':
        LOWORD(charPtr6) = -8961;
        BYTE2(charPtr6) = 65;
        break;
      default:
        LOWORD(charPtr6) = 255;
        BYTE2(charPtr6) = 0;
        break;
    }
    intVal3 = (int)charPtr6;
    specialDescriptors2 = m_specialDescriptors;
    ColoredSection = TextStyleSection::createColoredSection((cocos2d::ccColor3B)intVal3, int64Val2, intVal2);
    cocos2d::CCArray::addObject(specialDescriptors2, (cocos2d::_ccArray *)ColoredSection, obj);
    int64Val = sub_D1641C((__int64 *)text.m_data.m_data, "<c", 0, 2u);
    int64Val2 = int64Val;
    if ( int64Val == -1 )
      goto LABEL_23;
  }
  int64Val13 = *(_QWORD *)text.m_data.m_data;
LABEL_43:
  *qwordPtr2 = int64Val13;
  *(_QWORD *)text.m_data.m_data = &byte_124B6C8;
  return (std::string)qwordPtr2;
}


void MultilineBitmapFont::setOpacity(unsigned __int8 opacity)
{
  unsigned int intVal;
  unsigned int intVal2;
  cocos2d::CCObject *obj;
  bool flag;
  int intVal3;

  intVal = opacity;
  intVal2 = 0;
  while ( 1 )
  {
    flag = intVal2 >= (unsigned int)cocos2d::CCArray::count(m_characters);
    intVal3 = intVal2++;
    if ( flag )
      break;
    obj = cocos2d::CCArray::objectAtIndex(m_characters, intVal3);
    ((void (__fastcall *)(cocos2d::CCObject *, _QWORD))obj->__vftable[11].~CCObject_0)(obj, intVal);
  }
  cocos2d::CCSprite::setOpacity(this, intVal);
}


MultilineBitmapFont::~MultilineBitmapFont()
{
  MultilineBitmapFont::~MultilineBitmapFont(this);
  operator delete(this);
}
