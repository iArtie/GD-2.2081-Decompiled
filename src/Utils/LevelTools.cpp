#include "Utils/LevelTools.h"

int LevelTools::artistForAudio(int id)
{
  unsigned int intVal;
  int returnVal;

  intVal = id + 1;
  returnVal = 0;
  if ( intVal <= 0x28 )
    return byte_EA5080[intVal];
  return returnVal;
}


std::string LevelTools::base64DecodeString(std::string str)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;

  returnVal.m_data.m_data = internal;
  if ( *(_QWORD *)(*(_QWORD *)str.m_data.m_data - 24LL) )
  {
    cocos2d::ZipUtils::base64URLDecode((const std::string *)str.m_data.m_data);
    return returnVal;
  }
  else
  {
    *(_QWORD *)internal = *(_QWORD *)str.m_data.m_data;
    *(_QWORD *)str.m_data.m_data = &byte_124B6C8;
    return (std::string)internal;
  }
}


std::string LevelTools::base64EncodeString(std::string str)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;

  returnVal.m_data.m_data = internal;
  if ( *(_QWORD *)(*(_QWORD *)str.m_data.m_data - 24LL) )
  {
    cocos2d::ZipUtils::base64URLEncode((const std::string *)str.m_data.m_data);
    return returnVal;
  }
  else
  {
    *(_QWORD *)internal = *(_QWORD *)str.m_data.m_data;
    *(_QWORD *)str.m_data.m_data = &byte_124B6C8;
    return (std::string)internal;
  }
}


cocos2d::CCDictionary * LevelTools::createStarPackDict()
{
  cocos2d::CCArray *array;
  cocos2d::CCArray *array2;
  unsigned __int64 int64Val;
  unsigned __int64 int64Val2;
  unsigned __int64 int64Val3;
  __int64 int64Val4;
  cocos2d::CCString *str;
  cocos2d::CCObject *obj;
  char *charPtr;
  char *charPtr2;
  unsigned int *intPtr;
  int intVal;
  unsigned int *intPtr2;
  int intVal2;
  char *charPtr3;
  void *voidPtr;
  unsigned int intVal3;
  unsigned int intVal32;
  cocos2d::CCString *str2;
  int intVal4;
  cocos2d::CCString *str3;
  unsigned int intVal5;
  unsigned int *intPtr3;
  int intVal6;
  unsigned int *intPtr4;
  int intVal7;
  cocos2d::CCDictionary *returnVal;
  __int64 int64Val5;
  char *charPtr4;
  std::string str4;

  returnVal = cocos2d::CCDictionary::create();
  array = (cocos2d::CCArray *)sub_D153B8(
                             &int64Val5,
                             "1,3,2,4,3,4,5,5,6,6,7,6,8,7,9,8,10,8,11,9,19,10,20,10,21,10,22,10,26,10,27,10,28,10,29,10,3"
                             "0,10,31,10,32,4,33,5,34,6,35,8,36,8,37,5,38,5,39,6,40,7,41,8,42,8,43,8,44,9,45,6,46,10,47,1"
                             "0,48,10,49,10,50,10,52,4,53,3,54,4,55,4,56,5,57,6,58,7,59,8,60,8,61,8,62,9,63,9,64,10,65,10,66,10");
  array2 = (cocos2d::CCArray *)cocos2d::CCArray::create(array);
  int64Val = sub_D168E0(&int64Val5, &byte_F804F8, 0, 1u);
  int64Val2 = int64Val;
  int64Val3 = 0;
  int64Val4 = *(_QWORD *)(int64Val5 - 24);
  while ( 1 )
  {
    sub_D14F84(&charPtr4, &int64Val5, int64Val3, int64Val2);
    if ( int64Val4 != int64Val3 || *((_QWORD *)charPtr4 - 3) )
    {
      sub_D153B8(&str4, charPtr4);
      str = cocos2d::CCString::create(&str4);
      cocos2d::CCArray::addObject(array2, (cocos2d::_ccArray *)str, obj);
      charPtr = (char *)str4.m_data.m_data - 24;
      if ( (__int64 *)str4.m_data.m_data - 3 != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr2 = (unsigned int *)((char *)str4.m_data.m_data - 8);
          do
            intVal2 = __ldaxr(intPtr2);
          while ( __stlxr(intVal2 - 1, intPtr2) );
        }
        else
        {
          intVal2 = *((_DWORD *)str4.m_data.m_data - 2);
          *((_DWORD *)str4.m_data.m_data - 2) = intVal2 - 1;
        }
        if ( intVal2 <= 0 )
          break;
      }
    }
    if ( int64Val == -1 )
      goto LABEL_19;
LABEL_7:
    int64Val3 = int64Val + 1;
    int64Val = sub_D168E0(&int64Val5, &byte_F804F8, int64Val + 1, 1u);
    charPtr2 = charPtr4 - 24;
    if ( charPtr4 - 24 != (char *)&qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr = (unsigned int *)(charPtr4 - 8);
        do
          intVal = __ldaxr(intPtr);
        while ( __stlxr(intVal - 1, intPtr) );
      }
      else
      {
        intVal = *((_DWORD *)charPtr4 - 2);
        *((_DWORD *)charPtr4 - 2) = intVal - 1;
      }
      if ( intVal <= 0 )
        operator delete(charPtr2);
    }
    int64Val2 = int64Val - int64Val3;
    if ( *(_QWORD *)(int64Val5 - 24) < int64Val3 )
      sub_D13C5C(
        "%s: __pos (which is %zu) > this->size() (which is %zu)",
        "basic_string::substr",
        int64Val3,
        *(_QWORD *)(int64Val5 - 24));
  }
  operator delete(charPtr);
  if ( int64Val != -1 )
    goto LABEL_7;
LABEL_19:
  charPtr3 = charPtr4 - 24;
  if ( charPtr4 - 24 != (char *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr4 = (unsigned int *)(charPtr4 - 8);
      do
        intVal7 = __ldaxr(intPtr4);
      while ( __stlxr(intVal7 - 1, intPtr4) );
    }
    else
    {
      intVal7 = *((_DWORD *)charPtr4 - 2);
      *((_DWORD *)charPtr4 - 2) = intVal7 - 1;
    }
    if ( intVal7 <= 0 )
      operator delete(charPtr3);
  }
  voidPtr = (void *)(int64Val5 - 24);
  if ( (__int64 *)(int64Val5 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr3 = (unsigned int *)(int64Val5 - 8);
      do
        intVal6 = __ldaxr(intPtr3);
      while ( __stlxr(intVal6 - 1, intPtr3) );
    }
    else
    {
      intVal6 = *(_DWORD *)(int64Val5 - 8);
      *(_DWORD *)(int64Val5 - 8) = intVal6 - 1;
    }
    if ( intVal6 <= 0 )
      operator delete(voidPtr);
  }
  intVal3 = 0;
  while ( 1 )
  {
    intVal5 = intVal3 + 1;
    if ( intVal3 + 1 >= (unsigned int)cocos2d::CCArray::count(array2) )
      break;
    intVal32 = intVal3;
    intVal3 += 2;
    str2 = cocos2d::CCArray::stringAtIndex(array2, intVal32);
    intVal4 = cocos2d::CCString::intValue(str2);
    str3 = cocos2d::CCArray::stringAtIndex(array2, intVal5);
    cocos2d::CCDictionary::setObject(returnVal, str3, (const std::string *)intVal4);
  }
  return returnVal;
}


