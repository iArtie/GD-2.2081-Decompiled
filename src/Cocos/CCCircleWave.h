#pragma once

#include "Platform/cocos2d.h"

class CCCircleWaveDelegate;

enum CircleMode {
    Filled = 0,
    Outline = 1
};

class CCCircleWave : public cocos2d::CCNode {
public:
    void baseSetup(float radius);
    virtual void draw() override;
    void followObject(cocos2d::CCNode* newTarget, bool staticPosition);
    void removeMeAndCleanup();
    virtual void setPosition(const cocos2d::CCPoint* position) override;
    void updatePosition(float dt);
    void updateTweenAction(float value, const char* key);
    ~CCCircleWave();

protected:
    cocos2d::CCNode* m_target;
    cocos2d::CCPoint m_position;
    bool m_blendAdditive;
    CircleMode m_circleMode;
    float m_opacity;
    float m_opacityMod;
    cocos2d::ccColor3B m_color;
    float m_radius;
    CCCircleWaveDelegate* m_delegate;
    float m_lineWidth;
};