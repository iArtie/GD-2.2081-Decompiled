#pragma once

#include "Platform/cocos2d.h"

class CCCircleAlert;

class CCAlertCircle : public cocos2d::CCNode {
public:
    static CCAlertCircle* create();
    virtual void draw() override;
    bool init();
    ~CCAlertCircle();

private:
    CCCircleAlert* m_circleAlert;
};