std::string LevelTools::fbURLForArtist(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;

  switch ( id )
  {
    case 0:
      internal4 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal4;
      break;
    case 1:
      internal5 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal5;
      break;
    case 3:
      internal6 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal6;
      break;
    case 4:
      internal7 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal7;
      break;
    case 5:
      internal8 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal8;
      break;
    case 6:
      internal9 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal9;
      break;
    case 7:
      internal10 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal10;
      break;
    case 8:
      internal11 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal11;
      break;
    case 9:
      internal12 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal12;
      break;
    case 10:
      internal13 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal13;
      break;
    case 11:
      internal3 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal3;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, &byte_F288A0);
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


int LevelTools::getAudioBPM(int id)
{
  int returnVal;

  returnVal = 130;
  if ( (unsigned int)id <= 0xE )
    return dword_EA50B0[id];
  return returnVal;
}


std::string LevelTools::getAudioFileName(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;
  stl::StringData<char>::Internal *internal14;
  stl::StringData<char>::Internal *internal15;
  stl::StringData<char>::Internal *internal16;
  stl::StringData<char>::Internal *internal17;
  stl::StringData<char>::Internal *internal18;
  stl::StringData<char>::Internal *internal19;
  stl::StringData<char>::Internal *internal20;
  stl::StringData<char>::Internal *internal21;
  stl::StringData<char>::Internal *internal22;
  stl::StringData<char>::Internal *internal23;
  stl::StringData<char>::Internal *internal24;
  stl::StringData<char>::Internal *internal25;
  stl::StringData<char>::Internal *internal26;
  stl::StringData<char>::Internal *internal27;
  stl::StringData<char>::Internal *internal28;
  stl::StringData<char>::Internal *internal29;
  stl::StringData<char>::Internal *internal30;
  stl::StringData<char>::Internal *internal31;
  stl::StringData<char>::Internal *internal32;
  stl::StringData<char>::Internal *internal33;
  stl::StringData<char>::Internal *internal34;
  stl::StringData<char>::Internal *internal35;
  stl::StringData<char>::Internal *internal36;
  stl::StringData<char>::Internal *internal37;
  stl::StringData<char>::Internal *internal38;
  stl::StringData<char>::Internal *internal39;
  stl::StringData<char>::Internal *internal40;
  stl::StringData<char>::Internal *internal41;

  switch ( id )
  {
    case 0:
      internal40 = internal;
      sub_D153B8(internal, "StereoMadness.mp3");
      returnVal.m_data.m_data = internal40;
      break;
    case 2:
      internal39 = internal;
      sub_D153B8(internal, "Polargeist.mp3");
      returnVal.m_data.m_data = internal39;
      break;
    case 3:
      internal38 = internal;
      sub_D153B8(internal, "DryOut.mp3");
      returnVal.m_data.m_data = internal38;
      break;
    case 4:
      internal37 = internal;
      sub_D153B8(internal, "BaseAfterBase.mp3");
      returnVal.m_data.m_data = internal37;
      break;
    case 5:
      internal36 = internal;
      sub_D153B8(internal, "CantLetGo.mp3");
      returnVal.m_data.m_data = internal36;
      break;
    case 6:
      internal35 = internal;
      sub_D153B8(internal, "Jumper.mp3");
      returnVal.m_data.m_data = internal35;
      break;
    case 7:
      internal34 = internal;
      sub_D153B8(internal, "TimeMachine.mp3");
      returnVal.m_data.m_data = internal34;
      break;
    case 8:
      internal33 = internal;
      sub_D153B8(internal, "Cycles.mp3");
      returnVal.m_data.m_data = internal33;
      break;
    case 9:
      internal32 = internal;
      sub_D153B8(internal, "xStep.mp3");
      returnVal.m_data.m_data = internal32;
      break;
    case 10:
      internal31 = internal;
      sub_D153B8(internal, "Clutterfunk.mp3");
      returnVal.m_data.m_data = internal31;
      break;
    case 11:
      internal30 = internal;
      sub_D153B8(internal, "TheoryOfEverything.mp3");
      returnVal.m_data.m_data = internal30;
      break;
    case 12:
      internal29 = internal;
      sub_D153B8(internal, "Electroman.mp3");
      returnVal.m_data.m_data = internal29;
      break;
    case 13:
      internal28 = internal;
      sub_D153B8(internal, "Clubstep.mp3");
      returnVal.m_data.m_data = internal28;
      break;
    case 14:
      internal27 = internal;
      sub_D153B8(internal, "Electrodynamix.mp3");
      returnVal.m_data.m_data = internal27;
      break;
    case 15:
      internal26 = internal;
      sub_D153B8(internal, "HexagonForce.mp3");
      returnVal.m_data.m_data = internal26;
      break;
    case 16:
      internal25 = internal;
      sub_D153B8(internal, "BlastProcessing.mp3");
      returnVal.m_data.m_data = internal25;
      break;
    case 17:
      internal24 = internal;
      sub_D153B8(internal, "TheoryOfEverything2.mp3");
      returnVal.m_data.m_data = internal24;
      break;
    case 18:
      internal23 = internal;
      sub_D153B8(internal, "GeometricalDominator.mp3");
      returnVal.m_data.m_data = internal23;
      break;
    case 19:
      internal22 = internal;
      sub_D153B8(internal, "Deadlocked.mp3");
      returnVal.m_data.m_data = internal22;
      break;
    case 20:
      internal21 = internal;
      sub_D153B8(internal, "Fingerdash.mp3");
      returnVal.m_data.m_data = internal21;
      break;
    case 21:
      internal20 = internal;
      sub_D153B8(internal, "Dash.mp3");
      returnVal.m_data.m_data = internal20;
      break;
    case 22:
      internal19 = internal;
      sub_D153B8(internal, "Explorers.mp3");
      returnVal.m_data.m_data = internal19;
      break;
    case 23:
      internal18 = internal;
      sub_D153B8(internal, "The7Seas.mp3");
      returnVal.m_data.m_data = internal18;
      break;
    case 24:
      internal17 = internal;
      sub_D153B8(internal, "VikingArena.mp3");
      returnVal.m_data.m_data = internal17;
      break;
    case 25:
      internal16 = internal;
      sub_D153B8(internal, "AirborneRobots.mp3");
      returnVal.m_data.m_data = internal16;
      break;
    case 26:
      internal15 = internal;
      sub_D153B8(internal, "DJRubRub.mp3");
      returnVal.m_data.m_data = internal15;
      break;
    case 27:
      internal14 = internal;
      sub_D153B8(internal, "Payload.mp3");
      returnVal.m_data.m_data = internal14;
      break;
    case 28:
      internal13 = internal;
      sub_D153B8(internal, "BeastMode.mp3");
      returnVal.m_data.m_data = internal13;
      break;
    case 29:
      internal12 = internal;
      sub_D153B8(internal, "Machina.mp3");
      returnVal.m_data.m_data = internal12;
      break;
    case 30:
      internal11 = internal;
      sub_D153B8(internal, "Years.mp3");
      returnVal.m_data.m_data = internal11;
      break;
    case 31:
      internal10 = internal;
      sub_D153B8(internal, "ElectroFrontlines.mp3");
      returnVal.m_data.m_data = internal10;
      break;
    case 32:
      internal9 = internal;
      sub_D153B8(internal, "SpacePirates.mp3");
      returnVal.m_data.m_data = internal9;
      break;
    case 33:
      internal8 = internal;
      sub_D153B8(internal, "Striker.mp3");
      returnVal.m_data.m_data = internal8;
      break;
    case 34:
      internal7 = internal;
      sub_D153B8(internal, "Embers.mp3");
      returnVal.m_data.m_data = internal7;
      break;
    case 35:
      internal6 = internal;
      sub_D153B8(internal, "Round1.mp3");
      returnVal.m_data.m_data = internal6;
      break;
    case 36:
      internal5 = internal;
      sub_D153B8(internal, "MonsterDanceOff.mp3");
      returnVal.m_data.m_data = internal5;
      break;
    case 37:
      internal4 = internal;
      sub_D153B8(internal, "PressStart.mp3");
      returnVal.m_data.m_data = internal4;
      break;
    case 38:
      internal3 = internal;
      sub_D153B8(internal, "knockemout.mp3");
      returnVal.m_data.m_data = internal3;
      break;
    case 39:
      internal41 = internal;
      sub_D153B8(internal, "PowerTrip.mp3");
      returnVal.m_data.m_data = internal41;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, "BackOnTrack.mp3");
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


std::string LevelTools::getAudioString(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;
  stl::StringData<char>::Internal *internal14;
  stl::StringData<char>::Internal *internal15;
  stl::StringData<char>::Internal *internal16;
  stl::StringData<char>::Internal *internal17;
  stl::StringData<char>::Internal *internal18;
  stl::StringData<char>::Internal *internal19;
  stl::StringData<char>::Internal *internal20;
  stl::StringData<char>::Internal *internal21;
  stl::StringData<char>::Internal *internal22;
  stl::StringData<char>::Internal *internal23;
  stl::StringData<char>::Internal *internal24;

  switch ( id )
  {
    case 0:
      internal24 = internal;
      sub_D153B8(
        internal,
        "0.19~0.80~0.38~0.80~0.75~0.80~1.12~0.80~1.50~0.80~1.88~0.80~2.25~0.80~2.62~0.80~3.00~0.80~3.38~0.80~3.75~0.80~4."
        "12~0.80~4.50~0.80~4.88~0.80~5.25~0.80~5.62~0.80~6.00~0.80~6.38~0.80~6.75~0.80~7.12~0.80~7.50~0.80~7.88~0.80~8.25"
        "~0.80~8.62~0.80~9.00~0.80~9.38~0.80~9.75~0.80~10.12~0.80~10.50~0.80~10.88~0.80~11.25~0.80~11.62~0.80~12.00~1.00~"
        "12.00~0.90~12.38~0.90~12.75~0.90~13.12~0.90~13.50~0.90~13.88~0.90~14.25~0.90~14.62~0.90~15.00~0.90~15.38~0.90~15"
        ".75~0.90~16.12~0.90~16.50~0.90~16.88~0.90~17.25~0.90~17.62~0.90~18.00~0.90~18.38~0.90~18.75~0.90~19.12~0.90~19.5"
        "0~0.90~19.88~0.90~20.25~0.90~20.62~0.90~21.00~0.90~21.38~0.90~21.75~0.90~22.12~0.90~22.50~0.90~22.88~0.90~23.25~"
        "0.90~23.62~0.90~25.50~1.00~25.50~0.90~25.88~0.90~26.25~0.90~26.62~0.90~27.00~1.00~27.00~0.90~27.38~0.90~27.75~0."
        "90~28.12~0.90~28.50~1.00~28.50~0.90~28.88~0.90~29.25~0.90~29.62~0.90~30.00~1.00~30.00~0.90~30.38~0.90~30.75~0.90"
        "~31.12~0.90~31.50~1.00~31.50~0.90~31.88~0.90~32.25~0.90~32.62~0.90~33.00~1.00~33.00~0.90~33.38~0.90~33.75~0.90~3"
        "4.12~0.90~34.50~1.00~34.50~0.90~34.88~0.90~35.25~0.90~35.62~0.90~36.00~1.00~36.00~0.90~36.38~0.90~36.75~0.90~37."
        "12~0.90~37.50~1.00~37.50~0.90~37.88~0.90~38.25~0.90~38.62~0.90~39.00~1.00~39.00~0.90~39.38~0.90~39.75~0.90~40.12"
        "~0.90~40.31~1.00~40.50~0.90~40.88~0.90~41.25~0.90~41.62~0.90~42.00~1.00~42.00~0.90~42.38~0.90~42.75~0.90~43.12~0"
        ".90~43.50~1.00~43.50~0.90~43.88~0.90~44.25~0.90~44.62~0.90~45.00~1.00~45.00~0.90~45.38~0.90~45.75~0.90~46.12~0.9"
        "0~46.50~1.00~46.50~0.90~46.88~0.90~47.25~0.90~47.62~0.90~48.00~1.00~48.00~0.90~48.38~0.90~48.75~0.90~49.12~0.90~"
        "49.50~0.80~49.88~0.80~50.25~0.80~50.62~0.80~51.00~0.80~51.38~0.80~51.75~0.80~52.12~0.80~52.50~0.80~52.88~0.80~53"
        ".25~0.80~53.62~0.80~54.00~0.80~54.38~0.80~54.75~0.80~55.12~0.80~55.50~0.80~55.88~0.80~56.25~0.80~56.62~0.80~57.0"
        "0~0.80~57.38~0.80~57.75~0.80~58.12~0.80~58.50~0.80~58.88~0.80~59.25~0.80~59.62~0.80~60.00~0.80~60.38~0.80~60.75~"
        "0.80~61.12~0.80~61.50~0.80~61.88~0.80~62.25~0.80~62.62~0.80~63.00~0.80~63.38~0.80~63.75~0.80~64.12~0.80~64.50~0."
        "80~64.88~0.80~65.25~0.80~65.62~0.80~66.00~0.80~66.38~0.80~66.75~0.80~67.12~0.80~67.50~0.80~67.88~0.80~68.25~0.80"
        "~68.62~0.80~69.00~0.80~69.38~0.80~69.75~0.80~70.12~0.80~70.50~0.80~70.88~0.80~71.25~0.80~71.62~0.80~72.00~0.80~7"
        "2.38~0.80~72.75~0.80~73.12~0.80~73.50~1.00~73.50~0.80~73.88~0.80~74.06~0.80~74.44~0.80~74.62~0.80~75.00~0.80~75."
        "19~0.80~75.56~0.80~75.75~0.80~76.12~0.80~76.31~0.80~76.50~0.80~76.88~0.80~77.06~0.80~77.44~0.80~77.62~0.80~78.00"
        "~0.80~78.19~0.80~78.38~0.80~78.75~0.80~78.94~0.80~79.31~0.80~79.50~0.80~79.88~0.80~80.06~0.80~80.25~0.80~80.62~0"
        ".80~80.81~0.80~81.19~0.80~81.38~0.80~81.75~0.80~81.94~0.80~82.31~0.80~82.50~0.80~82.88~0.80~83.06~0.80~83.25~0.8"
        "0~83.62~0.80~83.81~0.80~84.19~0.80~84.38~0.80~84.75~0.80~84.94~0.80~85.31~0.80~85.50~0.80~85.69~1.00~85.69~0.90~"
        "85.88~0.80~85.88~0.90~86.25~0.90");
      returnVal.m_data.m_data = internal24;
      break;
    case 1:
      internal23 = internal;
      sub_D153B8(
        internal,
        "0.21~0.80~0.42~0.80~1.27~0.80~1.69~0.80~2.11~0.80~2.54~0.80~2.96~0.80~3.17~0.80~3.38~0.80~3.80~0.80~4.65~0.80~5."
        "07~0.80~5.49~0.80~5.70~0.80~5.92~0.80~6.34~0.80~6.55~0.80~6.76~0.80~7.18~0.80~8.03~0.80~8.45~0.80~8.87~0.80~9.30"
        "~0.80~9.72~0.80~9.93~0.80~10.14~0.80~10.56~0.80~11.41~0.80~11.83~0.80~12.25~0.80~12.46~0.80~12.68~0.80~13.10~0.8"
        "0~13.31~0.80~13.52~0.80~13.52~0.90~13.94~0.90~13.94~1.00~14.58~0.90~14.79~1.00~14.79~0.90~15.21~0.90~15.63~1.00~"
        "15.63~0.90~16.06~0.90~16.48~0.90~16.48~1.00~16.69~0.90~16.90~0.90~17.32~1.00~17.32~0.90~17.96~0.90~18.17~1.00~18"
        ".17~0.90~18.59~0.90~19.01~1.00~19.01~0.90~19.44~0.90~19.86~0.90~19.86~1.00~20.07~0.90~20.28~0.90~20.70~0.90~20.7"
        "0~1.00~21.34~0.90~21.55~1.00~21.55~0.90~21.97~0.90~22.39~0.90~22.39~1.00~22.82~0.90~23.24~0.90~23.24~1.00~23.45~"
        "0.90~23.66~0.90~24.08~0.90~24.08~1.00~24.72~0.90~24.93~1.00~24.93~0.90~25.35~0.90~25.77~1.00~25.77~0.90~26.20~0."
        "90~26.62~1.00~26.62~0.90~26.83~0.90~27.04~0.90~27.46~0.90~27.46~1.00~28.10~0.90~28.31~0.90~28.31~1.00~28.73~0.90"
        "~29.15~1.00~29.15~0.90~29.58~0.90~30.00~1.00~30.00~0.90~30.21~0.90~30.42~0.90~30.85~1.00~30.85~0.90~31.48~0.90~3"
        "1.69~1.00~31.69~0.90~32.11~0.90~32.54~1.00~32.54~0.90~32.96~0.90~33.38~1.00~33.38~0.90~33.59~0.90~33.80~0.90~34."
        "23~0.90~34.23~1.00~34.86~0.90~35.07~1.00~35.07~0.90~35.49~0.90~35.92~1.00~35.92~0.90~36.34~0.90~36.76~1.00~36.76"
        "~0.90~36.97~0.90~37.18~0.90~37.61~1.00~37.61~0.90~38.24~0.90~38.45~1.00~38.45~0.90~38.87~0.90~39.30~1.00~39.30~0"
        ".90~39.72~0.90~40.14~1.00~40.14~0.90~40.35~0.90~40.56~0.90~40.99~0.90~41.62~0.80~41.83~0.80~42.04~0.80~42.25~0.8"
        "0~42.68~0.80~42.89~0.80~43.10~0.80~43.52~0.80~43.73~0.80~43.94~0.80~44.37~0.80~45.00~0.80~45.21~0.80~45.42~0.80~"
        "45.63~0.80~46.06~0.80~46.27~0.80~46.48~0.80~46.90~0.80~47.11~0.80~47.32~0.80~47.75~0.80~48.38~0.80~48.59~0.80~48"
        ".80~0.80~49.01~0.80~49.44~0.80~49.65~0.80~49.86~0.80~50.28~0.80~50.49~0.80~50.70~0.80~51.13~0.80~51.76~0.80~51.9"
        "7~0.80~52.18~0.80~52.39~0.80~52.82~0.80~53.03~0.80~53.24~0.80~53.66~0.80~53.87~0.80~54.08~0.90~54.51~1.00~54.51~"
        "0.90~55.14~0.90~55.35~0.90~55.35~1.00~55.56~0.90~55.77~0.90~56.20~1.00~56.20~0.90~56.41~0.90~56.62~0.90~57.04~1."
        "00~57.04~0.90~57.25~0.90~57.46~0.90~57.89~1.00~57.89~0.90~58.52~0.90~58.73~1.00~58.73~0.90~58.94~0.90~59.15~0.90"
        "~59.58~1.00~59.58~0.90~59.79~0.90~60.00~0.90~60.42~1.00~60.42~0.90~60.63~0.90~60.85~0.90~61.27~0.90~61.27~1.00~6"
        "1.90~0.90~62.11~1.00~62.11~0.90~62.32~0.90~62.54~0.90~62.96~1.00~62.96~0.90~63.17~0.90~63.38~0.90~63.80~1.00~63."
        "80~0.90~64.01~0.90~64.23~0.90~64.65~0.90~64.65~1.00~65.28~0.90~65.49~0.90~65.49~1.00~65.70~0.90~65.92~0.90~66.34"
        "~1.00~66.34~0.90~66.55~0.90~66.76~0.90~67.18~1.00~67.18~0.90~67.39~0.90~67.61~0.90~67.61~0.80~68.03~0.90~68.03~0"
        ".80~68.45~0.90~68.45~0.80~68.66~0.80~68.87~0.80~69.08~0.80~69.30~0.80~69.72~0.80~70.14~0.80~70.35~0.80~70.56~0.8"
        "0~70.77~0.80~70.99~0.80~71.41~0.80~71.83~0.80~72.04~0.80~72.25~0.80~72.46~0.80~72.68~0.80~73.10~0.80~73.52~0.80~"
        "74.37~0.80~74.79~0.80~75.21~0.80~75.42~0.80~75.63~1.00~75.63~0.80~75.85~0.80~76.06~0.80~76.48~0.80~76.48~1.00~76"
        ".90~0.80~77.11~0.80~77.32~1.00~77.32~0.80~77.54~0.80~77.75~0.80~78.17~1.00~78.17~0.80~78.59~0.80~78.80~0.80~79.0"
        "1~1.00~79.01~0.80~79.23~0.80~79.44~0.80~79.86~1.00~79.86~0.80~80.70~1.00~81.13~0.90~81.55~1.00~81.55~0.90~81.97~0.90");
      returnVal.m_data.m_data = internal23;
      break;
    case 2:
      internal22 = internal;
      sub_D153B8(
        internal,
        "1.66~1.00~2.02~0.80~2.21~0.80~2.58~0.80~3.50~0.80~3.68~0.80~4.05~0.80~4.97~0.80~5.15~0.80~5.52~0.80~6.44~0.80~6."
        "63~0.80~6.99~0.80~7.36~1.00~7.91~0.80~8.10~0.80~8.47~0.80~9.39~0.80~9.57~0.80~9.94~0.80~10.86~0.80~11.04~0.80~11"
        ".41~0.80~12.33~0.80~12.52~0.80~12.88~0.80~13.80~0.80~13.99~0.80~14.17~0.80~15.28~0.80~15.46~0.80~15.83~0.80~16.7"
        "5~0.80~16.93~0.80~17.30~0.80~18.22~0.80~18.40~0.80~18.77~0.80~19.14~1.00~19.14~0.90~19.69~0.90~19.88~0.90~20.25~"
        "0.90~20.43~0.90~20.80~0.90~21.17~0.90~21.35~0.90~21.72~0.90~22.09~0.90~22.64~0.90~22.82~0.90~23.19~0.90~23.37~0."
        "90~23.74~0.90~24.11~0.90~24.29~0.90~24.66~0.90~25.03~0.90~25.58~0.90~25.77~0.90~26.13~0.90~26.32~0.90~26.69~0.90"
        "~27.06~0.90~27.24~0.90~27.61~0.90~27.98~0.90~28.16~0.90~28.53~0.90~28.71~0.90~29.08~0.90~29.26~0.90~29.63~0.90~2"
        "9.82~0.90~30.18~0.90~30.55~0.90~31.10~0.80~31.29~0.80~31.47~0.80~31.84~0.80~32.02~0.80~32.39~0.80~32.76~0.80~33."
        "13~0.80~33.50~0.80~34.42~0.80~34.79~0.80~35.15~0.80~35.52~0.80~35.89~0.80~36.07~0.80~36.44~0.80~36.81~0.80~37.36"
        "~0.80~37.73~0.80~38.10~0.80~38.28~0.80~38.65~0.80~39.02~0.80~39.39~0.80~40.31~0.80~40.67~0.80~41.04~0.80~41.41~0"
        ".80~41.78~0.80~41.96~0.80~42.33~0.80~43.07~1.00~43.07~0.90~43.44~0.90~43.62~0.90~43.99~0.90~44.36~0.90~44.72~0.9"
        "0~45.09~0.90~45.46~0.90~45.83~1.00~45.83~0.80~46.01~0.80~46.20~0.80~46.38~0.80~46.56~0.80~46.75~0.80~46.93~0.80~"
        "47.12~0.80~47.30~0.80~47.48~0.80~48.96~0.90~49.33~0.90~49.51~0.90~49.88~0.90~50.25~0.90~50.61~0.90~50.98~0.90~51"
        ".35~0.90~51.72~0.80~51.90~0.80~52.09~0.80~52.27~0.80~52.45~0.80~52.64~0.80~52.82~0.80~53.01~0.80~53.19~0.80~53.3"
        "7~0.80~54.85~0.90~55.21~0.90~57.79~0.90~58.16~0.90~58.34~0.80~58.53~0.80~58.71~0.80~58.90~0.80~59.08~0.80~59.26~"
        "0.80~60.74~0.90~61.10~0.90~61.29~0.90~61.66~0.90~62.02~0.90~62.39~0.90~62.76~0.90~63.13~0.90~63.50~0.80~63.68~0."
        "80~63.87~0.80~64.05~0.80~64.23~0.80~64.42~0.80~64.60~0.80~64.79~0.80~64.97~0.80~65.15~0.80~66.26~1.00~68.28~0.80"
        "~68.47~0.80~68.83~0.80~69.75~0.80~69.94~0.80~70.31~0.80~71.23~0.80~71.41~0.80~71.78~0.80~72.70~0.80~72.88~0.80~7"
        "3.25~0.80~74.17~0.80~74.36~0.80~74.72~0.80~75.64~0.80~75.83~0.80~76.20~0.80~76.38~0.80~76.75~0.80~77.12~0.80~77."
        "30~0.80~77.67~0.80~78.04~1.00~78.04~0.90~78.59~0.90~78.77~0.90~79.14~0.90~79.33~0.90~79.69~0.90~80.06~0.90~80.25"
        "~0.90~80.61~0.90~80.98~0.90~81.53~0.90~81.72~0.90~82.09~0.90~82.27~0.90~82.64~0.90~83.01~0.90~83.19~0.90~83.56~0"
        ".90~84.11~0.90~84.48~0.90~84.66~0.90~85.03~0.90~85.21~0.90~85.58~0.90~85.95~0.90~86.13~0.90~86.50~0.90~86.87~0.9"
        "0~87.06~0.90~87.42~0.90~87.61~0.90~87.98~0.90~88.16~0.90~88.53~0.90~88.71~0.90~89.08~0.90~89.45~0.90~89.82~1.00");
      returnVal.m_data.m_data = internal22;
      break;
    case 3:
      internal21 = internal;
      sub_D153B8(
        internal,
        "0.21~0.80~0.41~0.80~0.52~0.80~0.83~0.80~1.14~0.80~1.34~0.80~1.55~0.80~1.76~0.80~2.07~0.80~2.38~0.80~2.90~0.80~3."
        "10~0.80~3.31~0.80~3.52~0.80~3.62~0.80~3.93~0.80~4.03~0.80~4.55~0.80~4.76~0.80~4.97~0.80~5.17~0.80~5.38~0.80~5.69"
        "~0.80~6.10~0.80~6.31~0.80~6.52~0.80~6.83~0.80~7.03~0.80~7.14~0.80~7.45~0.80~7.86~0.80~8.07~0.80~8.28~0.80~8.48~0"
        ".80~8.79~0.80~9.10~0.80~9.52~0.80~9.72~0.80~9.93~0.80~10.03~0.80~10.34~0.80~10.55~0.80~10.76~0.80~11.07~0.80~11."
        "28~0.80~11.59~0.80~11.79~0.80~12.00~0.80~12.31~0.80~12.72~0.80~12.93~0.80~13.24~0.80~13.24~1.00~13.24~0.90~13.45"
        "~0.80~13.66~0.90~13.66~0.80~13.86~0.80~14.07~0.80~14.07~0.90~14.38~0.80~14.48~0.90~14.69~0.80~14.90~0.80~14.90~0"
        ".90~15.00~0.80~15.31~0.90~15.41~0.80~15.72~0.90~15.72~0.80~16.14~0.90~16.14~0.80~16.34~0.80~16.55~0.90~16.55~0.8"
        "0~16.76~0.80~16.97~0.80~16.97~0.90~17.07~0.80~17.38~0.80~17.38~0.90~17.79~0.80~17.79~0.90~18.00~0.80~18.10~0.90~"
        "18.21~0.80~18.41~0.80~18.62~0.90~18.72~0.80~18.93~0.90~19.03~0.80~19.34~0.90~19.45~0.80~19.66~0.80~19.76~0.90~19"
        ".86~0.80~20.07~0.80~20.17~0.90~20.28~0.80~20.48~0.80~20.59~0.90~20.69~0.80~21.00~0.90~21.00~0.80~21.31~0.80~21.5"
        "2~0.90~21.52~0.80~21.72~0.80~21.83~0.90~22.03~0.80~22.24~0.90~22.24~0.80~22.66~0.90~22.76~0.80~22.97~0.80~23.07~"
        "0.90~23.17~0.80~23.28~0.80~23.48~0.90~23.48~0.80~23.69~0.80~24.00~0.90~24.00~0.80~24.31~0.90~24.31~0.80~24.52~0."
        "80~24.72~0.80~24.83~0.90~25.03~0.80~25.14~0.90~25.34~0.80~25.55~0.90~25.55~0.80~25.97~0.90~26.07~0.80~26.17~0.80"
        "~26.38~0.90~26.38~0.80~26.69~0.80~28.14~0.90~28.14~1.00~28.24~0.80~28.55~0.90~28.86~0.90~29.17~0.90~29.59~0.90~2"
        "9.79~0.90~29.79~1.00~30.21~0.90~30.52~0.90~30.83~0.90~31.24~0.90~31.45~1.00~31.45~0.90~31.86~0.90~32.28~0.90~32."
        "48~0.90~32.90~0.90~33.10~1.00~33.10~0.90~33.52~0.90~33.93~0.90~34.34~0.90~34.55~0.90~34.66~0.90~34.76~1.00~35.17"
        "~0.90~35.48~0.90~35.79~0.90~36.21~0.90~36.31~1.00~36.41~0.90~36.83~0.90~37.24~0.90~37.55~0.90~37.86~0.90~37.97~1"
        ".00~38.07~0.90~38.48~0.90~38.90~0.90~39.10~0.90~39.52~0.90~39.72~0.90~39.72~1.00~40.03~0.90~40.55~0.90~40.97~0.9"
        "0~41.17~0.90~41.38~1.00~41.38~0.90~41.79~0.80~41.79~0.90~42.00~0.80~42.10~0.80~42.21~0.90~42.41~0.90~42.83~0.90~"
        "43.03~1.00~43.03~0.90~43.24~0.80~43.45~0.90~43.55~0.80~43.76~0.80~43.76~0.90~44.07~0.90~44.48~0.90~44.69~0.90~44"
        ".69~1.00~44.79~0.80~45.00~0.80~45.10~0.90~45.31~0.80~45.52~0.80~45.52~0.90~45.72~0.90~46.14~0.90~46.34~1.00~46.3"
        "4~0.90~46.55~0.80~46.76~0.90~46.76~0.80~47.07~0.80~47.17~0.90~47.59~0.90~47.79~0.90~48.00~1.00~48.00~0.90~48.10~"
        "0.80~48.41~0.80~48.41~0.90~48.62~0.80~48.72~0.80~48.83~0.90~49.03~0.90~49.45~0.90~49.66~1.00~49.66~0.90~49.76~0."
        "80~50.07~0.90~50.17~0.80~50.38~0.80~50.48~0.90~50.69~0.90~51.10~0.90~51.31~1.00~51.31~0.90~51.41~0.80~51.62~0.80"
        "~51.72~0.90~51.93~0.80~52.03~0.80~52.14~0.90~52.34~0.90~52.76~0.90~52.97~0.90~52.97~1.00~53.07~0.80~53.38~0.80~5"
        "3.38~0.90~53.69~0.80~53.79~0.90~54.21~0.90~54.41~0.90~54.62~1.00~54.62~0.90~55.34~0.90~55.55~0.90~55.86~0.90~56."
        "07~0.90~56.28~0.90~56.79~0.90~57.31~0.90~57.52~0.90~57.72~0.90~57.93~0.90~58.55~0.90~58.76~0.90~58.97~0.90~59.17"
        "~0.90~59.38~0.90~59.59~0.90~60.10~0.90~60.31~0.90~60.83~0.90~61.03~0.90~61.24~0.90~61.86~0.90~62.07~0.90~62.28~0"
        ".90~62.48~0.90~62.69~0.90~62.90~0.90~63.41~0.90~63.93~0.90~64.03~0.90~64.34~0.90~64.55~0.90~65.17~0.90~65.38~0.9"
        "0~65.59~0.90~65.79~0.90~66.00~0.90~66.21~0.90~66.83~0.90~66.93~0.90~67.45~0.90~67.66~0.90~67.86~0.90~68.07~0.90~"
        "68.48~0.80~68.79~0.80~69.00~0.80~69.21~0.80~69.52~0.80~69.72~0.80~70.24~0.80~70.66~0.80~70.97~0.80~71.07~0.80~71"
        ".28~0.80~72.00~0.80~72.21~0.80~72.41~0.80~72.52~0.80~72.83~0.80~72.93~0.80~73.55~0.80~73.86~0.80~74.17~0.80~74.4"
        "8~0.80~74.69~0.80~75.21~0.80~75.41~0.80~75.62~0.80~75.93~0.80~76.03~0.80~76.34~0.80~76.86~0.80~77.28~0.80~77.59~"
        "0.80~77.79~0.80~78.00~0.80~78.52~0.80~78.72~0.80~78.93~0.80~79.24~0.80~79.45~0.80~79.66~0.80~80.17~0.80~80.38~0."
        "80~80.79~0.80~81.00~0.80~81.10~1.00~81.10~0.90~81.21~0.80~81.83~0.80~81.93~0.90~82.03~0.80~82.14~0.90~82.34~0.80"
        "~82.34~0.90~82.55~0.80~82.55~0.90~82.76~0.90~82.76~0.80~82.97~0.80~83.28~0.90~83.48~0.80~83.79~0.90~83.90~0.80~8"
        "4.00~0.90~84.10~0.80~84.21~0.90~84.31~0.80~84.41~0.90~84.62~0.80");
      returnVal.m_data.m_data = internal21;
      break;
    case 4:
      internal20 = internal;
      sub_D153B8(internal, a03208005308008);
      returnVal.m_data.m_data = internal20;
      break;
    case 5:
      internal19 = internal;
      sub_D153B8(internal, a03508005308008);
      returnVal.m_data.m_data = internal19;
      break;
    case 6:
      internal18 = internal;
      sub_D153B8(
        internal,
        "0.00~0.90~0.34~0.90~0.67~0.90~1.01~0.90~1.35~0.90~1.69~0.90~2.02~0.90~2.36~0.90~2.70~0.90~3.03~0.90~3.37~0.90~3."
        "71~0.90~4.04~0.90~4.38~0.90~4.72~0.90~5.39~0.90~5.73~0.90~6.07~0.90~6.40~0.90~6.74~0.90~7.08~0.90~7.42~0.90~7.75"
        "~0.90~8.09~0.90~8.43~0.90~8.76~0.90~9.10~0.90~9.44~0.90~9.78~0.90~10.11~0.90~10.45~0.90~10.79~1.00~10.79~0.90~11"
        ".12~0.90~11.46~0.90~11.80~0.90~12.13~0.90~12.47~0.90~12.81~0.90~13.15~0.90~13.48~0.90~13.82~0.90~14.16~0.90~14.4"
        "9~0.90~14.83~0.90~15.17~0.90~15.51~0.90~15.84~0.90~16.18~0.90~16.52~0.90~16.85~0.90~17.19~0.90~17.53~0.90~17.87~"
        "0.90~18.20~0.90~18.54~0.90~18.88~0.90~19.21~0.90~19.55~0.90~19.89~0.90~20.22~0.90~20.56~0.90~20.90~0.90~21.24~0."
        "90~21.57~1.00~21.57~0.90~21.91~0.90~22.25~0.90~22.58~0.90~22.92~0.90~23.26~0.90~23.60~0.90~23.93~0.90~24.27~0.90"
        "~24.61~0.90~24.94~0.90~25.28~0.90~25.62~0.90~25.96~0.90~26.29~0.90~26.63~0.90~26.97~0.90~27.30~0.90~27.64~0.90~2"
        "7.98~0.90~28.31~0.90~28.65~0.90~28.99~0.90~29.33~0.90~29.66~0.90~30.00~0.90~30.34~0.90~30.67~0.90~31.01~0.90~31."
        "35~0.90~31.69~0.90~32.02~0.90~32.36~1.00~32.36~0.90~32.70~0.90~33.03~0.90~33.71~0.90~34.04~0.90~34.38~0.90~34.72"
        "~0.90~35.06~0.90~35.39~0.90~35.73~0.90~36.40~0.90~36.74~0.90~37.08~0.90~37.42~0.90~37.75~0.90~38.09~0.90~38.43~0"
        ".90~39.10~0.90~39.44~0.90~39.78~0.90~40.11~0.90~40.45~0.90~40.79~0.90~41.12~0.90~41.80~0.90~42.13~0.90~42.47~0.9"
        "0~42.81~0.90~43.15~1.00~43.15~0.90~43.48~0.90~43.82~0.90~44.49~0.90~44.83~0.90~45.17~0.90~45.51~0.90~45.84~0.90~"
        "46.18~0.90~46.52~0.90~47.19~0.90~47.53~0.90~47.87~0.90~48.20~0.90~48.54~0.90~48.88~0.90~49.21~0.90~49.89~0.90~50"
        ".22~0.90~50.56~0.90~50.90~0.90~51.24~0.90~51.57~0.90~51.91~0.90~52.58~0.90~52.92~0.90~53.26~0.90~53.60~0.90~53.9"
        "3~1.00~53.93~0.90~54.27~0.90~54.61~0.90~54.94~0.90~55.28~0.90~55.62~0.90~55.96~0.90~56.29~0.90~56.63~0.90~56.97~"
        "0.90~57.30~0.90~57.64~0.90~57.98~0.90~58.31~0.90~58.65~0.90~59.33~0.90~59.66~0.90~60.00~0.90~60.34~0.90~60.67~0."
        "90~61.01~0.90~61.35~0.90~61.69~0.90~62.02~0.90~62.36~0.90~62.70~0.90~63.03~0.90~63.37~0.90~63.37~0.90~63.71~0.90"
        "~64.04~0.90~64.38~0.90~64.72~1.00~64.72~0.90~65.39~0.90~65.73~0.90~66.07~0.90~66.40~0.90~66.74~0.90~67.08~0.90~6"
        "7.42~0.90~68.09~0.90~68.43~0.90~68.76~0.90~69.10~0.90~69.44~0.90~69.78~0.90~70.11~0.90~70.79~0.90~71.12~0.90~71."
        "46~0.90~71.80~0.90~72.13~0.90~72.47~0.90~72.81~0.90~73.48~0.90~73.82~0.90~74.16~0.90~74.49~0.90~74.83~0.90~75.17"
        "~0.90~75.51~1.00~75.51~0.90~75.84~0.90~76.18~0.90~76.52~0.90~76.85~0.90~77.19~0.90~77.53~0.90~77.87~0.90~78.20~0"
        ".90~78.54~0.90~78.88~0.90~79.21~0.90~79.55~0.90~79.89~0.90~80.22~0.90~80.56~0.90~80.90~0.90~81.24~0.90~81.57~0.9"
        "0~81.91~0.90~82.25~0.90~82.58~0.90~82.92~0.90~83.26~0.90~83.60~0.90~83.93~0.90~84.27~0.90~84.61~0.90~84.94~0.90~"
        "85.28~0.90~85.62~0.90~85.96~0.90~86.29~1.00~86.29~0.90~86.63~0.90~86.97~0.90~87.30~0.90~87.64~0.90");
      returnVal.m_data.m_data = internal18;
      break;
    case 7:
      internal17 = internal;
      sub_D153B8(
        internal,
        "1.78~0.90~2.62~0.90~3.41~0.90~3.72~0.90~4.04~0.90~4.25~0.90~5.14~0.90~5.93~0.90~6.77~0.90~7.08~0.90~7.40~0.90~7."
        "60~0.90~8.50~0.90~9.28~0.90~10.12~0.90~10.44~0.90~10.75~0.90~11.01~0.90~11.38~0.90~11.80~0.90~12.64~0.90~13.48~0"
        ".90~13.90~0.90~14.32~0.90~15.10~1.00~15.21~0.90~16.00~0.90~16.84~0.90~17.15~0.90~17.47~0.90~17.67~0.90~18.62~0.9"
        "0~19.41~0.90~20.24~0.90~20.56~0.90~20.87~0.90~21.08~0.90~21.92~0.90~22.71~0.90~23.55~0.90~23.92~0.90~24.23~0.90~"
        "24.44~0.90~24.81~0.90~25.23~0.90~26.01~0.90~26.91~0.90~27.33~0.90~27.74~0.90~28.53~1.00~28.64~0.90~29.06~0.90~29"
        ".32~1.00~29.48~0.90~29.90~0.90~30.16~1.00~30.31~0.90~30.42~1.00~30.79~1.00~30.79~0.90~31.05~1.00~31.15~0.90~31.6"
        "3~0.90~31.89~1.00~31.99~0.90~32.41~0.90~32.67~1.00~32.83~0.90~33.30~0.90~33.51~1.00~33.72~0.90~33.83~1.00~34.14~"
        "1.00~34.14~0.90~34.35~1.00~34.51~0.90~34.93~0.90~35.24~1.00~35.35~0.90~35.77~0.90~36.03~1.00~36.19~0.90~36.66~0."
        "90~36.87~1.00~37.03~0.90~37.19~1.00~37.50~0.90~37.50~1.00~37.76~1.00~37.87~0.90~38.13~1.00~38.29~0.90~38.65~1.00"
        "~38.71~0.90~39.13~0.90~39.39~1.00~39.55~0.90~39.97~0.90~40.23~1.00~40.44~0.90~40.65~1.00~40.80~0.90~41.07~1.00~4"
        "1.28~0.90~41.70~0.90~41.91~1.00~42.12~0.90~42.53~0.90~42.74~1.00~42.90~0.90~43.32~0.90~43.58~1.00~43.79~0.90~43."
        "85~1.00~44.16~1.00~44.16~0.90~44.42~1.00~44.58~0.90~45.00~0.90~45.31~1.00~45.47~0.90~45.89~0.90~46.10~1.00~46.31"
        "~0.90~46.73~0.90~46.94~1.00~47.15~0.90~47.26~1.00~47.57~1.00~47.57~0.90~47.83~1.00~47.99~0.90~48.36~0.90~48.62~1"
        ".00~48.78~0.90~49.20~0.90~49.46~1.00~49.67~0.90~50.09~0.90~50.35~1.00~50.51~0.90~50.66~1.00~50.93~1.00~50.93~0.9"
        "0~51.19~1.00~51.35~0.90~51.56~1.00~51.71~0.90~51.98~1.00~52.13~0.90~52.60~0.90~52.76~1.00~52.97~0.90~53.39~0.90~"
        "53.65~1.00~53.81~0.90~54.07~1.00~54.28~0.90~54.49~1.00~54.65~0.90~55.38~1.00~55.49~0.90~55.91~0.90~56.38~0.90~56"
        ".75~0.90~57.17~0.90~57.59~0.90~58.01~0.90~58.43~0.90~58.85~0.90~59.27~0.90~59.63~0.90~60.10~0.90~60.47~0.90~60.8"
        "9~0.90~61.31~0.90~62.20~0.90~62.62~0.90~63.04~0.90~63.46~0.90~63.83~0.90~64.30~0.90~64.72~0.90~65.14~0.90~65.56~"
        "0.90~65.93~0.90~66.40~0.90~66.77~0.90~67.19~0.90~67.60~0.90~68.02~0.90~68.81~1.00~68.92~0.90~69.34~0.90~69.76~0."
        "90~70.17~0.90~70.59~0.90~71.01~0.90~71.43~0.90~71.85~0.90~72.27~0.90~72.69~0.90~73.11~0.90~73.58~0.90~74.00~0.90"
        "~74.37~0.90~74.84~0.90~75.63~0.90~76.05~0.90~76.52~0.90~76.89~0.90~77.31~0.90~77.73~0.90~78.20~0.90~78.62~0.90~7"
        "8.99~0.90~79.41~0.90~79.83~0.90~80.24~0.90~80.66~0.90~81.14~0.90~81.50~0.90~81.92~0.90~82.19~1.00~82.40~0.90~82."
        "60~0.90~82.97~0.90~83.02~1.00~83.18~0.90~83.39~0.90~83.86~0.90~83.86~1.00~84.02~0.90~84.23~0.90~84.65~0.90~84.76"
        "~1.00~84.86~0.90~85.07~0.90~85.28~0.90~85.59~1.00~85.59~0.90~85.80~0.90~85.96~0.90~86.38~1.00~86.38~0.90~86.59~0"
        ".90~86.75~0.90~87.17~0.90~87.27~1.00~87.38~0.90~87.59~0.90~88.01~0.90~88.06~1.00~88.22~0.90~88.43~0.90~88.69~0.9"
        "0~88.85~0.90~88.90~1.00~89.06~0.90~89.32~0.90~89.74~0.90~89.74~1.00~89.90~0.90~90.10~0.90~90.52~0.90~90.58~1.00~"
        "90.73~0.90~90.94~0.90~91.36~0.90~91.42~1.00~91.57~0.90~91.78~0.90~91.99~0.90~92.26~1.00~92.41~0.90~92.62~0.90~93"
        ".09~1.00~93.09~0.90~93.25~0.90~93.46~0.90~93.93~0.90~93.93~1.00~94.09~0.90~94.35~0.90~94.62~0.90~94.77~1.00~94.8"
        "3~0.90~94.98~0.90~95.35~0.90~95.56~0.90~95.61~1.00~95.77~0.90");
      returnVal.m_data.m_data = internal17;
      break;
    case 8:
      internal16 = internal;
      sub_D153B8(internal, a02108004308007);
      returnVal.m_data.m_data = internal16;
      break;
    case 9:
      internal15 = internal;
      sub_D153B8(internal, a02308003508006);
      returnVal.m_data.m_data = internal15;
      break;
    case 10:
      internal14 = internal;
      sub_D153B8(internal, a04210005309009);
      returnVal.m_data.m_data = internal14;
      break;
    case 11:
      internal13 = internal;
      sub_D153B8(internal, a04309007809009);
      returnVal.m_data.m_data = internal13;
      break;
    case 12:
      internal12 = internal;
      sub_D153B8(internal, a06509009909011);
      returnVal.m_data.m_data = internal12;
      break;
    case 13:
      internal11 = internal;
      sub_D153B8(internal, a02308004308006);
      returnVal.m_data.m_data = internal11;
      break;
    case 14:
      internal10 = internal;
      sub_D153B8(internal, a02509004309008);
      returnVal.m_data.m_data = internal10;
      break;
    case 15:
      internal9 = internal;
      sub_D153B8(internal, a02008005008010);
      returnVal.m_data.m_data = internal9;
      break;
    case 16:
      internal8 = internal;
      sub_D153B8(internal, a02009006307008);
      returnVal.m_data.m_data = internal8;
      break;
    case 17:
      internal7 = internal;
      sub_D153B8(internal, a02108004108008);
      returnVal.m_data.m_data = internal7;
      break;
    case 18:
      internal6 = internal;
      sub_D153B8(
        internal,
        "0.41~0.90~0.81~0.90~1.10~0.90~1.41~0.90~1.83~0.90~2.35~0.90~2.78~0.90~2.95~1.00~3.21~0.90~3.50~0.90~3.98~0.90~4."
        "45~0.90~4.85~0.90~5.10~0.90~5.60~0.90~6.01~0.90~6.10~1.00~6.63~0.90~6.88~0.90~7.15~0.90~7.50~0.90~7.85~0.90~8.06"
        "~0.90~8.30~0.90~8.48~0.90~8.80~0.90~9.06~0.90~9.30~0.90~9.68~0.90~9.71~1.00~10.28~0.90~11.08~0.90~11.41~0.90~11."
        "71~0.90~11.95~0.90~12.15~0.90~12.33~0.90~12.51~0.90~12.60~1.00~13.13~0.90~13.31~0.90~13.60~0.90~13.93~0.90~14.15"
        "~0.90~14.38~0.90~14.58~0.90~14.76~0.90~14.98~0.90~15.26~0.90~15.53~0.90~15.76~0.90~15.83~1.00~16.15~0.90~16.73~0"
        ".90~17.38~0.90~17.63~0.90~17.95~0.90~18.15~0.90~18.58~0.90~18.96~0.90~19.06~1.00~19.11~0.90~19.40~0.90~20.01~0.9"
        "0~20.35~0.90~20.81~0.90~21.11~0.90~21.41~0.90~21.73~0.90~22.05~0.90~22.46~0.90~22.76~0.90~23.06~0.90~23.35~0.90~"
        "23.65~0.90~24.06~0.90~24.36~0.90~24.68~0.90~25.00~0.90~25.48~0.90~25.58~1.00~25.68~0.90~25.93~0.90~26.33~0.90~26"
        ".76~0.90~27.15~0.90~27.51~0.90~27.93~0.90~28.25~0.90~28.75~0.90~28.81~1.00~29.08~0.90~29.56~0.90~29.88~0.90~30.1"
        "3~0.90~30.38~0.90~30.80~0.90~31.15~0.90~31.58~0.90~31.60~1.00~31.80~0.90~32.01~0.90~32.18~0.90~32.40~0.90~33.78~"
        "0.90~34.16~0.90~34.40~0.90~35.23~0.90~35.30~1.00~35.61~0.90~36.03~0.90~36.31~0.90~36.88~0.90~37.15~0.90~37.46~0."
        "90~37.66~0.90~38.05~0.90~38.48~0.90~38.53~1.00~38.78~0.90~39.11~0.90~39.30~0.90~39.61~0.90~39.91~0.90~40.11~0.90"
        "~40.41~0.90~40.75~0.90~40.93~0.90~41.13~0.90~41.31~0.90~41.73~0.90~41.78~1.00~42.03~0.90~42.36~0.90~42.56~0.90~4"
        "2.85~0.90~43.16~0.90~43.38~0.90~43.66~0.90~43.96~0.90~44.15~0.90~44.38~0.90~44.60~0.90~44.96~0.90~45.05~1.00~45."
        "91~0.90~46.15~0.90~46.35~0.90~46.43~1.00~47.53~0.90~47.75~0.90~48.05~1.00~48.16~0.90~48.48~0.90~48.80~0.90~48.98"
        "~0.90~49.35~0.90~49.66~0.90~49.83~0.90~50.15~0.90~50.46~0.90~50.65~0.90~50.88~0.90~51.08~0.90~51.46~0.90~51.51~1"
        ".00~51.86~0.90~52.85~0.90~53.08~0.90~53.30~0.90~53.50~0.90~53.86~1.00~53.91~0.90~54.70~0.90~55.06~0.90~56.83~0.9"
        "0~57.06~0.90~57.50~0.90~57.93~0.90~58.03~1.00~58.91~0.90~59.11~0.90~59.33~0.90~59.40~1.00~60.66~0.90~60.83~0.90~"
        "60.98~1.00~61.01~0.90~61.71~0.90~61.95~0.90~62.26~0.90~62.58~0.90~62.76~0.90~63.10~0.90~63.43~0.90~63.61~0.90~63"
        ".98~0.90~64.41~0.90~64.53~1.00~64.55~0.90~64.68~0.90~64.93~0.90~65.26~0.90~65.56~0.90~65.88~0.90~66.08~1.00~66.0"
        "8~0.90~66.23~0.90~66.35~0.90~66.45~0.90~66.86~0.90~67.11~0.90~67.31~0.90~67.68~0.90~67.71~1.00~67.83~0.90~67.95~"
        "0.90~68.18~0.90~68.51~0.90~68.80~0.90~69.25~0.90~69.35~1.00~69.43~0.90~69.60~0.90~70.01~0.90~70.30~0.90~70.51~0."
        "90~70.73~0.90~70.93~0.90~70.95~1.00~71.08~0.90~71.33~0.90~71.53~0.90~71.75~0.90~72.03~0.90~72.51~0.90~72.61~1.00"
        "~72.65~0.90~72.73~0.90~73.00~0.90~73.38~0.90~73.61~0.90~73.75~0.90~74.15~0.90~74.21~1.00~74.26~0.90~74.40~0.90~7"
        "4.61~0.90~74.83~0.90~75.03~0.90~75.30~0.90~75.68~0.90~75.83~1.00~75.83~0.90~75.96~0.90~76.10~0.90~76.46~0.90~76."
        "58~0.90~76.81~0.90~77.01~0.90~77.26~0.90~77.43~0.90~77.45~1.00~77.98~0.90~78.21~0.90~78.51~0.90~78.78~0.90~79.00"
        "~0.90~79.05~1.00~79.30~0.90~79.68~0.90~79.88~1.00~79.88~0.90~80.06~0.90~80.23~1.00~80.28~0.90~80.46~0.90~80.50~1"
        ".00~80.66~0.90~81.01~0.90~81.30~0.90~81.50~0.90~81.80~0.90~82.05~0.90~82.28~0.90~82.33~1.00~82.58~0.90~82.88~0.9"
        "0~83.08~1.00~83.11~0.90~83.30~0.90~83.53~0.90~83.55~1.00~83.73~0.90~83.91~0.90~83.93~1.00~84.25~0.90~84.50~0.90~"
        "84.71~0.90~85.05~0.90~85.36~0.90~85.56~0.90~85.58~1.00~85.81~0.90~86.13~0.90~86.36~1.00~86.36~0.90~86.55~0.90~86"
        ".73~0.90~86.96~0.90~87.16~0.90~87.43~1.00~87.48~0.90~87.78~0.90~87.96~0.90~88.26~0.90~88.58~0.90~88.76~0.90~88.8"
        "6~1.00~89.06~0.90~89.35~0.90~89.58~0.90~89.60~1.00~89.80~0.90~90.01~1.00~90.03~0.90~90.55~0.90~90.60~1.00~91.00~"
        "0.90~91.13~1.00~91.33~0.90~92.10~0.90~92.78~0.90~92.85~1.00~93.60~0.90~93.66~1.00~93.75~0.90~94.01~0.90~94.40~0."
        "90~94.70~0.90~95.20~0.90~95.31~1.00~95.50~0.90~95.85~0.90~96.00~0.90~96.06~1.00~96.43~0.90~96.46~1.00~96.81~0.90~96.88~1.00");
      returnVal.m_data.m_data = internal6;
      break;
    case 19:
      internal5 = internal;
      sub_D153B8(internal, a03009005509009);
      returnVal.m_data.m_data = internal5;
      break;
    case 20:
      internal4 = internal;
      sub_D153B8(
        internal,
        "0.92~0.9~1.08~0.9~1.67~0.9~2.07~0.9~2.23~0.9~2.99~0.9~3.27~0.9~3.82~0.9~4.30~0.9~4.85~0.9~5.10~0.9~5.60~0.9~6.49"
        "~0.9~6.85~0.9~7.27~0.9~7.52~0.9~7.82~0.9~7.97~0.9~8.10~0.9~8.25~0.9~8.64~0.9~9.44~0.9~9.59~0.9~10.24~0.9~10.64~0"
        ".9~10.79~0.9~11.82~0.9~12.84~0.9~13.44~0.9~13.67~0.9~14.24~0.9~15.02~0.9~15.40~0.9~15.80~0.9~16.08~0.9~17.17~0.9"
        "~17.55~0.9~17.99~0.9~18.39~0.9~19.40~0.9~19.74~0.9~20.14~0.9~20.49~0.9~20.92~0.9~21.10~0.9~21.24~0.9~21.52~0.9~2"
        "1.89~0.9~22.29~0.9~22.65~0.9~23.09~0.9~23.69~0.9~24.02~0.9~24.45~0.9~24.82~0.9~25.77~0.9~26.17~0.9~26.59~0.9~26."
        "99~0.9~27.39~0.9~27.64~0.9~27.90~0.9~28.32~0.9~28.74~0.9~29.10~0.9~29.54~0.9~29.80~0.9~30.07~0.9~30.34~0.9~30.59"
        "~0.9~30.87~0.9~31.17~0.9~31.42~0.9~31.67~0.9~31.94~0.9~32.22~0.9~32.37~0.9~32.50~0.9~32.64~0.9~32.77~0.9~32.91~0"
        ".9~33.02~0.9~33.15~0.9~33.29~0.9~33.39~0.9~33.45~0.9~33.55~0.9~33.60~0.9~33.72~0.9~33.80~0.9~34.37~0.9~34.64~0.9"
        "~35.19~0.9~35.35~0.9~35.47~0.9~35.84~0.9~36.24~0.9~36.74~0.9~36.99~0.9~37.55~0.9~37.74~0.9~38.02~0.9~38.39~0.9~3"
        "8.94~0.9~39.20~0.9~39.72~0.9~39.89~0.9~40.14~0.9~40.54~0.9~40.80~0.9~41.21~0.9~41.57~0.9~41.84~0.9~42.22~0.9~42."
        "64~0.9~42.96~0.9~43.49~0.9~43.97~0.9~44.14~0.9~44.31~0.9~44.44~0.9~44.84~0.9~45.34~0.9~45.61~0.9~46.14~0.9~46.29"
        "~0.9~46.57~0.9~46.97~0.9~47.47~0.9~47.74~0.9~48.27~0.9~48.44~0.9~48.71~0.9~49.11~0.9~49.37~0.9~49.72~0.9~50.13~0"
        ".9~50.39~0.9~51.49~0.9~51.99~0.9~52.51~0.9~53.09~0.9~53.34~0.9~53.91~0.9~54.17~0.9~54.44~0.9~55.01~0.9~55.26~0.9"
        "~55.76~0.9~56.29~0.9~56.78~0.9~57.36~0.9~57.61~0.9~58.18~0.9~58.44~0.9~58.69~0.9~59.26~0.9~59.51~0.9~59.74~0.9~6"
        "0.06~0.9~60.56~0.9~61.06~0.9~61.64~0.9~61.93~0.9~62.48~0.9~62.74~0.9~63.04~0.9~63.56~0.9~63.85~0.9~64.36~0.9~64."
        "86~0.9~65.41~0.9~65.98~0.9~66.24~0.9~66.80~0.9~67.04~0.9~67.29~0.9~67.84~0.9~68.13~0.9~68.64~0.9~70.73~0.9~71.83"
        "~0.9~72.88~0.9~73.44~0.9~73.71~0.9~73.86~0.9~74.11~0.9~74.53~0.9~74.81~0.9~74.98~0.9~75.21~0.9~75.79~0.9~75.94~0"
        ".9~77.46~0.9~77.71~0.9~78.08~0.9~78.23~0.9~78.73~0.9~78.89~0.9~79.08~0.9~79.24~0.9~79.59~0.9~79.75~0.9~80.11~0.9"
        "~80.38~0.9~80.93~0.9~81.46~0.9~81.75~0.9~81.98~0.9~82.28~0.9~82.51~0.9~83.13~0.9~83.40~0.9~83.56~0.9~83.83~0.9~8"
        "4.26~0.9~84.41~0.9~84.70~0.9~85.25~0.9");
      returnVal.m_data.m_data = internal4;
      break;
    case 23:
      internal3 = internal;
      sub_D153B8(
        internal,
        "0.20~0.9~0.55~0.9~1.00~0.9~1.45~0.9~1.87~0.9~2.28~0.9~2.73~0.9~3.15~0.9~3.57~0.9~4.00~0.9~4.43~0.9~4.85~0.9~5.28"
        "~0.9~5.72~0.9~6.12~0.9~6.57~0.9~7.00~0.9~7.43~0.9~7.85~0.9~8.27~0.9~8.68~0.9~9.12~0.9~9.57~0.9~9.98~0.9~10.42~0."
        "9~10.85~0.9~11.28~0.9~11.72~0.9~12.12~0.9~12.57~0.9~13.02~0.9~13.47~0.9~13.87~0.9~14.28~0.9~14.70~0.9~15.15~0.9~"
        "15.57~0.9~15.98~0.9~16.40~0.9~16.83~0.9~17.27~0.9~17.72~0.9~18.10~0.9~18.55~0.9~18.98~0.9~19.42~0.9~19.85~0.9~20"
        ".28~0.9~20.72~0.9~21.13~0.9~21.55~0.9~21.95~0.9~22.43~0.9~22.88~0.9~23.28~0.9~23.68~0.9~24.12~0.9~24.57~0.9~24.9"
        "8~0.9~25.42~0.9~25.80~0.9~26.27~0.9~26.68~0.9~27.12~0.9~27.55~0.9~27.95~0.9~28.38~0.9~28.83~0.9~29.25~0.9~29.68~"
        "0.9~30.12~0.9~30.55~0.9~30.97~0.9~31.42~0.9~31.82~0.9~32.25~0.9~32.68~0.9~33.12~0.9~33.55~0.9~34.00~0.9~34.40~0."
        "9~34.83~0.9~35.25~0.9~35.70~0.9~36.13~0.9~36.55~0.9~36.97~0.9~37.42~0.9~37.82~0.9~38.23~0.9~38.70~0.9~39.12~0.9~"
        "39.55~0.9~40.00~0.9~40.42~0.9~40.85~0.9~41.32~0.9~41.70~0.9~42.12~0.9~42.55~0.9~42.98~0.9~43.42~0.9~43.83~0.9~44"
        ".25~0.9~44.67~0.9~45.12~0.9~45.57~0.9~46.00~0.9~46.38~0.9~46.83~0.9~47.27~0.9~47.70~0.9~48.13~0.9~48.55~0.9~48.9"
        "7~0.9~49.42~0.9~49.85~0.9~50.27~0.9~50.68~0.9~51.12~0.9~51.55~0.9~51.98~0.9~52.40~0.9~52.82~0.9~53.25~0.9~53.70~"
        "0.9~54.13~0.9~54.55~0.9~54.98~0.9~55.42~0.9~55.85~0.9~56.28~0.9~56.68~0.9~57.13~0.9~57.58~0.9~58.02~0.9~58.45~0."
        "9~58.83~0.9~59.28~0.9~59.70~0.9~60.17~0.9~60.57~0.9~60.98~0.9~61.43~0.9~61.83~0.9~62.27~0.9~62.70~0.9~63.13~0.9~"
        "63.58~0.9~63.98~0.9~64.38~0.9~64.82~0.9~65.27~0.9~65.70~0.9~66.15~0.9~66.57~0.9~67.00~0.9~67.43~0.9~67.85~0.9~68"
        ".32~0.9~68.72~0.9~69.15~0.9~69.57~0.9~70.02~0.9~70.43~0.9~70.85~0.9~71.27~0.9~71.70~0.9~72.12~0.9~72.57~0.9~73.0"
        "0~0.9~73.42~0.9~73.83~0.9~74.25~0.9~74.68~0.9~75.10~0.9~75.53~0.9~75.97~0.9~76.42~0.9~76.85~0.9~77.23~0.9~77.70~"
        "0.9~78.13~0.9~78.53~0.9~79.00~0.9~79.42~0.9~79.87~0.9~80.27~0.9~80.67~0.9~81.10~0.9~81.57~0.9~81.98~0.9~82.40~0."
        "9~82.83~0.9~83.23~0.9~83.70~0.9~84.15~0.9~84.55~0.9~84.95~0.9~85.42~0.9~85.82~0.9~86.27~0.9~86.75~0.9~87.15~0.9~"
        "87.58~0.9~87.98~0.9~88.40~0.9~88.83~0.9~89.27~0.9~89.73~0.9~90.13~0.9~90.58~0.9~90.98~0.9");
      returnVal.m_data.m_data = internal3;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, &byte_F288A0);
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


std::string LevelTools::getAudioTitle(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;
  stl::StringData<char>::Internal *internal14;
  stl::StringData<char>::Internal *internal15;
  stl::StringData<char>::Internal *internal16;
  stl::StringData<char>::Internal *internal17;
  stl::StringData<char>::Internal *internal18;
  stl::StringData<char>::Internal *internal19;
  stl::StringData<char>::Internal *internal20;
  stl::StringData<char>::Internal *internal21;
  stl::StringData<char>::Internal *internal22;
  stl::StringData<char>::Internal *internal23;
  stl::StringData<char>::Internal *internal24;
  stl::StringData<char>::Internal *internal25;
  stl::StringData<char>::Internal *internal26;
  stl::StringData<char>::Internal *internal27;
  stl::StringData<char>::Internal *internal28;
  stl::StringData<char>::Internal *internal29;
  stl::StringData<char>::Internal *internal30;
  stl::StringData<char>::Internal *internal31;
  stl::StringData<char>::Internal *internal32;
  stl::StringData<char>::Internal *internal33;
  stl::StringData<char>::Internal *internal34;
  stl::StringData<char>::Internal *internal35;
  stl::StringData<char>::Internal *internal36;
  stl::StringData<char>::Internal *internal37;
  stl::StringData<char>::Internal *internal38;
  stl::StringData<char>::Internal *internal39;
  stl::StringData<char>::Internal *internal40;
  stl::StringData<char>::Internal *internal41;
  stl::StringData<char>::Internal *internal42;
  stl::StringData<char>::Internal *internal43;

  switch ( id )
  {
    case -1:
      internal42 = internal;
      sub_D153B8(internal, "Practice: Stay Inside Me");
      returnVal.m_data.m_data = internal42;
      break;
    case 0:
      internal41 = internal;
      sub_D153B8(internal, "Stereo Madness");
      returnVal.m_data.m_data = internal41;
      break;
    case 1:
      internal40 = internal;
      sub_D153B8(internal, "Back On Track");
      returnVal.m_data.m_data = internal40;
      break;
    case 2:
      internal39 = internal;
      sub_D153B8(internal, "Polargeist");
      returnVal.m_data.m_data = internal39;
      break;
    case 3:
      internal38 = internal;
      sub_D153B8(internal, "Dry Out");
      returnVal.m_data.m_data = internal38;
      break;
    case 4:
      internal37 = internal;
      sub_D153B8(internal, "Base After Base");
      returnVal.m_data.m_data = internal37;
      break;
    case 5:
      internal36 = internal;
      sub_D153B8(internal, "Cant Let Go");
      returnVal.m_data.m_data = internal36;
      break;
    case 6:
      internal35 = internal;
      sub_D153B8(internal, "Jumper");
      returnVal.m_data.m_data = internal35;
      break;
    case 7:
      internal34 = internal;
      sub_D153B8(internal, "Time Machine");
      returnVal.m_data.m_data = internal34;
      break;
    case 8:
      internal33 = internal;
      sub_D153B8(internal, "Cycles");
      returnVal.m_data.m_data = internal33;
      break;
    case 9:
      internal32 = internal;
      sub_D153B8(internal, "xStep");
      returnVal.m_data.m_data = internal32;
      break;
    case 10:
      internal31 = internal;
      sub_D153B8(internal, "Clutterfunk");
      returnVal.m_data.m_data = internal31;
      break;
    case 11:
      internal30 = internal;
      sub_D153B8(internal, "Theory of Everything");
      returnVal.m_data.m_data = internal30;
      break;
    case 12:
      internal29 = internal;
      sub_D153B8(internal, "Electroman Adventures");
      returnVal.m_data.m_data = internal29;
      break;
    case 13:
      internal28 = internal;
      sub_D153B8(internal, "Clubstep");
      returnVal.m_data.m_data = internal28;
      break;
    case 14:
      internal27 = internal;
      sub_D153B8(internal, "Electrodynamix");
      returnVal.m_data.m_data = internal27;
      break;
    case 15:
      internal26 = internal;
      sub_D153B8(internal, "Hexagon Force");
      returnVal.m_data.m_data = internal26;
      break;
    case 16:
      internal25 = internal;
      sub_D153B8(internal, "Blast Processing");
      returnVal.m_data.m_data = internal25;
      break;
    case 17:
      internal24 = internal;
      sub_D153B8(internal, "Theory of Everything 2");
      returnVal.m_data.m_data = internal24;
      break;
    case 18:
      internal23 = internal;
      sub_D153B8(internal, "Geometrical Dominator");
      returnVal.m_data.m_data = internal23;
      break;
    case 19:
      internal22 = internal;
      sub_D153B8(internal, "Deadlocked");
      returnVal.m_data.m_data = internal22;
      break;
    case 20:
      internal21 = internal;
      sub_D153B8(internal, "Fingerdash");
      returnVal.m_data.m_data = internal21;
      break;
    case 21:
      internal20 = internal;
      sub_D153B8(internal, "Dash");
      returnVal.m_data.m_data = internal20;
      break;
    case 22:
      internal19 = internal;
      sub_D153B8(internal, "Explorers");
      returnVal.m_data.m_data = internal19;
      break;
    case 23:
      internal18 = internal;
      sub_D153B8(internal, "The Seven Seas");
      returnVal.m_data.m_data = internal18;
      break;
    case 24:
      internal17 = internal;
      sub_D153B8(internal, "Viking Arena");
      returnVal.m_data.m_data = internal17;
      break;
    case 25:
      internal16 = internal;
      sub_D153B8(internal, "Airborne Robots");
      returnVal.m_data.m_data = internal16;
      break;
    case 26:
      internal15 = internal;
      sub_D153B8(internal, "Secret");
      returnVal.m_data.m_data = internal15;
      break;
    case 27:
      internal14 = internal;
      sub_D153B8(internal, "Payload");
      returnVal.m_data.m_data = internal14;
      break;
    case 28:
      internal13 = internal;
      sub_D153B8(internal, "Beast Mode");
      returnVal.m_data.m_data = internal13;
      break;
    case 29:
      internal12 = internal;
      sub_D153B8(internal, "Machina");
      returnVal.m_data.m_data = internal12;
      break;
    case 30:
      internal11 = internal;
      sub_D153B8(internal, "Years");
      returnVal.m_data.m_data = internal11;
      break;
    case 31:
      internal10 = internal;
      sub_D153B8(internal, "Frontlines");
      returnVal.m_data.m_data = internal10;
      break;
    case 32:
      internal9 = internal;
      sub_D153B8(internal, "Space Pirates");
      returnVal.m_data.m_data = internal9;
      break;
    case 33:
      internal8 = internal;
      sub_D153B8(internal, "Striker");
      returnVal.m_data.m_data = internal8;
      break;
    case 34:
      internal7 = internal;
      sub_D153B8(internal, "Embers");
      returnVal.m_data.m_data = internal7;
      break;
    case 35:
      internal6 = internal;
      sub_D153B8(internal, "Round 1");
      returnVal.m_data.m_data = internal6;
      break;
    case 36:
      internal5 = internal;
      sub_D153B8(internal, "Monster Dance Off");
      returnVal.m_data.m_data = internal5;
      break;
    case 37:
      internal4 = internal;
      sub_D153B8(internal, "Press Start");
      returnVal.m_data.m_data = internal4;
      break;
    case 38:
      internal3 = internal;
      sub_D153B8(internal, "Nock Em");
      returnVal.m_data.m_data = internal3;
      break;
    case 39:
      internal43 = internal;
      sub_D153B8(internal, "Power Trip");
      returnVal.m_data.m_data = internal43;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, "Unknown");
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


bool LevelTools::getLastGameplayReversed()
{
  return byte_1241F81;
}


bool LevelTools::getLastGameplayRotated()
{
  return byte_1241F80;
}


float LevelTools::getLastTimewarp()
{
  return *(float *)&dword_122F014;
}


GJGameLevel * LevelTools::getLevel(int id, bool noString)
{
  _BOOL4 flag;
  __int64 int64Val;
  void *voidPtr;
  int intVal;
  __int64 int64Val2;
  void *voidPtr2;
  unsigned int *intPtr;
  int intVal2;
  void *voidPtr3;
  int intVal3;
  int intVal4;
  int intVal5;
  unsigned int *intPtr2;
  void *voidPtr4;
  int intVal6;
  unsigned int *intPtr3;
  void *voidPtr5;
  int intVal7;
  __int64 int64Val3;
  void *voidPtr6;
  LocalLevelManager *localLevelManager;
  void *voidPtr7;
  int intVal8;
  float value;
  GJGameLevel *returnVal;
  void *voidPtr8;
  int intVal9;
  unsigned int *intPtr4;
  void *voidPtr9;
  int intVal10;
  unsigned int *intPtr5;
  void *voidPtr10;
  int intVal11;
  unsigned int *intPtr6;
  void *voidPtr11;
  int intVal12;
  unsigned int *intPtr7;
  void *voidPtr12;
  int intVal13;
  unsigned int *intPtr8;
  void *voidPtr13;
  int intVal14;
  unsigned int *intPtr9;
  void *voidPtr14;
  int intVal15;
  unsigned int *intPtr10;
  void *voidPtr15;
  int intVal16;
  unsigned int *intPtr11;
  void *voidPtr16;
  int intVal17;
  unsigned int *intPtr12;
  void *voidPtr17;
  int intVal18;
  void *voidPtr18;
  void *voidPtr19;
  int intVal19;
  unsigned int *intPtr13;
  void *voidPtr20;
  int intVal20;
  unsigned int *intPtr14;
  void *voidPtr21;
  int intVal21;
  unsigned int *intPtr15;
  int intVal22;
  void *voidPtr22;
  int intVal23;
  unsigned int *intPtr16;
  void *voidPtr23;
  int intVal24;
  unsigned int *intPtr17;
  void *voidPtr24;
  int intVal25;
  int intVal26;
  int intVal27;
  unsigned int *intPtr18;
  unsigned int *intPtr19;
  int intVal28;
  void *voidPtr25;
  int intVal29;
  unsigned int *intPtr20;
  void *voidPtr26;
  int intVal30;
  unsigned int *intPtr21;
  void *voidPtr27;
  int intVal31;
  unsigned int *intPtr22;
  void *voidPtr28;
  int intVal32;
  int intVal33;
  int intVal34;
  unsigned int *intPtr23;
  void *voidPtr29;
  int intVal35;
  unsigned int *intPtr24;
  void *voidPtr30;
  int intVal36;
  unsigned int *intPtr25;
  void *voidPtr31;
  int intVal37;
  unsigned int *intPtr26;
  unsigned int *intPtr27;
  int intVal38;
  unsigned int *intPtr28;
  int intVal39;
  unsigned int *intPtr29;
  int intVal40;
  unsigned int *intPtr30;
  int intVal41;
  unsigned int *intPtr31;
  int intVal42;
  unsigned int *intPtr32;
  int intVal43;
  unsigned int *intPtr33;
  int intVal44;
  unsigned int *intPtr34;
  int intVal45;
  unsigned int *intPtr35;
  int intVal46;
  unsigned int *intPtr36;
  int intVal47;
  unsigned int *intPtr37;
  int intVal48;
  unsigned int *intPtr38;
  int intVal49;
  unsigned int *intPtr39;
  int intVal50;
  unsigned int *intPtr40;
  unsigned int *intPtr41;
  int intVal51;
  unsigned int *intPtr42;
  int intVal52;
  unsigned int *intPtr43;
  int intVal53;
  unsigned int *intPtr44;
  int intVal54;
  unsigned int *intPtr45;
  int intVal55;
  unsigned int *intPtr46;
  int intVal56;
  unsigned int *intPtr47;
  int intVal57;
  unsigned int *intPtr48;
  int intVal58;
  unsigned int *intPtr49;
  int intVal59;
  unsigned int *intPtr50;
  int intVal60;
  unsigned int *intPtr51;
  int intVal61;
  unsigned int *intPtr52;
  int intVal62;
  unsigned int *intPtr53;
  int intVal63;
  unsigned int *intPtr54;
  int intVal64;
  unsigned int *intPtr55;
  int intVal65;
  __int64 int64Val4;

  flag = noString;
  int64Val = GJGameLevel::create(_stack_chk_guard);
  if ( id == 14 )
  {
    LevelTools::getAudioTitle((int)&byte_9[4]);
    sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
    voidPtr28 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr39 = (unsigned int *)(int64Val4 - 8);
        do
          intVal50 = __ldaxr(intPtr39);
        while ( __stlxr(intVal50 - 1, intPtr39) );
      }
      else
      {
        intVal50 = *(_DWORD *)(int64Val4 - 8);
        *(_DWORD *)(int64Val4 - 8) = intVal50 - 1;
      }
      if ( intVal50 <= 0 )
        operator delete(voidPtr28);
    }
    *(_DWORD *)(int64Val + 452) = 13;
    *(_DWORD *)(int64Val + 448) = 6;
    *(_DWORD *)(int64Val + 696) = 14;
    intVal32 = rand();
    *(_DWORD *)(int64Val + 836) = 10;
    *(_DWORD *)(int64Val + 680) = 1;
    intVal33 = (int)(float)((float)((float)intVal32 * 4.6566e-10) * 1000.0);
    *(_DWORD *)(int64Val + 692) = intVal33;
    *(_DWORD *)(int64Val + 688) = intVal33 + 14;
    intVal34 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
    *(_DWORD *)(int64Val + 676) = intVal34;
    *(_DWORD *)(int64Val + 672) = intVal34 + 1;
    sub_D153B8(&int64Val4, "29_237_29_54_29_29_29_132_29_29_98_29_132_29_29_29");
    sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
    int64Val2 = int64Val4;
    voidPtr2 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
      goto LABEL_48;
    if ( !&pthread_create )
      goto LABEL_202;
    intPtr23 = (unsigned int *)(int64Val4 - 8);
    do
      intVal2 = __ldaxr(intPtr23);
    while ( __stlxr(intVal2 - 1, intPtr23) );
    goto LABEL_67;
  }
  if ( id <= 14 )
  {
    if ( id == 7 )
    {
      LevelTools::getAudioTitle((int)byte_6);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr27 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr30 = (unsigned int *)(int64Val4 - 8);
          do
            intVal41 = __ldaxr(intPtr30);
          while ( __stlxr(intVal41 - 1, intPtr30) );
        }
        else
        {
          intVal41 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal41 - 1;
        }
        if ( intVal41 <= 0 )
          operator delete(voidPtr27);
      }
      *(_DWORD *)(int64Val + 452) = 6;
      *(_DWORD *)(int64Val + 448) = 4;
      *(_DWORD *)(int64Val + 696) = 7;
      intVal31 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal31;
      *(_DWORD *)(int64Val + 688) = intVal31 + 7;
      sub_D153B8(&int64Val4, "29_98_29_40_29_29_29_29_29_29_98_29_98_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr22 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr22);
      while ( __stlxr(intVal2 - 1, intPtr22) );
      goto LABEL_67;
    }
    if ( id > 7 )
    {
      if ( id == 10 )
      {
        LevelTools::getAudioTitle((int)byte_9);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr22 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr49 = (unsigned int *)(int64Val4 - 8);
            do
              intVal59 = __ldaxr(intPtr49);
            while ( __stlxr(intVal59 - 1, intPtr49) );
          }
          else
          {
            intVal59 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal59 - 1;
          }
          if ( intVal59 <= 0 )
            operator delete(voidPtr22);
        }
        *(_DWORD *)(int64Val + 452) = 9;
        *(_DWORD *)(int64Val + 448) = 5;
        *(_DWORD *)(int64Val + 696) = 10;
        intVal23 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal23;
        *(_DWORD *)(int64Val + 688) = intVal23 + 10;
        sub_D153B8(&int64Val4, "29_132_29_40_29_29_29_54_29_29_132_29_73_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr16 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr16);
        while ( __stlxr(intVal2 - 1, intPtr16) );
      }
      else if ( id <= 10 )
      {
        if ( id == 8 )
        {
          LevelTools::getAudioTitle((int)byte_7);
          sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
          voidPtr13 = (void *)(int64Val4 - 24);
          if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
          {
            if ( &pthread_create )
            {
              intPtr46 = (unsigned int *)(int64Val4 - 8);
              do
                intVal56 = __ldaxr(intPtr46);
              while ( __stlxr(intVal56 - 1, intPtr46) );
            }
            else
            {
              intVal56 = *(_DWORD *)(int64Val4 - 8);
              *(_DWORD *)(int64Val4 - 8) = intVal56 - 1;
            }
            if ( intVal56 <= 0 )
              operator delete(voidPtr13);
          }
          *(_DWORD *)(int64Val + 452) = 7;
          *(_DWORD *)(int64Val + 448) = 4;
          *(_DWORD *)(int64Val + 696) = 8;
          intVal14 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
          *(_DWORD *)(int64Val + 692) = intVal14;
          *(_DWORD *)(int64Val + 688) = intVal14 + 8;
          sub_D153B8(&int64Val4, "29_132_29_40_29_29_29_29_29_29_73_29_98_29_29_29");
          sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
          int64Val2 = int64Val4;
          voidPtr2 = (void *)(int64Val4 - 24);
          if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
            goto LABEL_48;
          if ( !&pthread_create )
            goto LABEL_202;
          intPtr9 = (unsigned int *)(int64Val4 - 8);
          do
            intVal2 = __ldaxr(intPtr9);
          while ( __stlxr(intVal2 - 1, intPtr9) );
        }
        else
        {
          LevelTools::getAudioTitle((int)byte_8);
          sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
          voidPtr8 = (void *)(int64Val4 - 24);
          if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
          {
            if ( &pthread_create )
            {
              intPtr54 = (unsigned int *)(int64Val4 - 8);
              do
                intVal64 = __ldaxr(intPtr54);
              while ( __stlxr(intVal64 - 1, intPtr54) );
            }
            else
            {
              intVal64 = *(_DWORD *)(int64Val4 - 8);
              *(_DWORD *)(int64Val4 - 8) = intVal64 - 1;
            }
            if ( intVal64 <= 0 )
              operator delete(voidPtr8);
          }
          *(_DWORD *)(int64Val + 452) = 8;
          *(_DWORD *)(int64Val + 448) = 4;
          *(_DWORD *)(int64Val + 696) = 9;
          intVal9 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
          *(_DWORD *)(int64Val + 692) = intVal9;
          *(_DWORD *)(int64Val + 688) = intVal9 + 9;
          sub_D153B8(&int64Val4, "29_98_29_40_29_29_29_40_29_29_98_29_98_29_29_29");
          sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
          int64Val2 = int64Val4;
          voidPtr2 = (void *)(int64Val4 - 24);
          if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
            goto LABEL_48;
          if ( !&pthread_create )
            goto LABEL_202;
          intPtr4 = (unsigned int *)(int64Val4 - 8);
          do
            intVal2 = __ldaxr(intPtr4);
          while ( __stlxr(intVal2 - 1, intPtr4) );
        }
      }
      else if ( id == 12 )
      {
        LevelTools::getAudioTitle((int)&byte_9[2]);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr25 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr42 = (unsigned int *)(int64Val4 - 8);
            do
              intVal52 = __ldaxr(intPtr42);
            while ( __stlxr(intVal52 - 1, intPtr42) );
          }
          else
          {
            intVal52 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal52 - 1;
          }
          if ( intVal52 <= 0 )
            operator delete(voidPtr25);
        }
        *(_DWORD *)(int64Val + 452) = 11;
        *(_DWORD *)(int64Val + 448) = 5;
        *(_DWORD *)(int64Val + 696) = 12;
        intVal29 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal29;
        *(_DWORD *)(int64Val + 688) = intVal29 + 12;
        sub_D153B8(&int64Val4, "29_237_29_54_29_29_29_73_29_29_132_29_132_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr20 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr20);
        while ( __stlxr(intVal2 - 1, intPtr20) );
      }
      else if ( id <= 12 )
      {
        LevelTools::getAudioTitle((int)&byte_9[1]);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr4 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr35 = (unsigned int *)(int64Val4 - 8);
            do
              intVal46 = __ldaxr(intPtr35);
            while ( __stlxr(intVal46 - 1, intPtr35) );
          }
          else
          {
            intVal46 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal46 - 1;
          }
          if ( intVal46 <= 0 )
            operator delete(voidPtr4);
        }
        *(_DWORD *)(int64Val + 452) = 10;
        *(_DWORD *)(int64Val + 448) = 5;
        *(_DWORD *)(int64Val + 696) = 11;
        intVal6 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal6;
        *(_DWORD *)(int64Val + 688) = intVal6 + 11;
        sub_D153B8(&int64Val4, "29_237_29_40_29_29_29_73_29_29_29_29_98_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr3 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr3);
        while ( __stlxr(intVal2 - 1, intPtr3) );
      }
      else
      {
        LevelTools::getAudioTitle((int)&byte_9[3]);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr19 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr45 = (unsigned int *)(int64Val4 - 8);
            do
              intVal55 = __ldaxr(intPtr45);
            while ( __stlxr(intVal55 - 1, intPtr45) );
          }
          else
          {
            intVal55 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal55 - 1;
          }
          if ( intVal55 <= 0 )
            operator delete(voidPtr19);
        }
        *(_DWORD *)(int64Val + 452) = 12;
        *(_DWORD *)(int64Val + 448) = 5;
        *(_DWORD *)(int64Val + 696) = 10;
        intVal19 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal19;
        *(_DWORD *)(int64Val + 688) = intVal19 + 10;
        sub_D153B8(&int64Val4, "29_237_29_40_29_29_29_98_29_29_98_29_132_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr13 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr13);
        while ( __stlxr(intVal2 - 1, intPtr13) );
      }
      goto LABEL_67;
    }
    if ( id == 3 )
    {
      LevelTools::getAudioTitle((int)dword_0 + 2);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr14 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr37 = (unsigned int *)(int64Val4 - 8);
          do
            intVal48 = __ldaxr(intPtr37);
          while ( __stlxr(intVal48 - 1, intPtr37) );
        }
        else
        {
          intVal48 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal48 - 1;
        }
        if ( intVal48 <= 0 )
          operator delete(voidPtr14);
      }
      *(_DWORD *)(int64Val + 452) = 2;
      *(_DWORD *)(int64Val + 448) = 2;
      *(_DWORD *)(int64Val + 696) = 3;
      intVal15 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal15;
      *(_DWORD *)(int64Val + 688) = intVal15 + 3;
      sub_D153B8(&int64Val4, "29_98_29_40_29_29_29_29_29_29_29_29_98_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr10 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr10);
      while ( __stlxr(intVal2 - 1, intPtr10) );
      goto LABEL_67;
    }
    if ( id > 3 )
    {
      if ( id == 5 )
      {
        LevelTools::getAudioTitle((int)byte_4);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr31 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr52 = (unsigned int *)(int64Val4 - 8);
            do
              intVal62 = __ldaxr(intPtr52);
            while ( __stlxr(intVal62 - 1, intPtr52) );
          }
          else
          {
            intVal62 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal62 - 1;
          }
          if ( intVal62 <= 0 )
            operator delete(voidPtr31);
        }
        *(_DWORD *)(int64Val + 452) = 4;
        *(_DWORD *)(int64Val + 448) = 3;
        *(_DWORD *)(int64Val + 696) = 5;
        intVal37 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal37;
        *(_DWORD *)(int64Val + 688) = intVal37 + 5;
        sub_D153B8(&int64Val4, "29_73_29_40_29_29_29_29_29_29_98_29_73_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr26 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr26);
        while ( __stlxr(intVal2 - 1, intPtr26) );
      }
      else
      {
        if ( id > 5 )
        {
          LevelTools::getAudioTitle((int)byte_5);
          sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
          voidPtr16 = (void *)(int64Val4 - 24);
          if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
          {
            if ( &pthread_create )
            {
              intPtr38 = (unsigned int *)(int64Val4 - 8);
              do
                intVal49 = __ldaxr(intPtr38);
              while ( __stlxr(intVal49 - 1, intPtr38) );
            }
            else
            {
              intVal49 = *(_DWORD *)(int64Val4 - 8);
              *(_DWORD *)(int64Val4 - 8) = intVal49 - 1;
            }
            if ( intVal49 <= 0 )
              operator delete(voidPtr16);
          }
          *(_DWORD *)(int64Val + 452) = 5;
          *(_DWORD *)(int64Val + 448) = 3;
          *(_DWORD *)(int64Val + 696) = 6;
          intVal17 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
          *(_DWORD *)(int64Val + 692) = intVal17;
          *(_DWORD *)(int64Val + 688) = intVal17 + 6;
          sub_D153B8(&int64Val4, "29_73_29_40_29_29_29_29_29_29_29_29_73_29_29_29");
          sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
          int64Val2 = int64Val4;
          voidPtr2 = (void *)(int64Val4 - 24);
          if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
            goto LABEL_48;
          if ( &pthread_create )
          {
            intPtr12 = (unsigned int *)(int64Val4 - 8);
            do
              intVal2 = __ldaxr(intPtr12);
            while ( __stlxr(intVal2 - 1, intPtr12) );
            goto LABEL_67;
          }
LABEL_202:
          intVal2 = *(_DWORD *)(int64Val2 - 8);
          *(_DWORD *)(int64Val2 - 8) = intVal2 - 1;
          goto LABEL_67;
        }
        LevelTools::getAudioTitle((int)dword_0 + 3);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr50 = (unsigned int *)(int64Val4 - 8);
            do
              intVal60 = __ldaxr(intPtr50);
            while ( __stlxr(intVal60 - 1, intPtr50) );
          }
          else
          {
            intVal60 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal60 - 1;
          }
          if ( intVal60 <= 0 )
            operator delete(voidPtr);
        }
        *(_DWORD *)(int64Val + 452) = 3;
        *(_DWORD *)(int64Val + 448) = 2;
        *(_DWORD *)(int64Val + 696) = 4;
        intVal = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal;
        *(_DWORD *)(int64Val + 688) = intVal + 4;
        sub_D153B8(&int64Val4, "29_73_29_40_29_29_29_29_29_29_73_29_73_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr);
        while ( __stlxr(intVal2 - 1, intPtr) );
      }
