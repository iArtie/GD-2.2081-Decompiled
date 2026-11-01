#pragma once

#include "Platform/cocos2d.h"

class GhostTrailEffect : public cocos2d::CCNode {
public:
    static GhostTrailEffect* create();
    bool init() override;
    void draw() override;
    void doBlendAdditive();
    void stopTrail();
    virtual ~GhostTrailEffect();

    cocos2d::ccColor3B m_color;
    cocos2d::ccBlendFunc m_blendFunc;
    float m_opacity;
    float m_playerScale;
    void* m_delegate;
};