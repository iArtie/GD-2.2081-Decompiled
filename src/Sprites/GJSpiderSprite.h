#pragma once

#include "Platform/cocos2d.h"

class GJSpiderSprite : public GJRobotSprite {
public:
    static GJSpiderSprite* create(int frame);
    bool init(int frame) override;
    virtual ~GJSpiderSprite();

private:
    IconType m_iconType;
};