LABEL_67:
      if ( intVal2 <= 0 )
        operator delete(voidPtr2);
      goto LABEL_48;
    }
    if ( id == 1 )
    {
      LevelTools::getAudioTitle(0);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr12 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr31 = (unsigned int *)(int64Val4 - 8);
          do
            intVal42 = __ldaxr(intPtr31);
          while ( __stlxr(intVal42 - 1, intPtr31) );
        }
        else
        {
          intVal42 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal42 - 1;
        }
        if ( intVal42 <= 0 )
          operator delete(voidPtr12);
      }
      *(_QWORD *)(int64Val + 448) = 1;
      *(_DWORD *)(int64Val + 696) = 1;
      intVal13 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal13;
      *(_DWORD *)(int64Val + 688) = intVal13 + 1;
      sub_D153B8(&int64Val4, "29_98_29_40_29_29_29_29_29_29_177_29_73_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr8 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr8);
      while ( __stlxr(intVal2 - 1, intPtr8) );
      goto LABEL_67;
    }
    if ( id == 2 )
    {
      LevelTools::getAudioTitle((int)dword_0 + 1);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr9 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr29 = (unsigned int *)(int64Val4 - 8);
          do
            intVal40 = __ldaxr(intPtr29);
          while ( __stlxr(intVal40 - 1, intPtr29) );
        }
        else
        {
          intVal40 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal40 - 1;
        }
        if ( intVal40 <= 0 )
          operator delete(voidPtr9);
      }
      *(_DWORD *)(int64Val + 452) = 1;
      *(_DWORD *)(int64Val + 448) = 1;
      *(_DWORD *)(int64Val + 696) = 2;
      intVal10 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal10;
      *(_DWORD *)(int64Val + 688) = intVal10 + 2;
      sub_D153B8(&int64Val4, "29_54_29_40_29_29_29_29_29_29_98_29_54_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr5 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr5);
      while ( __stlxr(intVal2 - 1, intPtr5) );
      goto LABEL_67;
    }
    return LevelTools::getLevel((int)dword_0 + 1, flag);
  }
  if ( id == 21 )
  {
    LevelTools::getAudioTitle((int)&dword_14);
    sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
    voidPtr30 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr48 = (unsigned int *)(int64Val4 - 8);
        do
          intVal58 = __ldaxr(intPtr48);
        while ( __stlxr(intVal58 - 1, intPtr48) );
      }
      else
      {
        intVal58 = *(_DWORD *)(int64Val4 - 8);
        *(_DWORD *)(int64Val4 - 8) = intVal58 - 1;
      }
      if ( intVal58 <= 0 )
        operator delete(voidPtr30);
    }
    *(_DWORD *)(int64Val + 452) = 20;
    *(_DWORD *)(int64Val + 448) = 5;
    *(_DWORD *)(int64Val + 696) = 12;
    intVal36 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
    *(_DWORD *)(int64Val + 692) = intVal36;
    *(_DWORD *)(int64Val + 688) = intVal36 + 12;
    sub_D153B8(
      &int64Val4,
      "40_630_195_48_0_0_210_114_0_0_572_247_97_0_0_53_466_135_0_459_276_71_448_0_101_88_101_0_0_0_0_0_0_0_0_0_0_0_0_0_0_"
      "0_0_0_0_0_0_0_0_0_0_0_0_0_0");
    sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
    int64Val2 = int64Val4;
    voidPtr2 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
      goto LABEL_48;
    if ( !&pthread_create )
      goto LABEL_202;
    intPtr25 = (unsigned int *)(int64Val4 - 8);
    do
      intVal2 = __ldaxr(intPtr25);
    while ( __stlxr(intVal2 - 1, intPtr25) );
    goto LABEL_67;
  }
  if ( id <= 21 )
  {
    if ( id == 17 )
    {
      LevelTools::getAudioTitle((int)&word_10);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr15 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr28 = (unsigned int *)(int64Val4 - 8);
          do
            intVal39 = __ldaxr(intPtr28);
          while ( __stlxr(intVal39 - 1, intPtr28) );
        }
        else
        {
          intVal39 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal39 - 1;
        }
        if ( intVal39 <= 0 )
          operator delete(voidPtr15);
      }
      *(_DWORD *)(int64Val + 452) = 16;
      *(_DWORD *)(int64Val + 448) = 4;
      *(_DWORD *)(int64Val + 696) = 10;
      intVal16 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal16;
      *(_DWORD *)(int64Val + 688) = intVal16 + 10;
      sub_D153B8(&int64Val4, "29_237_29_73_29_29_98_132_29_29_566_54_132_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr11 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr11);
      while ( __stlxr(intVal2 - 1, intPtr11) );
    }
    else if ( id <= 17 )
    {
      if ( id == 15 )
      {
        LevelTools::getAudioTitle((int)&byte_9[5]);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr20 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr32 = (unsigned int *)(int64Val4 - 8);
            do
              intVal43 = __ldaxr(intPtr32);
            while ( __stlxr(intVal43 - 1, intPtr32) );
          }
          else
          {
            intVal43 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal43 - 1;
          }
          if ( intVal43 <= 0 )
            operator delete(voidPtr20);
        }
        *(_DWORD *)(int64Val + 452) = 14;
        *(_DWORD *)(int64Val + 448) = 5;
        *(_DWORD *)(int64Val + 696) = 12;
        intVal20 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal20;
        *(_DWORD *)(int64Val + 688) = intVal20 + 12;
        sub_D153B8(&int64Val4, "29_237_98_40_29_29_29_237_29_29_98_73_177_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr14 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr14);
        while ( __stlxr(intVal2 - 1, intPtr14) );
      }
      else
      {
        LevelTools::getAudioTitle((int)&byte_9[6]);
        sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
        voidPtr11 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
        {
          if ( &pthread_create )
          {
            intPtr36 = (unsigned int *)(int64Val4 - 8);
            do
              intVal47 = __ldaxr(intPtr36);
            while ( __stlxr(intVal47 - 1, intPtr36) );
          }
          else
          {
            intVal47 = *(_DWORD *)(int64Val4 - 8);
            *(_DWORD *)(int64Val4 - 8) = intVal47 - 1;
          }
          if ( intVal47 <= 0 )
            operator delete(voidPtr11);
        }
        *(_DWORD *)(int64Val + 452) = 15;
        *(_DWORD *)(int64Val + 448) = 5;
        *(_DWORD *)(int64Val + 696) = 12;
        intVal12 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
        *(_DWORD *)(int64Val + 692) = intVal12;
        *(_DWORD *)(int64Val + 688) = intVal12 + 12;
        sub_D153B8(&int64Val4, "29_317_73_73_29_29_54_132_29_29_237_132_177_29_29_29");
        sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
        int64Val2 = int64Val4;
        voidPtr2 = (void *)(int64Val4 - 24);
        if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
          goto LABEL_48;
        if ( !&pthread_create )
          goto LABEL_202;
        intPtr7 = (unsigned int *)(int64Val4 - 8);
        do
          intVal2 = __ldaxr(intPtr7);
        while ( __stlxr(intVal2 - 1, intPtr7) );
      }
    }
    else if ( id == 19 )
    {
      LevelTools::getAudioTitle((int)&word_12);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr29 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr51 = (unsigned int *)(int64Val4 - 8);
          do
            intVal61 = __ldaxr(intPtr51);
          while ( __stlxr(intVal61 - 1, intPtr51) );
        }
        else
        {
          intVal61 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal61 - 1;
        }
        if ( intVal61 <= 0 )
          operator delete(voidPtr29);
      }
      *(_DWORD *)(int64Val + 452) = 18;
      *(_DWORD *)(int64Val + 448) = 4;
      *(_DWORD *)(int64Val + 696) = 10;
      intVal35 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal35;
      *(_DWORD *)(int64Val + 688) = intVal35 + 10;
      sub_D153B8(&int64Val4, "29_424_132_40_29_29_566_132_29_29_756_237_132_29_73_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr24 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr24);
      while ( __stlxr(intVal2 - 1, intPtr24) );
    }
    else if ( id <= 19 )
    {
      LevelTools::getAudioTitle((int)&word_10 + 1);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr3 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr34 = (unsigned int *)(int64Val4 - 8);
          do
            intVal45 = __ldaxr(intPtr34);
          while ( __stlxr(intVal45 - 1, intPtr34) );
        }
        else
        {
          intVal45 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal45 - 1;
        }
        if ( intVal45 <= 0 )
          operator delete(voidPtr3);
      }
      *(_DWORD *)(int64Val + 452) = 17;
      *(_DWORD *)(int64Val + 448) = 6;
      *(_DWORD *)(int64Val + 696) = 14;
      intVal3 = rand();
      *(_DWORD *)(int64Val + 836) = 20;
      *(_DWORD *)(int64Val + 680) = 1;
      intVal4 = (int)(float)((float)((float)intVal3 * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal4;
      *(_DWORD *)(int64Val + 688) = intVal4 + 14;
      intVal5 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 676) = intVal5;
      *(_DWORD *)(int64Val + 672) = intVal5 + 1;
      sub_D153B8(&int64Val4, "29_317_40_54_29_40_132_132_29_29_424_98_237_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr2 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr2);
      while ( __stlxr(intVal2 - 1, intPtr2) );
    }
    else
    {
      LevelTools::getAudioTitle((int)&word_12 + 1);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr24 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr44 = (unsigned int *)(int64Val4 - 8);
          do
            intVal54 = __ldaxr(intPtr44);
          while ( __stlxr(intVal54 - 1, intPtr44) );
        }
        else
        {
          intVal54 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal54 - 1;
        }
        if ( intVal54 <= 0 )
          operator delete(voidPtr24);
      }
      *(_DWORD *)(int64Val + 452) = 19;
      *(_DWORD *)(int64Val + 448) = 6;
      *(_DWORD *)(int64Val + 696) = 15;
      intVal25 = rand();
      *(_DWORD *)(int64Val + 836) = 30;
      *(_DWORD *)(int64Val + 680) = 1;
      intVal26 = (int)(float)((float)((float)intVal25 * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal26;
      *(_DWORD *)(int64Val + 688) = intVal26 + 15;
      intVal27 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 676) = intVal27;
      *(_DWORD *)(int64Val + 672) = intVal27 + 1;
      sub_D153B8(&int64Val4, "29_317_73_73_29_29_317_424_73_29_566_317_177_29_132_54");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr18 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr18);
      while ( __stlxr(intVal2 - 1, intPtr18) );
    }
    goto LABEL_67;
  }
  if ( id == 5001 )
  {
    sub_D153B8(&int64Val4, "The Tower");
    sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
    voidPtr23 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr47 = (unsigned int *)(int64Val4 - 8);
        do
          intVal57 = __ldaxr(intPtr47);
        while ( __stlxr(intVal57 - 1, intPtr47) );
      }
      else
      {
        intVal57 = *(_DWORD *)(int64Val4 - 8);
        *(_DWORD *)(int64Val4 - 8) = intVal57 - 1;
      }
      if ( intVal57 <= 0 )
        operator delete(voidPtr23);
    }
    *(_DWORD *)(int64Val + 452) = 0;
    *(_DWORD *)(int64Val + 456) = 10003039;
    *(_DWORD *)(int64Val + 448) = 2;
    *(_DWORD *)(int64Val + 696) = 5;
    intVal24 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
    *(_DWORD *)(int64Val + 692) = intVal24;
    *(_DWORD *)(int64Val + 688) = intVal24 + 5;
    sub_D153B8(&int64Val4, "1,210,25,209,56,310,58,250,62,249");
    sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
    int64Val3 = int64Val4;
    voidPtr6 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
      goto LABEL_47;
    if ( !&pthread_create )
      goto LABEL_204;
    intPtr17 = (unsigned int *)(int64Val4 - 8);
    do
      intVal22 = __ldaxr(intPtr17);
    while ( __stlxr(intVal22 - 1, intPtr17) );
