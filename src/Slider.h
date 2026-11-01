#pragma once

#include "cocos2d.h"
#include "SliderTouchLogic.h"
#include "SliderThumb.h"
#include "SliderDelegate.h"

class Slider : public cocos2d::CCLayer {
public:
    static Slider* create(cocos2d::CCNode* target, cocos2d::SEL_MenuHandler handler);
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void disableSlider();
    void disableTouch();
    void enableSlider();
    bool getLiveDragging();
    SliderThumb* getThumb();
    float getValue();
    void hideGroove(bool visibility);
    void setBarVisibility(bool visibility);
    void setLiveDragging(bool activateThumb);
    void setMaxOffset(float offset);
    void setRotated(bool rotated);
    void setValue(float val);
    void sliderBegan();
    void sliderEnded();
    void updateBar();
    virtual ~Slider();

protected:
    SliderTouchLogic* m_touchLogic = nullptr;
    bool m_enabled = true;
    cocos2d::CCSprite* m_sliderBar = nullptr;
    cocos2d::CCSprite* m_groove = nullptr;
    float m_width = 0.0f;
    float m_height = 0.0f;
    SliderDelegate* m_delegate = nullptr;
};