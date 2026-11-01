#pragma once

#include "Platform/cocos2d.h"

class GJUnlockableItem : public cocos2d::CCObject {
public:
    static GJUnlockableItem* create();
    bool init() override;
    virtual ~GJUnlockableItem();
};