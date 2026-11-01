#pragma once

#include "cocos2d.h"

class Slider;
class CCMenuItemSpriteExtra;
class GJScaleControlDelegate;

class GJScaleControl : public cocos2d::CCLayer {
public:
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    static GJScaleControl* create();
    void finishTouch();
    bool init() override;
    void onToggleLockScale(cocos2d::CCObject* sender);
    float scaleFromValue(float value);
    float skewFromValue(float value);
    void sliderChanged(cocos2d::CCObject* sender);
    void updateLabelX(float value);
    void updateLabelXY(float value);
    void updateLabelY(float value);
    float valueFromScale(float scale);
    float valueFromSkew(float value);
    virtual ~GJScaleControl();

private:
    bool m_scaleLocked;
    CCMenuItemSpriteExtra* m_scaleLockButton;
    Slider* m_sliderX;
    Slider* m_sliderY;
    Slider* m_sliderXY;
    int m_touchID;
    cocos2d::CCLabelBMFont* m_scaleXLabel;
    cocos2d::CCLabelBMFont* m_scaleYLabel;
    cocos2d::CCLabelBMFont* m_scaleLabel;
    GJScaleControlDelegate* m_delegate;
    float m_lowerBound;
    float m_upperBound;
    float m_valueX;
    int m_scaleButtonType;
    float m_valueY;
    bool m_scale1Lock;
    float m_changedValueX;
    float m_changedValueY;
};