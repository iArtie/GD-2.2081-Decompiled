#pragma once

#include "cocos2d.h"

class ColorSelectDelegate {
public:
    virtual ~ColorSelectDelegate() {}

    virtual void colorSelectClosed(cocos2d::CCNode* popup);
};