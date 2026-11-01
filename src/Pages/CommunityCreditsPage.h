#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Nodes/CommunityCreditNode.h"

class CommunityCreditsPage : public FLAlertLayer {
public:
    static CommunityCreditsPage* create();
    void goToPage(int page);
    bool init() override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onSwitchPage(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void show() override;
    virtual ~CommunityCreditsPage();

    void FLAlert_Clicked(FLAlertLayer* alert, bool btn2) override;

protected:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    cocos2d::CCDictionary* m_pageObjects;
    CCMenuItemSpriteExtra* m_prevButton;
    CCMenuItemSpriteExtra* m_nextButton;
    cocos2d::CCMenu* m_buttonMenu;
    int m_page;
    int m_ZOrder;
    bool m_noElasticity;
};