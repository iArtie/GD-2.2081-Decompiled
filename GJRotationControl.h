#pragma once

#include "cocos2d.h"

class GJRotationControlDelegate;

class GJRotationControl : public cocos2d::CCLayer {
public:
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    static GJRotationControl* create();
    void draw() override;
    void finishTouch();
    bool init() override;
    void setAngle(float angle);
    void updateSliderPosition(cocos2d::CCPoint position);
    virtual ~GJRotationControl();

private:
    int m_touchID;
    cocos2d::CCPoint m_controlPosition;
    float m_startingRotation;
    float m_currentRotation;
    cocos2d::CCPoint m_cursorDifference;
    GJRotationControlDelegate* m_delegate;
    cocos2d::CCSprite* m_controlSprite;
};