#pragma once

#include "Platform/cocos2d.h"

class CCLightStrip : public cocos2d::CCNode {
public:
    static CCLightStrip* create(float width, float toWidth, float toHeight, float duration, float delay);

    virtual void draw() override;
    bool init(float width, float toWidth, float toHeight, float duration, float delay);
    void updateTweenAction(float value, const char* keyword);

    ~CCLightStrip();

private:
    cocos2d::ccColor3B m_color;
    float m_opacity;
    float m_objectWidth;
    float m_width;
    float m_height;
    float m_toWidth;
    float m_toHeight;
    float m_duration;
};