#pragma once

#include "cocos2d.h"

class PlayerCheckpoint : public cocos2d::CCNode {
public:
    static PlayerCheckpoint* create();

    bool init() override;

    virtual ~PlayerCheckpoint();

protected:
    cocos2d::CCArray* m_touchingRings = nullptr;
};