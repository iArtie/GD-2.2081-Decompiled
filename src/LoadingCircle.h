#pragma once

#include "cocos2d.h"

class LoadingCircleSprite : public cocos2d::CCSprite {
public:
    static LoadingCircleSprite* create(float spinSpeed);
    void hideCircle();
    bool init(float spinSpeed);
    ~LoadingCircleSprite();
};