#include "LoadingCircleSprite.h"

LoadingCircleSprite* LoadingCircleSprite::create(float spinSpeed)
{
    LoadingCircleSprite* ret = new LoadingCircleSprite();
    if (ret->init(spinSpeed)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void LoadingCircleSprite::hideCircle()
{
    stopActionByTag(0);
    setOpacity(0);
    setVisible(false);
}

bool LoadingCircleSprite::init(float spinSpeed)
{
    if (!cocos2d::CCSprite::initWithFile("loadingCircle.png"))
        return false;

    setBlendFunc(cocos2d::ccBlendFunc{ GL_SRC_ALPHA, GL_ONE });
    cocos2d::CCRotateBy* rotateBy = cocos2d::CCRotateBy::create(1.0f, spinSpeed * 360.0f);
    cocos2d::CCRepeatForever* repeat = cocos2d::CCRepeatForever::create(rotateBy);
    runAction(repeat);

    return true;
}

LoadingCircleSprite::~LoadingCircleSprite()
{
}