// GJFlyGroundLayer.h
#pragma once

#include "Platform/cocos2d.h"

class GJFlyGroundLayer : public cocos2d::CCLayer {
public:
    static GJFlyGroundLayer* create();
    bool init();
    virtual ~GJFlyGroundLayer();

    bool m_showGround;
};