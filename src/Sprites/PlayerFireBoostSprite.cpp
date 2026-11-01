#include "Sprites/PlayerFireBoostSprite.h"

PlayerFireBoostSprite* PlayerFireBoostSprite::create()
{
    PlayerFireBoostSprite* returnVal = new PlayerFireBoostSprite();
    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool PlayerFireBoostSprite::init()
{
    if (!cocos2d::CCSprite::initWithFile("fireBoost_001.png"))
        return false;

    setAnchorPoint(cocos2d::CCPoint(0.5f, 1.0f));

    return true;
}

void PlayerFireBoostSprite::animateFireIn()
{
    stopAllActions();

    cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.06f, m_size * 0.6f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(PlayerFireBoostSprite::loopFireAnimation));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(scaleTo, callFunc, nullptr);
    runAction(seq);
}

void PlayerFireBoostSprite::animateFireOut()
{
    stopAllActions();

    cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.4f, 0.01f);
    runAction(scaleTo);
}

void PlayerFireBoostSprite::loopFireAnimation()
{
    stopAllActions();

    cocos2d::CCScaleTo* scaleUp = cocos2d::CCScaleTo::create(0.06f, m_size * 1.2f);
    cocos2d::CCScaleTo* scaleDown = cocos2d::CCScaleTo::create(0.06f, m_size * 0.6f);
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(scaleUp, scaleDown, nullptr);
    cocos2d::CCRepeatForever* repeat = cocos2d::CCRepeatForever::create(seq);
    runAction(repeat);
}

PlayerFireBoostSprite::~PlayerFireBoostSprite()
{
}