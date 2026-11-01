#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/FLAlertLayerProtocol.h"

class ScrollingLayer;
class CCMenuItemSpriteExtra;

class FLAlertLayer : public cocos2d::CCLayerColor, public FLAlertLayerProtocol
{
public:
    FLAlertLayer* FLAlertLayer::create(const char* title, const std::string* desc, const char* btn);
    
    void incrementForcePrio();
    virtual void keyBackClicked();
    virtual void keyDown(cocos2d::enumKeyCodes key, double timestamp);
    virtual void onBtn1(cocos2d::CCObject* sender);
    virtual void onBtn2(cocos2d::CCObject* sender);
    virtual void registerWithTouchDispatcher();
    virtual void show();
    
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    
    virtual ~FLAlertLayer();

protected:
    FLAlertLayerProtocol* m_alertProtocol;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    ScrollingLayer* m_scrollingLayer;
    bool m_reverseKeyBack;
    bool m_noElasticity;
    bool m_noAction;
    bool m_forcePrioRegistered;
    int m_joystickConnected;
    int m_controlConnected;
};