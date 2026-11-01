#pragma once

#include "cocos2d.h"

class CCSpriteCOpacity : public cocos2d::CCSprite {
public:
    static CCSpriteCOpacity* createWithSpriteFrame(cocos2d::CCSpriteFrame* frame);
    static CCSpriteCOpacity* createWithSpriteFrameName(const char* frameName);

    virtual void setOpacity(unsigned __int8 opacity) override;

    ~CCSpriteCOpacity();
};