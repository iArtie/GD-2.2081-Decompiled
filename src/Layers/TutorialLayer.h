#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"

class CCMenuItemSpriteExtra;

class TutorialLayer : public FLAlertLayer {
public:
    static TutorialLayer* create();
    bool init() override;
    void keyBackClicked() override;
    void loadPage(int page);
    void onClose(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void removeTutorialTexture();
    virtual ~TutorialLayer();

protected:
    int m_page = 0;
    cocos2d::CCLayer* m_tutorialLayer = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    CCMenuItemSpriteExtra* m_exitButton = nullptr;
};