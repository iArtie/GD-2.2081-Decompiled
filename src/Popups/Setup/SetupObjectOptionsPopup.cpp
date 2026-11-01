#include "Popups/Setup/SetupObjectOptionsPopup.h"

void SetupObjectOptionsPopup::determineStartValues()
{
  cocos2d::CCObject *obj;
  unsigned int intVal;
  cocos2d::CCObject *obj2;

  if ( m_gameObject )
  {
    m_gameObjects = (cocos2d::CCArray *)cocos2d::CCArray::create((cocos2d::CCArray *)m_gameObject);
    cocos2d::CCArray::addObject(m_gameObjects, (cocos2d::_ccArray *)m_gameObject, obj);
  }
  else
  {
  }
  if ( m_gameObjects )
  {
    if ( (unsigned int)cocos2d::CCArray::count(m_gameObjects) )
    {
      m_effectObject = 0;
      m_iceBlock = 1;
      intVal = 0;
      m_dontFade = 1;
      m_dontEnter = 1;
      m_noEffects = 1;
      m_noParticle = 1;
      m_groupParent = 1;
      m_areaParent = 1;
      m_highDetail = 1;
      m_noTouch = 1;
      m_passable = 1;
      m_hide = 1;
      m_nonStickX = 1;
      m_nonStickY = 1;
      m_extraSticky = 1;
      m_dontBoostY = 1;
      m_dontBoostX = 1;
      m_scaleStick = 1;
      m_gripSlope = 1;
      m_noGlow = 1;
      m_unk2a6 = 1;
      m_reverse = 1;
      m_singlePlayerTouch = 1;
      m_centerEffect = 1;
      m_extendedCollision = 1;
      m_noAudioScale = 1;
      while ( intVal < (unsigned int)cocos2d::CCArray::count(m_gameObjects) )
      {
        obj2 = cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal);
        if ( !BYTE1(obj2[22].m_uOrderOfArrival) )
          m_iceBlock = 0;
        if ( !HIBYTE(obj2[18].m_nTag) )
          m_dontFade = 0;
        if ( !BYTE2(obj2[18].m_nTag) )
          m_dontEnter = 0;
        if ( !BYTE2(obj2[20].m_nLuaID) )
          m_groupParent = 0;
        if ( !HIBYTE(obj2[20].m_nLuaID) )
          m_areaParent = 0;
        if ( !HIBYTE(obj2[22].m_uID) )
          m_highDetail = 0;
        if ( !LOBYTE(obj2[18].m_uReference) )
          m_noEffects = 0;
        if ( !BYTE1(obj2[18].m_uReference) )
          m_noParticle = 0;
        if ( !BYTE2(obj2[21].m_uReference) )
          m_noTouch = 0;
        if ( !BYTE1(obj2[22].m_nZOrder) )
          m_passable = 0;
        if ( !BYTE2(obj2[22].m_nZOrder) )
          m_hide = 0;
        if ( !LOBYTE(obj2[11].m_uReference) )
          m_extendedCollision = 0;
        if ( !BYTE1(obj2[17].__vftable) )
          m_noAudioScale = 0;
        if ( !HIBYTE(obj2[22].m_nZOrder) )
          m_nonStickX = 0;
        if ( !LOBYTE(obj2[22].m_uOrderOfArrival) )
          m_nonStickY = 0;
        if ( !LOBYTE(obj2[22].m_unknown5) )
          m_extraSticky = 0;
        if ( !BYTE1(obj2[22].m_unknown5) )
          m_dontBoostY = 0;
        if ( !BYTE2(obj2[22].m_unknown5) )
          m_dontBoostX = 0;
        if ( !HIBYTE(obj2[22].m_uOrderOfArrival) )
          m_scaleStick = 0;
        if ( !BYTE2(obj2[22].m_uOrderOfArrival) )
          m_gripSlope = 0;
        if ( !LOBYTE(obj2[17].m_unknown5) )
          m_noGlow = 0;
        if ( LODWORD(obj2[22].__vftable) != 1 )
          goto LABEL_49;
        m_effectObject = 1;
        if ( !LOBYTE(obj2[31].m_eObjType) )
          m_reverse = 0;
        if ( !LOBYTE(obj2[30].m_nZOrder) )
          m_singlePlayerTouch = 0;
        if ( BYTE2(obj2[26].__vftable) )
        {
LABEL_49:
          ++intVal;
        }
        else
        {
          m_centerEffect = 0;
          ++intVal;
        }
      }
    }
  }
}


void SetupObjectOptionsPopup::keyBackClicked()
{
  onClose(this, 0);
}


void SetupObjectOptionsPopup::onAlwaysHide(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_hide;
  m_hide = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isHide = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE2(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_nZOrder) = m_hide;
    }
  }
}


void SetupObjectOptionsPopup::onApplyScaleStick(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_scaleStick;
  m_scaleStick = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isScaleStick = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      HIBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_uOrderOfArrival) = m_scaleStick;
    }
  }
}


