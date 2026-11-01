#pragma once

#include "cocos2d.h"

class SimpleObject : public cocos2d::CCObject {
public:
    static SimpleObject* create();
    bool init();
    virtual ~SimpleObject();
};