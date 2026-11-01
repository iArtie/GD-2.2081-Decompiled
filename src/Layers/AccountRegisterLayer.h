#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/GJAccountRegisterDelegate.h"
#include <string>

class AccountRegisterLayer : public FLAlertLayer, public TextInputDelegate, public GJAccountRegisterDelegate, public FLAlertLayerProtocol {
public:
    static AccountRegisterLayer* create();
    
    bool init() override;
    ~AccountRegisterLayer() override;
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    bool allowTextInput(CCTextInputNode* node) override;
    
    void disableNodes();
    void hideLoadingUI();
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onSubmit(cocos2d::CCObject* sender);
    void registerAccountFailed(AccountError errorType) override;
    void registerAccountFinished() override;
    void registerWithTouchDispatcher() override;
    void resetLabel(int tag);
    void resetLabels();
    void showLoadingUI();
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void textInputReturn(CCTextInputNode* node) override;
    void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    void toggleUI(bool enable);
    void updateLabel(AccountError type);
    bool validEmail(std::string email);
    bool validPassword(std::string password);
    bool validUser(std::string username);

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    CCTextInputNode* m_usernameField;
    CCTextInputNode* m_passwordField;
    CCTextInputNode* m_confirmPasswordField;
    CCTextInputNode* m_emailField;
    CCTextInputNode* m_verifyEmailField;
    cocos2d::CCLabelBMFont* m_usernameLabel;
    cocos2d::CCLabelBMFont* m_passwordLabel;
    cocos2d::CCLabelBMFont* m_confirmPasswordLabel;
    cocos2d::CCLabelBMFont* m_emailLabel;
    cocos2d::CCLabelBMFont* m_verifyEmailLabel;
    LoadingCircle* m_loadingCircle;
    void* m_lockInput;
    bool m_noElasticity;
};