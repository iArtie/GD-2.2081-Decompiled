#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class UIPOptionsLayer : public SetupTriggerPopup {
public:
    static UIPOptionsLayer* create();
    bool init() override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void registerWithTouchDispatcher() override;

    cocos2d::CCRect getTouchRect();
    float getValue(int tag);
    void onClose(cocos2d::CCObject* sender);
    void onReset(cocos2d::CCObject* sender);
    void valueDidChange(int tag, float value);

    ~UIPOptionsLayer();

protected:
    int m_touchID = -1;
    cocos2d::CCPoint m_touchPosition;
    cocos2d::CCPoint m_touchDelta;
    cocos2d::CCNode* m_practiceNode = nullptr;
};