#pragma once

#include "Platform/cocos2d.h"
#include "UI/SliderThumb.h"
#include "UI/Slider.h"

class SliderTouchLogic : public cocos2d::CCMenu {
public:
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void registerWithTouchDispatcher() override;
    void setMaxOffset(float offset);
    void setRotated(bool rotated);
    virtual ~SliderTouchLogic();

protected:
    SliderThumb* m_thumb = nullptr;
    float m_length = 0.0f;
    bool m_rotated = false;
    cocos2d::CCPoint m_position;
    bool m_activateThumb = false;
    Slider* m_slider = nullptr;
};