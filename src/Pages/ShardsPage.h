#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Utils/GJEnums.h"

class ShardsPage : public FLAlertLayer {
public:
    static ShardsPage* create();
    bool init() override;
    void keyBackClicked() override;
    void registerWithTouchDispatcher() override;
    void show() override;
    void FLAlert_Clicked(FLAlertLayer* alert, bool btn2) override;
    void goToPage(int page);
    void onClose(cocos2d::CCObject* sender);
    void onIconInfo(cocos2d::CCObject* sender);
    void onSwitchPage(cocos2d::CCObject* sender);
    virtual ~ShardsPage();

protected:
    int m_page = -1;
    cocos2d::CCDictionary* m_pages = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    cocos2d::CCSprite* m_tierSprite = nullptr;
};