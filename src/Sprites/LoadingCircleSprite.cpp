#include "Sprites/LoadingCircleSprite.h"

LoadingCircleSprite * LoadingCircleSprite::create(float spinSpeed)
{
  LoadingCircleSprite *returnVal;

  returnVal = (LoadingCircleSprite *)operator new(0x258u);
  cocos2d::CCSprite::CCSprite(returnVal);
  returnVal->cocos2d::CCSprite::cocos2d::CCNodeRGBA::cocos2d::CCNode::cocos2d::CCObject::cocos2d::CCCopying::__vftable = (LoadingCircleSprite_vtbl *)&off_113CB80;
  returnVal->cocos2d::CCSprite::cocos2d::CCNodeRGBA::cocos2d::CCRGBAProtocol::__vftable = (cocos2d::CCRGBAProtocol_vtbl *)&off_113D058;
  returnVal->cocos2d::CCSprite::cocos2d::CCTextureProtocol::cocos2d::CCBlendProtocol::__vftable = (cocos2d::CCTextureProtocol_vtbl *)&off_113D0D8;
  if ( LoadingCircleSprite::init(returnVal, spinSpeed) )
  {
    cocos2d::CCObject::autorelease(returnVal);
    return returnVal;
  }
  else
  {
    returnVal->~LoadingCircleSprite_0(returnVal);
    return 0;
  }
}


void LoadingCircleSprite::hideCircle()
{
  cocos2d::CCNode::stopActionByTag(this, 0);
  setOpacity(this, 0);
  setVisible(this, 0);
}


bool LoadingCircleSprite::init(float spinSpeed)
{
  char returnVal;
  cocos2d::CCRotateBy *cCRotateBy;
  cocos2d::CCActionInterval *cCActionInterval;
  cocos2d::CCRepeatForever *cCRepeatForever;

  returnVal = cocos2d::CCSprite::initWithFile(this, "loadingCircle.png");
  if ( returnVal )
  {
    cCRotateBy = (cocos2d::CCRotateBy *)((__int64 (__fastcall *)(LoadingCircleSprite *, __int64))setBlendFunc)(
                                  this,
                                  0x100000302LL);
    cCActionInterval = (cocos2d::CCActionInterval *)cocos2d::CCRotateBy::create(cCRotateBy, 1.0, spinSpeed * 360.0);
    cCRepeatForever = cocos2d::CCRepeatForever::create(cCActionInterval);
    cocos2d::CCNode::runAction(this, cCRepeatForever);
  }
  return returnVal;
}


LoadingCircleSprite::~LoadingCircleSprite()
{
  LoadingCircleSprite::~LoadingCircleSprite(this);
  operator delete(this);
}
