#pragma once

#include "Platform/cocos2d.h"
#include <string>

class PromoInterstitial : public FLAlertLayer {
public:
    static PromoInterstitial* create(bool fullVersion);

    bool init(bool fullVersion);

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    void keyBackClicked() override;
    void onClick(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void setup();
    void show() override;

    virtual ~PromoInterstitial();

protected:
    bool m_showAdBanner = false;
    std::string m_marketLink;
    int m_joystickConnected = -1;
    cocos2d::CCMenu* m_closeMenu = nullptr;
    bool m_promoTouched = false;
    cocos2d::CCMenu* m_buttonMenu = nullptr;
    bool m_fullVersion = false;
    cocos2d::CCLayer* m_mainLayer = nullptr;
};