LABEL_134:
    if ( intVal22 <= 0 )
      operator delete(voidPtr6);
    goto LABEL_47;
  }
  if ( id <= 5001 )
  {
    if ( id == 22 )
    {
      LevelTools::getAudioTitle((int)&dword_14 + 1);
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr17 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr55 = (unsigned int *)(int64Val4 - 8);
          do
            intVal65 = __ldaxr(intPtr55);
          while ( __stlxr(intVal65 - 1, intPtr55) );
        }
        else
        {
          intVal65 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal65 - 1;
        }
        if ( intVal65 <= 0 )
          operator delete(voidPtr17);
      }
      *(_DWORD *)(int64Val + 452) = 21;
      *(_DWORD *)(int64Val + 448) = 5;
      *(_DWORD *)(int64Val + 696) = 12;
      intVal18 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal18;
      *(_DWORD *)(int64Val + 688) = intVal18 + 12;
      sub_D153B8(&int64Val4, "24,340,52,244,54,1335,56,356,58,510,60,1257");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      voidPtr18 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr43 = (unsigned int *)(int64Val4 - 8);
          do
            intVal53 = __ldaxr(intPtr43);
          while ( __stlxr(intVal53 - 1, intPtr43) );
        }
        else
        {
          intVal53 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal53 - 1;
        }
        if ( intVal53 <= 0 )
          operator delete(voidPtr18);
      }
      *(_DWORD *)(int64Val + 912) = 21940;
      goto LABEL_48;
    }
    if ( id == 3001 )
    {
      sub_D153B8(&int64Val4, "The Challenge");
      sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
      voidPtr10 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
      {
        if ( &pthread_create )
        {
          intPtr41 = (unsigned int *)(int64Val4 - 8);
          do
            intVal51 = __ldaxr(intPtr41);
          while ( __stlxr(intVal51 - 1, intPtr41) );
        }
        else
        {
          intVal51 = *(_DWORD *)(int64Val4 - 8);
          *(_DWORD *)(int64Val4 - 8) = intVal51 - 1;
        }
        if ( intVal51 <= 0 )
          operator delete(voidPtr10);
      }
      *(_DWORD *)(int64Val + 448) = 3;
      *(_DWORD *)(int64Val + 452) = 26;
      *(_DWORD *)(int64Val + 696) = 3;
      intVal11 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
      *(_DWORD *)(int64Val + 692) = intVal11;
      *(_DWORD *)(int64Val + 688) = intVal11 + 3;
      sub_D153B8(&int64Val4, "73_237_29_40_29_29_237_98_29_29_237_237_132_29_29_29");
      sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
      int64Val2 = int64Val4;
      voidPtr2 = (void *)(int64Val4 - 24);
      if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
        goto LABEL_48;
      if ( !&pthread_create )
        goto LABEL_202;
      intPtr6 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr6);
      while ( __stlxr(intVal2 - 1, intPtr6) );
      goto LABEL_67;
    }
    return LevelTools::getLevel((int)dword_0 + 1, flag);
  }
  if ( id == 5003 )
  {
    sub_D153B8(&int64Val4, "The Cellar");
    sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
    voidPtr26 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr27 = (unsigned int *)(int64Val4 - 8);
        do
          intVal38 = __ldaxr(intPtr27);
        while ( __stlxr(intVal38 - 1, intPtr27) );
      }
      else
      {
        intVal38 = *(_DWORD *)(int64Val4 - 8);
        *(_DWORD *)(int64Val4 - 8) = intVal38 - 1;
      }
      if ( intVal38 <= 0 )
        operator delete(voidPtr26);
    }
    *(_DWORD *)(int64Val + 452) = 0;
    *(_DWORD *)(int64Val + 456) = 10002875;
    *(_DWORD *)(int64Val + 448) = 4;
    *(_DWORD *)(int64Val + 696) = 7;
    intVal30 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
    *(_DWORD *)(int64Val + 692) = intVal30;
    *(_DWORD *)(int64Val + 688) = intVal30 + 7;
    sub_D153B8(&int64Val4, "50,206,52,396,54,682");
    sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
    int64Val3 = int64Val4;
    voidPtr6 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
      goto LABEL_47;
    if ( !&pthread_create )
      goto LABEL_204;
    intPtr21 = (unsigned int *)(int64Val4 - 8);
    do
      intVal22 = __ldaxr(intPtr21);
    while ( __stlxr(intVal22 - 1, intPtr21) );
    goto LABEL_134;
  }
  if ( id < 5003 )
  {
    sub_D153B8(&int64Val4, "The Sewers");
    sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
    voidPtr21 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr33 = (unsigned int *)(int64Val4 - 8);
        do
          intVal44 = __ldaxr(intPtr33);
        while ( __stlxr(intVal44 - 1, intPtr33) );
      }
      else
      {
        intVal44 = *(_DWORD *)(int64Val4 - 8);
        *(_DWORD *)(int64Val4 - 8) = intVal44 - 1;
      }
      if ( intVal44 <= 0 )
        operator delete(voidPtr21);
    }
    *(_DWORD *)(int64Val + 452) = 0;
    *(_DWORD *)(int64Val + 456) = 10003129;
    *(_DWORD *)(int64Val + 448) = 3;
    *(_DWORD *)(int64Val + 696) = 6;
    intVal21 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
    *(_DWORD *)(int64Val + 692) = intVal21;
    *(_DWORD *)(int64Val + 688) = intVal21 + 6;
    sub_D153B8(&int64Val4, "50,366,52,332,54,408");
    sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
    int64Val3 = int64Val4;
    voidPtr6 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) == &qword_124B6B0 )
      goto LABEL_47;
    if ( !&pthread_create )
      goto LABEL_204;
    intPtr15 = (unsigned int *)(int64Val4 - 8);
    do
      intVal22 = __ldaxr(intPtr15);
    while ( __stlxr(intVal22 - 1, intPtr15) );
    goto LABEL_134;
  }
  if ( id != 5004 )
    return LevelTools::getLevel((int)dword_0 + 1, flag);
  sub_D153B8(&int64Val4, "The Secret Hollow");
  sub_D18EA8((__int64 *)(int64Val + 344), &int64Val4);
  voidPtr5 = (void *)(int64Val4 - 24);
  if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr53 = (unsigned int *)(int64Val4 - 8);
      do
        intVal63 = __ldaxr(intPtr53);
      while ( __stlxr(intVal63 - 1, intPtr53) );
    }
    else
    {
      intVal63 = *(_DWORD *)(int64Val4 - 8);
      *(_DWORD *)(int64Val4 - 8) = intVal63 - 1;
    }
    if ( intVal63 <= 0 )
      operator delete(voidPtr5);
  }
  *(_DWORD *)(int64Val + 452) = 0;
  *(_DWORD *)(int64Val + 456) = 10006086;
  *(_DWORD *)(int64Val + 448) = 4;
  *(_DWORD *)(int64Val + 696) = 7;
  intVal7 = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0);
  *(_DWORD *)(int64Val + 692) = intVal7;
  *(_DWORD *)(int64Val + 688) = intVal7 + 7;
  sub_D153B8(&int64Val4, "52,505,54,1104,56,556,58,368");
  sub_D18EA8((__int64 *)(int64Val + 888), &int64Val4);
  int64Val3 = int64Val4;
  voidPtr6 = (void *)(int64Val4 - 24);
  if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr40 = (unsigned int *)(int64Val4 - 8);
      do
        intVal22 = __ldaxr(intPtr40);
      while ( __stlxr(intVal22 - 1, intPtr40) );
      goto LABEL_134;
    }
