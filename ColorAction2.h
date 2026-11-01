#pragma once

#include "cocos2d.h"

class ColorAction2 {
public:
    void resetAction();
    void step(float dt);

private:
    bool m_stepFinished;
    bool m_paused;
    float m_deltaTime;
    float m_duration;
    float m_stepInterval;
    cocos2d::ccColor3B m_fromColor;
    cocos2d::ccColor3B m_toColor;
    float m_fromOpacity;
    cocos2d::ccColor3B m_color;
    float m_toOpacity;
    float m_currentOpacity;
};