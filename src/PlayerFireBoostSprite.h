#pragma once

#include "cocos2d.h"

class PlayerFireBoostSprite : public cocos2d::CCSprite {
public:
    static PlayerFireBoostSprite* create();

    bool init() override;

    void animateFireIn();
    void animateFireOut();
    void loopFireAnimation();

    virtual ~PlayerFireBoostSprite();

protected:
    float m_size = 1.0f;
};