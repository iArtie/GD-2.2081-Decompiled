#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJPromoPopup : public FLAlertLayer {
public:
    static GJPromoPopup* create(const std::string& filename);
    bool init(const std::string& filename) override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onExit() override;
    void registerWithTouchDispatcher() override;
    void show() override;
    virtual ~GJPromoPopup();

private:
    std::string m_promoFrame;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    cocos2d::CCMenu* m_buttonMenu;
};