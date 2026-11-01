#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "FLAlertLayerProtocol.h"
#include "GJAccountLoginDelegate.h"
#include "TextInputDelegate.h"
#include <string>

class CCTextInputNode;
class LoadingCircle;
class AccountError;

class AccountLoginLayer : public FLAlertLayer, public FLAlertLayerProtocol, public TextInputDelegate, public GJAccountLoginDelegate
{
public:
    static AccountLoginLayer* create(std::string username);
    
    bool init(std::string username);
    
    void disableNodes();
    void hideLoadingUI();
    void showLoadingUI();
    void toggleUI(bool enable);
    
    void resetLabel(int tag);
    void resetLabels();
    void updateLabel(AccountError type);
    
    void keyBackClicked();
    void registerWithTouchDispatcher();
    
    void onClose(cocos2d::CCObject* sender);
    void onSubmit(cocos2d::CCObject* sender);
    void onForgotUser(cocos2d::CCObject* sender);
    void onForgotPassword(cocos2d::CCObject* sender);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void loginAccountFailed(AccountError errorType);
    void loginAccountFinished(int accountID, int userID);
    
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void textInputOpened(CCTextInputNode* node);
    
    ~AccountLoginLayer();

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_usernameLabel;
    CCTextInputNode* m_usernameInput;
    cocos2d::CCLabelBMFont* m_passwordLabel;
    CCTextInputNode* m_passwordInput;
    LoadingCircle* m_loadingCircle;
    std::string m_username;
    std::string m_password;
    bool m_noElasticity;
    int m_joystickConnected;
};