void SetupObjectOptionsPopup::onCenterEffect(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;
  cocos2d::CCObject *obj;

  flag = !m_centerEffect;
  m_centerEffect = flag;
  if ( m_gameObject )
  {
    if ( m_gameObject->m_classType == Effect )
      BYTE2(m_gameObject[1].m_sAdditionalTransform.b) = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      obj = cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2);
      if ( LODWORD(obj[22].__vftable) == 1 )
        BYTE2(obj[26].__vftable) = m_centerEffect;
    }
  }
}


void SetupObjectOptionsPopup::onClose(cocos2d::CCObject *sender)
{
  setKeypadEnabled(this, 0);
  removeFromParentAndCleanup(this, 1);
}


void SetupObjectOptionsPopup::onDisableGlow(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_noGlow;
  m_noGlow = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_hasNoGlow = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      LOBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[17].m_unknown5) = m_noGlow;
    }
  }
}


void SetupObjectOptionsPopup::onDisableObject(cocos2d::CCObject *sender)
{
  cocos2d::CCObject *obj;
  LevelEditorLayer *m_levelEditorLayer;
  unsigned int intVal;
  bool flag;
  int intVal2;
  cocos2d::CCArray *i;
  cocos2d::CCObject *obj2;
  GameObject *gameObject2;
  __int64 m_savedObjectType;
  void (__fastcall *setType)(GameObject *__hidden, GameObjectType);
  cocos2d::CCDataVisitor *m_nLuaID;
  void (__fastcall *acceptVisitor)(cocos2d::CCObject *__hidden, cocos2d::CCDataVisitor *);

  obj = (cocos2d::CCObject *)!m_noTouch;
  m_noTouch = (char)obj;
  if ( m_gameObject )
    m_gameObjects = cocos2d::CCArray::createWithObject((cocos2d::CCArray *)m_gameObject, obj);
  else
  m_levelEditorLayer = GameManager::sharedState()->m_levelEditorLayer;
  intVal = 0;
  flag = (unsigned int)cocos2d::CCArray::count(m_gameObjects) == 0;
  intVal2 = 0;
  for ( i = m_gameObjects; !flag; i = m_gameObjects )
  {
    obj2 = cocos2d::CCArray::objectAtIndex(i, intVal2);
    gameObject2 = (GameObject *)obj2;
    ++intVal;
    if ( m_levelEditorLayer )
    {
      GJBaseGameLayer::removeObjectFromSection(m_levelEditorLayer, (GameObject *)obj2);
      m_savedObjectType = (unsigned int)gameObject2->m_savedObjectType;
      setType = gameObject2->setType;
      gameObject2->m_isNoTouch = m_noTouch;
      setType(gameObject2, (GameObjectType)m_savedObjectType);
      gameObject2->saveActiveColors(gameObject2);
      m_levelEditorLayer->addToSection(m_levelEditorLayer, gameObject2);
    }
    else
    {
      m_nLuaID = (cocos2d::CCDataVisitor *)(unsigned int)obj2[16].m_nLuaID;
      acceptVisitor = obj2->__vftable[22].acceptVisitor;
      BYTE2(obj2[21].m_uReference) = m_noTouch;
      acceptVisitor(obj2, m_nLuaID);
      gameObject2->saveActiveColors(gameObject2);
    }
    flag = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
    intVal2 = intVal;
  }
}


void SetupObjectOptionsPopup::onDontBoost(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_dontBoostY;
  m_dontBoostY = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isDontBoostY = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE1(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_unknown5) = m_dontBoostY;
    }
  }
}


void SetupObjectOptionsPopup::onDontBoostX(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_dontBoostX;
  m_dontBoostX = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isDontBoostX = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE2(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_unknown5) = m_dontBoostX;
    }
  }
}


void SetupObjectOptionsPopup::onDontEnter(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_dontEnter;
  m_dontEnter = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isDontEnter = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE2(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[18].m_nTag) = m_dontEnter;
    }
  }
}


void SetupObjectOptionsPopup::onDontFade(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_dontFade;
  m_dontFade = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isDontFade = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      HIBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[18].m_nTag) = m_dontFade;
    }
  }
}


void SetupObjectOptionsPopup::onExtendedCollision(cocos2d::CCObject *sender)
{
  LevelEditorLayer *m_levelEditorLayer;
  bool flag;
  unsigned int intVal;
  GameObject *gameObject2;
  bool flag2;
  int intVal2;

  m_levelEditorLayer = GameManager::sharedState()->m_levelEditorLayer;
  if ( m_levelEditorLayer )
  {
    flag = !m_extendedCollision;
    m_extendedCollision = flag;
    if ( m_gameObject )
    {
      GJBaseGameLayer::updateExtendedCollision(m_levelEditorLayer, m_gameObject, flag);
    }
    else
    {
      intVal = 0;
      while ( 1 )
      {
        flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
        intVal2 = intVal++;
        if ( flag2 )
          break;
        gameObject2 = (GameObject *)cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2);
        GJBaseGameLayer::updateExtendedCollision(m_levelEditorLayer, gameObject2, m_extendedCollision);
      }
    }
  }
}


