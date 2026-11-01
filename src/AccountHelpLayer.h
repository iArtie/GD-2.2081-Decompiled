#pragma once

#include "cocos2d.h"
#include "GJDropDownLayer.h"
#include "FLAlertLayerProtocol.h"
#include "GJAccountDelegate.h"

class TextArea;
class CCMenuItemSpriteExtra;
class FLAlertLayer;

class AccountHelpLayer : public GJDropDownLayer, public FLAlertLayerProtocol, public GJAccountDelegate
{
public:
    static AccountHelpLayer* create();
    
    void customSetup();
    void updatePage();
    void doUnlink();
    void verifyUnlink();
    void exitLayer();
    void layerHidden();
    
    void onReLogin(cocos2d::CCObject* sender);
    void onAccountManagement(cocos2d::CCObject* sender);
    void onUnlink(cocos2d::CCObject* sender);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    
    ~AccountHelpLayer();

private:
    TextArea* m_textArea;
    cocos2d::CCLabelBMFont* m_loginStatusLabel;
    CCMenuItemSpriteExtra* m_unlinkAccountButton;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    CCMenuItemSpriteExtra* m_refreshLoginButton;
    bool m_shouldShowAccountLayer;
};