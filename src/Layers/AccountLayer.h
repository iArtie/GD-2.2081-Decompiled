#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/GJAccountDelegate.h"
#include "Delegates/GJAccountBackupDelegate.h"
#include "Delegates/GJAccountSyncDelegate.h"

class TextArea;
class CCMenuItemSpriteExtra;
class LoadingCircle;
class FLAlertLayer;

class AccountLayer : public cocos2d::CCLayer, public FLAlertLayerProtocol, public GJAccountDelegate, public GJAccountBackupDelegate, public GJAccountSyncDelegate
{
public:
    static AccountLayer* create();
    
    void customSetup();
    void updatePage(bool changed);
    void doBackup();
    void doSync();
    void showLoadingUI();
    void hideLoadingUI();
    void toggleUI(bool enable);
    void exitLayer();
    void layerHidden();
    
    void onLogin(cocos2d::CCObject* sender);
    void onRegister(cocos2d::CCObject* sender);
    void onBackup(cocos2d::CCObject* sender);
    void onSync(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void onMore(cocos2d::CCObject* sender);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void accountStatusChanged();
    void backupAccountFailed(BackupAccountError errorType, int response);
    void backupAccountFinished();
    void syncAccountFailed(BackupAccountError errorType, int response);
    void syncAccountFinished();
    
    ~AccountLayer();

private:
    TextArea* m_textArea;
    cocos2d::CCLabelBMFont* m_linkedAccountTitle;
    CCMenuItemSpriteExtra* m_loginButton;
    CCMenuItemSpriteExtra* m_registerButton;
    CCMenuItemSpriteExtra* m_backupButton;
    CCMenuItemSpriteExtra* m_syncButton;
    CCMenuItemSpriteExtra* m_helpButton;
    CCMenuItemSpriteExtra* m_moreButton;
    bool m_isLoggedIn;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    LoadingCircle* m_loadingCircle;
    bool m_accountHelpRelated;
};