void SetupObjectOptionsPopup::onExtraSticky(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_extraSticky;
  m_extraSticky = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isExtraSticky = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      LOBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_unknown5) = m_extraSticky;
    }
  }
}


void SetupObjectOptionsPopup::onGripSlope(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_gripSlope;
  m_gripSlope = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isGripSlope = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE2(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_uOrderOfArrival) = m_gripSlope;
    }
  }
}


void SetupObjectOptionsPopup::onHideEffects(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_noEffects;
  m_noEffects = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_hasNoEffects = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      LOBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[18].m_uReference) = m_noEffects;
    }
  }
}


void SetupObjectOptionsPopup::onHighDetail(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_highDetail;
  m_highDetail = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isHighDetail = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      HIBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_uID) = m_highDetail;
    }
  }
}


void SetupObjectOptionsPopup::onIceBlock(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_iceBlock;
  m_iceBlock = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isIceBlock = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE1(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_uOrderOfArrival) = m_iceBlock;
    }
  }
}


void SetupObjectOptionsPopup::onNoAudioScale(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_noAudioScale;
  m_noAudioScale = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_hasNoAudioScale = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE1(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[17].__vftable) = m_noAudioScale;
    }
  }
}


void SetupObjectOptionsPopup::onNoParticle(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_noParticle;
  m_noParticle = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_hasNoParticles = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE1(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[18].m_uReference) = m_noParticle;
    }
  }
}


void SetupObjectOptionsPopup::onNonStickX(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_nonStickX;
  m_nonStickX = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isNonStickX = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      HIBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_nZOrder) = m_nonStickX;
    }
  }
}


void SetupObjectOptionsPopup::onNonStickY(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_nonStickY;
  m_nonStickY = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isNonStickY = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      LOBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_uOrderOfArrival) = m_nonStickY;
    }
  }
}


void SetupObjectOptionsPopup::onPassable(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_passable;
  m_passable = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_isPassable = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE1(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[22].m_nZOrder) = m_passable;
    }
  }
}


void SetupObjectOptionsPopup::onReverse(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;
  cocos2d::CCObject *obj;

  if ( m_groupIDLayer )
    m_groupIDLayer->m_reverseChanged = 1;
  flag = !m_reverse;
  m_reverse = flag;
  if ( m_gameObject )
  {
    if ( m_gameObject->m_classType == Effect )
      LOBYTE(m_gameObject[1].m_obRect.origin.y) = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      obj = cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2);
      if ( LODWORD(obj[22].__vftable) == 1 )
        LOBYTE(obj[31].m_eObjType) = m_reverse;
    }
  }
}


void SetupObjectOptionsPopup::onSinglePlayerTouch(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;
  cocos2d::CCObject *obj;

  flag = !m_singlePlayerTouch;
  m_singlePlayerTouch = flag;
  if ( m_gameObject )
  {
    if ( m_gameObject->m_classType == Effect )
      LOBYTE(m_gameObject[1].m_transformToBatch.c) = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      obj = cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2);
      if ( LODWORD(obj[22].__vftable) == 1 )
        LOBYTE(obj[30].m_nZOrder) = m_singlePlayerTouch;
    }
  }
}


void SetupObjectOptionsPopup::onToggleAreaParent(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_areaParent;
  m_areaParent = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_hasAreaParent = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      HIBYTE(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[20].m_nLuaID) = m_areaParent;
    }
  }
}


void SetupObjectOptionsPopup::onToggleGroupParent(cocos2d::CCObject *sender)
{
  bool flag;
  unsigned int intVal;
  bool flag2;
  int intVal2;

  flag = !m_groupParent;
  m_groupParent = flag;
  if ( m_gameObject )
  {
    m_gameObject->m_hasGroupParent = flag;
  }
  else
  {
    intVal = 0;
    while ( 1 )
    {
      flag2 = intVal >= (unsigned int)cocos2d::CCArray::count(m_gameObjects);
      intVal2 = intVal++;
      if ( flag2 )
        break;
      BYTE2(cocos2d::CCArray::objectAtIndex(m_gameObjects, intVal2)[20].m_nLuaID) = m_groupParent;
    }
  }
}


void SetupObjectOptionsPopup::show()
{
  cocos2d::CCScene *m_pRunningScene;

  m_pRunningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
  m_ZOrder = (unsigned int)m_ZOrder;
  if ( !m_scene )
    m_scene = m_pRunningScene;
  if ( !(_DWORD)m_ZOrder )
  {
    m_ZOrder = 105;
    m_ZOrder = 105;
  }
  m_scene->addChild_0(m_scene, this, m_ZOrder);
}


SetupObjectOptionsPopup::~SetupObjectOptionsPopup()
{
  SetupObjectOptionsPopup::~SetupObjectOptionsPopup(this);
  operator delete(this);
}
