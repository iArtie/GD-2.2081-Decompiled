#pragma once

#include "cocos2d.h"

class CCCircleAlert : public cocos2d::CCNode {
public:
    static CCCircleAlert* create(float duration, float startRadius, float endRadius);
    bool init(float duration, float startRadius, float endRadius);
    ~CCCircleAlert();

private:
    cocos2d::ccColor3B m_color;
    float m_radius;
    unsigned char m_opacity;
};