LABEL_204:
    intVal22 = *(_DWORD *)(int64Val3 - 8);
    *(_DWORD *)(int64Val3 - 8) = intVal22 - 1;
    goto LABEL_134;
  }
LABEL_47:
  *(_DWORD *)(int64Val + 640) = (unsigned int)byte_5;
LABEL_48:
  if ( !flag )
  {
    localLevelManager = LocalLevelManager::sharedState();
    LocalLevelManager::getMainLevelString(localLevelManager, id);
    sub_D18EA8((__int64 *)(int64Val + 360), &int64Val4);
    voidPtr7 = (void *)(int64Val4 - 24);
    if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
    {
      if ( &pthread_create )
      {
        intPtr19 = (unsigned int *)(int64Val4 - 8);
        do
          intVal28 = __ldaxr(intPtr19);
        while ( __stlxr(intVal28 - 1, intPtr19) );
      }
      else
      {
        intVal28 = *(_DWORD *)(int64Val4 - 8);
        *(_DWORD *)(int64Val4 - 8) = intVal28 - 1;
      }
      if ( intVal28 <= 0 )
        operator delete(voidPtr7);
    }
  }
  *(_DWORD *)(int64Val + 336) = id;
  intVal8 = rand();
  *(_DWORD *)(int64Val + 868) = 1;
  value = (float)((float)intVal8 * 4.6566e-10) * 1000.0;
  *(_DWORD *)(int64Val + 332) = (int)value;
  *(_DWORD *)(int64Val + 328) = id + (int)value;
  returnVal = (GJGameLevel *)int64Val;
  if ( id != 3001 )
    *(_DWORD *)(int64Val + 704) = 3;
  return returnVal;
}


