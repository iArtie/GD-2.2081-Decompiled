#pragma once

#include "Platform/cocos2d.h"

class SliderThumb : public cocos2d::CCMenuItemImage {
public:
    float getValue();
    void setMaxOffset(float offset);
    void setRotated(bool rotated);
    void setValue(float value);
    virtual ~SliderThumb();

protected:
    bool m_vertical = false;
    float m_length = 0.0f;
};