std::unordered_set<int> * LevelTools::getLevelList(std::unordered_set<int> *__return_ptr retstr)
{
  std::_Hashtable<int, int, std::allocator<int>, std::__detail::_Identity, std::equal_to<int>, std::hash<int>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::_Hashtable_traits<false, true, true>>::size_type M_bucket_count;
  std::_Hashtable<int, int, std::allocator<int>, std::__detail::_Identity, std::equal_to<int>, std::hash<int>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::_Hashtable_traits<false, true, true>>::__bucket_type *p_M_single_bucket;
  _DWORD *dwordPtr;
  int intVal;
  unsigned __int64 int64Val;
  unsigned __int64 int64Val2;
  std::_Hashtable<int, int, std::allocator<int>, std::__detail::_Identity, std::equal_to<int>, std::hash<int>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::_Hashtable_traits<false, true, true>>::__bucket_type __bucket_type;
  std::__detail::_Hash_node_base *M_nxt;
  int intVal2;
  __int64 int64Val3;
  int intVal3;
  __int64 int64Val4;

  retstr->_M_h._M_before_begin._M_nxt = 0;
  retstr->_M_h._M_element_count = 0;
  retstr->_M_h._M_rehash_policy._M_next_resize = 0;
  retstr->_M_h._M_rehash_policy._M_max_load_factor = 1.0;
  M_bucket_count = sub_D146DC((__int64)&retstr->_M_h._M_rehash_policy, 0x2Bu);
  retstr->_M_h._M_bucket_count = M_bucket_count;
  if ( M_bucket_count == 1 )
  {
    retstr->_M_h._M_single_bucket = 0;
    p_M_single_bucket = &retstr->_M_h._M_single_bucket;
  }
  else
  {
    if ( M_bucket_count > 0x1FFFFFFFFFFFFFFFLL )
      sub_D1383C();
    int64Val4 = 8 * M_bucket_count;
    p_M_single_bucket = (std::_Hashtable<int, int, std::allocator<int>, std::__detail::_Identity, std::equal_to<int>, std::hash<int>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::_Hashtable_traits<false, true, true>>::__bucket_type *)operator new(8 * M_bucket_count);
    memset(p_M_single_bucket, 0, int64Val4);
    M_bucket_count = retstr->_M_h._M_bucket_count;
  }
  retstr->_M_h._M_buckets = p_M_single_bucket;
  dwordPtr = &unk_EA5114;
  intVal = 1;
  int64Val = 1;
  int64Val2 = 1 % M_bucket_count;
  __bucket_type = p_M_single_bucket[1 % M_bucket_count];
  if ( !__bucket_type )
    goto LABEL_12;
LABEL_5:
  M_nxt = __bucket_type->_M_nxt;
  if ( LODWORD(__bucket_type->_M_nxt[1]._M_nxt) == intVal )
  {
LABEL_9:
    if ( !__bucket_type->_M_nxt )
      goto LABEL_12;
    if ( dwordPtr != (_DWORD *)&unk_EA51BC )
      goto LABEL_11;
  }
  else
  {
    while ( M_nxt->_M_nxt )
    {
      intVal2 = (int)M_nxt->_M_nxt[1]._M_nxt;
      __bucket_type = M_nxt;
      if ( int64Val2 != intVal2 % M_bucket_count )
        break;
      M_nxt = M_nxt->_M_nxt;
      if ( intVal2 == intVal )
        goto LABEL_9;
    }
LABEL_12:
    while ( 1 )
    {
      int64Val3 = operator new(0x10u);
      intVal3 = *(dwordPtr - 1);
      *(_QWORD *)int64Val3 = 0;
      *(_DWORD *)(int64Val3 + 8) = intVal3;
      std::_Hashtable<int,int,std::allocator<int>,std::__detail::_Identity,std::equal_to<int>,std::hash<int>,std::__detail::_Mod_range_hashing,std::__detail::_Default_ranged_hash,std::__detail::_Prime_rehash_policy,std::__detail::_Hashtable_traits<false,true,true>>::_M_insert_unique_node(
        (__int64)retstr,
        int64Val2,
        int64Val,
        (_QWORD *)int64Val3);
      if ( dwordPtr == (_DWORD *)&unk_EA51BC )
        break;
LABEL_11:
      intVal = *dwordPtr++;
      M_bucket_count = retstr->_M_h._M_bucket_count;
      int64Val = intVal;
      int64Val2 = intVal % M_bucket_count;
      __bucket_type = retstr->_M_h._M_buckets[int64Val2];
      if ( __bucket_type )
        goto LABEL_5;
    }
  }
  return retstr;
}


SongInfoObject * LevelTools::getSongObject(int id)
{
  std::string str;
  std::string str2;
  std::string str3;
  std::string str4;
  std::string str5;
  SongInfoObject *returnVal;
  void *voidPtr;
  void *voidPtr2;
  void *voidPtr3;
  void *voidPtr4;
  void *voidPtr5;
  unsigned int *intPtr;
  unsigned int *intPtr2;
  unsigned int *intPtr3;
  unsigned int *intPtr4;
  unsigned int *intPtr5;
  int intVal;
  unsigned int *intPtr6;
  int intVal2;
  unsigned int *intPtr7;
  int intVal3;
  unsigned int *intPtr8;
  int intVal4;
  unsigned int *intPtr9;
  int intVal5;
  unsigned int *intPtr10;
  int intVal6;
  unsigned int *intPtr11;
  int intVal7;
  unsigned int *intPtr12;
  int intVal8;
  unsigned int *intPtr13;
  int intVal9;
  std::string str6;
  std::string str7;
  unsigned int intVal10;
  unsigned int *intPtr14;
  unsigned int *intPtr15;
  unsigned int *intPtr16;
  unsigned int *intPtr17;
  __int64 int64Val;
  __int64 int64Val2;
  __int64 int64Val3;
  __int64 int64Val4;
  __int64 int64Val5;

  LevelTools::getAudioTitle(id);
  intVal10 = LevelTools::artistForAudio(id);
  LevelTools::nameForArtist(intVal10);
  sub_D18D28(&intPtr16, &intPtr14);
  sub_D18D28(&intPtr17, &intPtr15);
  sub_D153B8(&int64Val, &byte_F288A0);
  sub_D153B8(&int64Val2, &byte_F288A0);
  sub_D153B8(&int64Val3, &byte_F288A0);
  sub_D153B8(&int64Val4, &byte_F288A0);
  sub_D153B8(&int64Val5, &byte_F288A0);
  str.m_data.m_data = (stl::StringData<char>::Internal *)intVal10;
  LODWORD(str6.m_data.m_data) = 0;
  str2.m_data.m_data = (stl::StringData<char>::Internal *)&intPtr17;
  LOBYTE(str7.m_data.m_data) = 0;
  str3.m_data.m_data = (stl::StringData<char>::Internal *)&int64Val2;
  str4.m_data.m_data = (stl::StringData<char>::Internal *)&int64Val3;
  str5.m_data.m_data = (stl::StringData<char>::Internal *)&int64Val4;
  returnVal = (SongInfoObject *)((SongInfoObject *(__fastcall *)(SongInfoObject *__hidden, int, std::string, std::string, int, float, std::string, std::string, std::string, std::string, int, std::string, bool, int))SongInfoObject::create)(
                           (SongInfoObject *)(unsigned int)id,
                           (int)&intPtr16,
                           str2,
                           str,
                           (int)&int64Val,
                           0.0,
                           str3,
                           str4,
                           str5,
                           str6,
                           (int)&int64Val5,
                           str7,
                           0,
                           -1);
  voidPtr = (void *)(int64Val5 - 24);
  if ( (__int64 *)(int64Val5 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr5 = (unsigned int *)(int64Val5 - 8);
      do
        intVal = __ldaxr(intPtr5);
      while ( __stlxr(intVal - 1, intPtr5) );
    }
    else
    {
      intVal = *(_DWORD *)(int64Val5 - 8);
      *(_DWORD *)(int64Val5 - 8) = intVal - 1;
    }
    if ( intVal <= 0 )
      operator delete(voidPtr);
  }
  voidPtr2 = (void *)(int64Val4 - 24);
  if ( (__int64 *)(int64Val4 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr6 = (unsigned int *)(int64Val4 - 8);
      do
        intVal2 = __ldaxr(intPtr6);
      while ( __stlxr(intVal2 - 1, intPtr6) );
    }
    else
    {
      intVal2 = *(_DWORD *)(int64Val4 - 8);
      *(_DWORD *)(int64Val4 - 8) = intVal2 - 1;
    }
    if ( intVal2 <= 0 )
      operator delete(voidPtr2);
  }
  voidPtr3 = (void *)(int64Val3 - 24);
  if ( (__int64 *)(int64Val3 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr7 = (unsigned int *)(int64Val3 - 8);
      do
        intVal3 = __ldaxr(intPtr7);
      while ( __stlxr(intVal3 - 1, intPtr7) );
    }
    else
    {
      intVal3 = *(_DWORD *)(int64Val3 - 8);
      *(_DWORD *)(int64Val3 - 8) = intVal3 - 1;
    }
    if ( intVal3 <= 0 )
      operator delete(voidPtr3);
  }
  voidPtr4 = (void *)(int64Val2 - 24);
  if ( (__int64 *)(int64Val2 - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr8 = (unsigned int *)(int64Val2 - 8);
      do
        intVal4 = __ldaxr(intPtr8);
      while ( __stlxr(intVal4 - 1, intPtr8) );
    }
    else
    {
      intVal4 = *(_DWORD *)(int64Val2 - 8);
      *(_DWORD *)(int64Val2 - 8) = intVal4 - 1;
    }
    if ( intVal4 <= 0 )
      operator delete(voidPtr4);
  }
  voidPtr5 = (void *)(int64Val - 24);
  if ( (__int64 *)(int64Val - 24) != &qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr9 = (unsigned int *)(int64Val - 8);
      do
        intVal5 = __ldaxr(intPtr9);
      while ( __stlxr(intVal5 - 1, intPtr9) );
    }
    else
    {
      intVal5 = *(_DWORD *)(int64Val - 8);
      *(_DWORD *)(int64Val - 8) = intVal5 - 1;
    }
    if ( intVal5 <= 0 )
      operator delete(voidPtr5);
  }
  intPtr = intPtr17 - 6;
  if ( intPtr17 - 6 != (unsigned int *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr10 = intPtr17 - 2;
      do
        intVal6 = __ldaxr(intPtr10);
      while ( __stlxr(intVal6 - 1, intPtr10) );
    }
    else
    {
      intVal6 = *(intPtr17 - 2);
      *(intPtr17 - 2) = intVal6 - 1;
    }
    if ( intVal6 <= 0 )
      operator delete(intPtr);
  }
  intPtr2 = intPtr16 - 6;
  if ( intPtr16 - 6 != (unsigned int *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr11 = intPtr16 - 2;
      do
        intVal7 = __ldaxr(intPtr11);
      while ( __stlxr(intVal7 - 1, intPtr11) );
    }
    else
    {
      intVal7 = *(intPtr16 - 2);
      *(intPtr16 - 2) = intVal7 - 1;
    }
    if ( intVal7 <= 0 )
      operator delete(intPtr2);
  }
  intPtr3 = intPtr15 - 6;
  if ( intPtr15 - 6 != (unsigned int *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr12 = intPtr15 - 2;
      do
        intVal8 = __ldaxr(intPtr12);
      while ( __stlxr(intVal8 - 1, intPtr12) );
    }
    else
    {
      intVal8 = *(intPtr15 - 2);
      *(intPtr15 - 2) = intVal8 - 1;
    }
    if ( intVal8 <= 0 )
      operator delete(intPtr3);
  }
  intPtr4 = intPtr14 - 6;
  if ( intPtr14 - 6 != (unsigned int *)&qword_124B6B0 )
  {
    if ( &pthread_create )
    {
      intPtr13 = intPtr14 - 2;
      do
        intVal9 = __ldaxr(intPtr13);
      while ( __stlxr(intVal9 - 1, intPtr13) );
    }
    else
    {
      intVal9 = *(intPtr14 - 2);
      *(intPtr14 - 2) = intVal9 - 1;
    }
    if ( intVal9 <= 0 )
      operator delete(intPtr4);
  }
  return returnVal;
}


void LevelTools::moveTriggerObjectsToArray(cocos2d::CCArray *objects, cocos2d::CCDictionary *objectsDict, int index)
{
  cocos2d::CCArray *array;
  cocos2d::CCArray *array2;
  cocos2d::CCObject *obj;
  cocos2d::CCObject *obj2;
  cocos2d::CCObject *obj3;
  bool flag;

  array = (cocos2d::CCArray *)cocos2d::CCDictionary::objectForKey(objectsDict, (const std::string *)index);
  array2 = array;
  if ( array )
  {
LABEL_2:
    if ( (unsigned int)cocos2d::CCArray::count(array) )
    {
      while ( (unsigned int)cocos2d::CCArray::count(array2) )
      {
        obj = cocos2d::CCArray::objectAtIndex(array2, 0);
        obj2 = cocos2d::CCArray::objectAtIndex(array2, 0);
        cocos2d::CCArray::addObject(objects, (cocos2d::_ccArray *)obj2, obj3);
        cocos2d::CCArray::removeObjectAtIndex(array2, 0, 1u, flag);
        if ( HIDWORD(obj[18].__vftable) == 2900 && LOBYTE(obj[32].m_uOrderOfArrival) )
        {
          array = (cocos2d::CCArray *)cocos2d::CCDictionary::objectForKey(
                                     objectsDict,
                                     (const std::string *)obj[32].m_unknown5);
          array2 = array;
          if ( array )
            goto LABEL_2;
          return;
        }
      }
    }
  }
}


std::string LevelTools::nameForArtist(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;
  stl::StringData<char>::Internal *internal14;

  switch ( id )
  {
    case 0:
      internal5 = internal;
      sub_D153B8(internal, "DJVI");
      returnVal.m_data.m_data = internal5;
      break;
    case 1:
      internal6 = internal;
      sub_D153B8(internal, "Waterflame");
      returnVal.m_data.m_data = internal6;
      break;
    case 2:
      internal7 = internal;
      sub_D153B8(internal, "OcularNebula");
      returnVal.m_data.m_data = internal7;
      break;
    case 3:
      internal8 = internal;
      sub_D153B8(internal, "ForeverBound");
      returnVal.m_data.m_data = internal8;
      break;
    case 4:
      internal9 = internal;
      sub_D153B8(internal, "Step");
      returnVal.m_data.m_data = internal9;
      break;
    case 5:
      internal10 = internal;
      sub_D153B8(internal, "DJ-Nate");
      returnVal.m_data.m_data = internal10;
      break;
    case 6:
      internal11 = internal;
      sub_D153B8(internal, "F-777");
      returnVal.m_data.m_data = internal11;
      break;
    case 7:
      internal12 = internal;
      sub_D153B8(internal, "MDK");
      returnVal.m_data.m_data = internal12;
      break;
    case 8:
      internal13 = internal;
      sub_D153B8(internal, "Dex Arson");
      returnVal.m_data.m_data = internal13;
      break;
    case 9:
      internal14 = internal;
      sub_D153B8(internal, "Boom Kitty");
      returnVal.m_data.m_data = internal14;
      break;
    case 10:
      internal3 = internal;
      sub_D153B8(internal, "Bossfight");
      returnVal.m_data.m_data = internal3;
      break;
    case 11:
      internal4 = internal;
      sub_D153B8(internal, "Hinkik");
      returnVal.m_data.m_data = internal4;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, " ");
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


std::string LevelTools::ngURLForArtist(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;

  switch ( id )
  {
    case 0:
      internal5 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal5;
      break;
    case 1:
      internal6 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal6;
      break;
    case 2:
      internal7 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal7;
      break;
    case 3:
      internal8 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal8;
      break;
    case 4:
      internal9 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal9;
      break;
    case 5:
      internal10 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal10;
      break;
    case 6:
      internal11 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal11;
      break;
    case 8:
      internal12 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal12;
      break;
    case 9:
      internal13 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal13;
      break;
    case 10:
      internal3 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal3;
      break;
    case 11:
      internal4 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal4;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, &byte_F288A0);
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


float LevelTools::offsetBPMForTrack(int id)
{
  unsigned int intVal;
  float returnVal;

  intVal = id - 10;
  returnVal = 0.0;
  if ( intVal <= 3 )
    return *(float *)&byte_EA5080[4 * intVal + 112];
  return returnVal;
}


void LevelTools::sortSpeedObjects(cocos2d::CCArray *objects, GJBaseGameLayer *layer)
{
  cocos2d::ccArray *data;
  cocos2d::ccArray *ccArray;
  cocos2d::CCObject **obj;
  cocos2d::CCObject **obj2;
  cocos2d::CCObject *obj3;
  unsigned int intVal;
  unsigned int intVal2;
  int intVal3;
  const cocos2d::CCRect *rect;
  int m_eObjType_high;
  cocos2d::CCObject **arr;
  cocos2d::CCObject **obj4;
  cocos2d::CCObject *i;
  cocos2d::CCArray *Group;
  cocos2d::CCArray *array;
  cocos2d::CCObject *obj5;
  cocos2d::CCPoint point;
  cocos2d::CCPoint point2;
  cocos2d::CCRect rect2;

  if ( (unsigned int)cocos2d::CCArray::count(objects) )
  {
    if ( objects )
    {
      data = objects->data;
      if ( data->num )
      {
        arr = data->arr;
        obj4 = &arr[data->num - 1];
        if ( arr <= obj4 )
        {
          for ( i = *arr; *arr; i = *arr )
          {
            ((void (__fastcall *)(cocos2d::CCObject *))i->__vftable[22].update)(i);
            cocos2d::CCPoint::operator=(&i[31].m_uIndexInArray, &rect2);
            if ( SHIWORD(i[31].m_eObjType) == -5 )
            {
              Group = GJBaseGameLayer::getGroup(layer, i[25].m_uOrderOfArrival);
              array = Group;
              if ( Group
                && (unsigned int)cocos2d::CCArray::count(Group)
                && (obj5 = cocos2d::CCArray::randomObject(array)) != 0 )
              {
                ((void (__fastcall *)(cocos2d::CCObject *))obj5->__vftable[22].update)(obj5);
              }
              else
              {
                ((void (__fastcall *)(cocos2d::CCObject *))i->__vftable[22].update)(i);
              }
              cocos2d::CCPoint::operator=(&i[33].m_eObjType, &rect2);
            }
            if ( obj4 < ++arr )
              break;
          }
        }
      }
      LevelTools::sortChannelOrderObjects(objects, 0, (bool)dword_0 + 1);
      ccArray = objects->data;
      if ( ccArray->num )
      {
        obj = ccArray->arr;
        obj2 = &obj[ccArray->num - 1];
        if ( obj <= obj2 )
        {
          obj3 = *obj;
          if ( *obj )
          {
            intVal = 0;
            intVal2 = 0;
            while ( 1 )
            {
              intVal3 = HIDWORD(obj3[18].__vftable);
              if ( intVal3 == 1330 )
                goto LABEL_20;
              if ( intVal3 <= 1330 )
              {
                if ( intVal3 == 141 )
                  goto LABEL_20;
                if ( intVal3 <= 141 )
                {
                  if ( intVal3 == 36 || intVal3 == 84 )
                    goto LABEL_20;
LABEL_15:
                  if ( LOBYTE(obj3[26].__vftable) )
                    goto LABEL_16;
                  goto LABEL_20;
                }
                if ( intVal3 < 200 )
                  goto LABEL_15;
                if ( intVal3 <= 203 )
                  goto LABEL_16;
                if ( intVal3 != 1022 )
                  goto LABEL_15;
              }
              else
              {
                if ( intVal3 == 1704 )
                  goto LABEL_20;
                if ( intVal3 <= 1704 )
                {
                  if ( intVal3 == 1334 )
                    goto LABEL_16;
                  if ( intVal3 == 1594 || intVal3 == 1333 )
                    goto LABEL_20;
                  goto LABEL_15;
                }
                if ( intVal3 == 2902 )
                {
LABEL_16:
                  if ( !BYTE2(obj3[26].__vftable) )
                  {
                    rect = (const cocos2d::CCRect *)((__int64 (__fastcall *)(cocos2d::CCObject *))obj3->__vftable[16].acceptVisitor)(obj3);
                    cocos2d::CCRect::CCRect(&rect2, rect);
                    cocos2d::CCPoint::CCPoint(
                      &point,
                      rect2.origin.x + (float)(rect2.size.width * 0.5),
                      rect2.origin.y + (float)(rect2.size.height * 0.5));
                    if ( intVal >= (intVal2 ^ 1) )
                    {
                      if ( intVal2 <= intVal )
                      {
                        if ( intVal2 >= intVal )
                        {
                          if ( intVal && intVal2 )
                            point.y = (float)(point.y - (float)(rect2.size.height * 0.5)) - 15.0;
                        }
                        else
                        {
                          point.y = (float)(point.y + (float)(rect2.size.height * 0.5)) + 15.0;
                        }
                      }
                      else
                      {
                        point.x = (float)(point.x + (float)(rect2.size.width * 0.5)) + 15.0;
                      }
                    }
                    else
                    {
                      point.x = (float)(point.x - (float)(rect2.size.width * 0.5)) - 15.0;
                    }
                    cocos2d::CCPoint::CCPoint(&point2, &point);
                    cocos2d::CCPoint::operator=(&obj3[31].m_uIndexInArray, &point2);
                  }
                  goto LABEL_20;
                }
                if ( intVal3 != 3027 && intVal3 != 1751 )
                  goto LABEL_15;
              }
LABEL_20:
              m_eObjType_high = SHIWORD(obj3[31].m_eObjType);
              if ( m_eObjType_high == -4 )
              {
                if ( !BYTE1(obj3[32].m_uOrderOfArrival) )
                {
                  intVal = (unsigned int)(obj3[32].m_eObjType - 3) <= 1;
                  intVal2 = (obj3[32].m_uIndexInArray & 0xFFFFFFFD) == 1;
                }
              }
              else if ( m_eObjType_high == -1 )
              {
                intVal2 ^= 1u;
              }
              if ( obj2 >= ++obj )
              {
                obj3 = *obj;
                if ( *obj )
                  continue;
              }
              break;
            }
          }
        }
      }
    }
    else
    {
      LevelTools::sortChannelOrderObjects(0, 0, (bool)dword_0 + 1);
    }
    LevelTools::sortChannelOrderObjects(objects, 0, (bool)dword_0 + 1);
  }
}


void LevelTools::toggleDebugLogging(bool enabled)
{
  ;
}


std::string LevelTools::urlForAudio(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;
  stl::StringData<char>::Internal *internal14;
  stl::StringData<char>::Internal *internal15;
  stl::StringData<char>::Internal *internal16;
  stl::StringData<char>::Internal *internal17;
  stl::StringData<char>::Internal *internal18;
  stl::StringData<char>::Internal *internal19;
  stl::StringData<char>::Internal *internal20;
  stl::StringData<char>::Internal *internal21;
  stl::StringData<char>::Internal *internal22;
  stl::StringData<char>::Internal *internal23;
  stl::StringData<char>::Internal *internal24;
  stl::StringData<char>::Internal *internal25;
  stl::StringData<char>::Internal *internal26;
  stl::StringData<char>::Internal *internal27;
  stl::StringData<char>::Internal *internal28;
  stl::StringData<char>::Internal *internal29;
  stl::StringData<char>::Internal *internal30;
  stl::StringData<char>::Internal *internal31;
  stl::StringData<char>::Internal *internal32;
  stl::StringData<char>::Internal *internal33;
  stl::StringData<char>::Internal *internal34;
  stl::StringData<char>::Internal *internal35;
  stl::StringData<char>::Internal *internal36;
  stl::StringData<char>::Internal *internal37;
  stl::StringData<char>::Internal *internal38;
  stl::StringData<char>::Internal *internal39;
  stl::StringData<char>::Internal *internal40;
  stl::StringData<char>::Internal *internal41;
  stl::StringData<char>::Internal *internal42;

  switch ( id )
  {
    case -1:
      internal42 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal42;
      break;
    case 0:
      internal41 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal41;
      break;
    case 1:
      internal40 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal40;
      break;
    case 2:
      internal39 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal39;
      break;
    case 3:
      internal38 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal38;
      break;
    case 4:
      internal37 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal37;
      break;
    case 5:
      internal36 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal36;
      break;
    case 6:
      internal35 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal35;
      break;
    case 7:
      internal34 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal34;
      break;
    case 8:
      internal33 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal33;
      break;
    case 9:
      internal32 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal32;
      break;
    case 10:
      internal31 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal31;
      break;
    case 11:
      internal30 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal30;
      break;
    case 12:
      internal29 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal29;
      break;
    case 13:
      internal28 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal28;
      break;
    case 14:
      internal27 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal27;
      break;
    case 15:
      internal26 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal26;
      break;
    case 16:
      internal25 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal25;
      break;
    case 17:
      internal24 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal24;
      break;
    case 18:
      internal23 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal23;
      break;
    case 19:
      internal22 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal22;
      break;
    case 20:
      internal21 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal21;
      break;
    case 21:
      internal20 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal20;
      break;
    case 22:
      internal19 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal19;
      break;
    case 23:
      internal18 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal18;
      break;
    case 24:
      internal17 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal17;
      break;
    case 25:
      internal16 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal16;
      break;
    case 27:
      internal15 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal15;
      break;
    case 28:
      internal14 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal14;
      break;
    case 29:
      internal13 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal13;
      break;
    case 30:
      internal12 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal12;
      break;
    case 31:
      internal11 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal11;
      break;
    case 32:
      internal10 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal10;
      break;
    case 33:
      internal9 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal9;
      break;
    case 34:
      internal8 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal8;
      break;
    case 35:
      internal7 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal7;
      break;
    case 36:
      internal6 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal6;
      break;
    case 37:
      internal5 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal5;
      break;
    case 38:
      internal4 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal4;
      break;
    case 39:
      internal3 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal3;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, &byte_F288A0);
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}


float LevelTools::valueForSpeedMod(int speed)
{
  unsigned int intVal;

  intVal = speed - 1;
  if ( intVal > 3 )
    return 311.58;
  else
    return *(float *)&byte_EA5080[4 * intVal + 128];
}


bool LevelTools::verifyLevelIntegrity(std::string str, int id)
{
  __int64 int64Val;
  bool returnVal;
  __int64 int64Val2;

  int64Val = *(_QWORD *)str.m_data.m_data;
  returnVal = 1;
  int64Val2 = *(_QWORD *)(int64Val - 24);
  switch ( id )
  {
    case 1:
      returnVal = (_DWORD)int64Val2 == 41941;
      break;
    case 2:
      returnVal = (_DWORD)int64Val2 == 27194;
      break;
    case 3:
      returnVal = (_DWORD)int64Val2 == 27485;
      break;
    case 4:
      returnVal = (_DWORD)int64Val2 == 28387;
      break;
    case 5:
      returnVal = (_DWORD)int64Val2 == 39306;
      break;
    case 6:
      returnVal = (_DWORD)int64Val2 == 32534;
      break;
    case 7:
      returnVal = (_DWORD)int64Val2 == 52577;
      break;
    case 8:
      returnVal = (_DWORD)int64Val2 == 68387;
      break;
    case 9:
      returnVal = (_DWORD)int64Val2 == 73863;
      break;
    case 10:
      returnVal = (_DWORD)int64Val2 == 95360;
      break;
    case 11:
      returnVal = (_DWORD)int64Val2 == 96482;
      break;
    case 12:
      returnVal = (_DWORD)int64Val2 == 91628;
      break;
    case 13:
      returnVal = (_DWORD)int64Val2 == 119893;
      break;
    case 14:
      returnVal = (_DWORD)int64Val2 == 162731;
      break;
    case 15:
      returnVal = (_DWORD)int64Val2 == 207047;
      break;
    case 16:
      returnVal = (_DWORD)int64Val2 == 238937;
      break;
    case 17:
      returnVal = (_DWORD)int64Val2 == 290335;
      break;
    case 18:
      returnVal = (_DWORD)int64Val2 == 370132;
      break;
    case 19:
      returnVal = (_DWORD)int64Val2 == 635171;
      break;
    case 20:
      returnVal = (_DWORD)int64Val2 == 739125;
      break;
    default:
      return returnVal;
  }
  return returnVal;
}


std::string LevelTools::ytURLForArtist(int id)
{
  stl::StringData<char>::Internal *internal;
  std::string returnVal;
  stl::StringData<char>::Internal *internal2;
  stl::StringData<char>::Internal *internal3;
  stl::StringData<char>::Internal *internal4;
  stl::StringData<char>::Internal *internal5;
  stl::StringData<char>::Internal *internal6;
  stl::StringData<char>::Internal *internal7;
  stl::StringData<char>::Internal *internal8;
  stl::StringData<char>::Internal *internal9;
  stl::StringData<char>::Internal *internal10;
  stl::StringData<char>::Internal *internal11;
  stl::StringData<char>::Internal *internal12;
  stl::StringData<char>::Internal *internal13;
  stl::StringData<char>::Internal *internal14;

  switch ( id )
  {
    case 0:
      internal5 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal5;
      break;
    case 1:
      internal6 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal6;
      break;
    case 2:
      internal7 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal7;
      break;
    case 3:
      internal8 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal8;
      break;
    case 4:
      internal9 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal9;
      break;
    case 5:
      internal10 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal10;
      break;
    case 6:
      internal11 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal11;
      break;
    case 7:
      internal12 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal12;
      break;
    case 8:
      internal13 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal13;
      break;
    case 9:
      internal14 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal14;
      break;
    case 10:
      internal3 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal3;
      break;
    case 11:
      internal4 = internal;
      sub_D153B8(internal, "https:
      returnVal.m_data.m_data = internal4;
      break;
    default:
      internal2 = internal;
      sub_D153B8(internal, &byte_F288A0);
      returnVal.m_data.m_data = internal2;
      break;
  }
  return returnVal;
}
