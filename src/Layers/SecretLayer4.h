#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/DialogDelegate.h"
#include <string>

class CCTextInputNode;
class CCMenuItemSpriteExtra;
class FLAlertLayer;
class DialogLayer;

class SecretLayer4 : public cocos2d::CCLayer, public TextInputDelegate, public FLAlertLayerProtocol, public DialogDelegate {
public:
    static SecretLayer4* create();
    static cocos2d::CCScene* scene();

    bool init() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void dialogClosed(DialogLayer* layer) override;

    std::string getBasicMessage();
    std::string getErrorMessage();
    std::string getMessage();
    std::string getThreadMessage();

    void keyBackClicked() override;

    void onBack(cocos2d::CCObject* sender);
    void onChest01(cocos2d::CCObject* sender);
    void onExit() override;
    void onSubmit(cocos2d::CCObject* sender);
    void playCoinEffect();
    void selectAThread();
    void showDialog(int index);
    void showFirstDialog();

    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;

    void updateMessageLabel(std::string text);
    void updateSearchLabel(const char* text);

    virtual ~SecretLayer4();

protected:
    CCTextInputNode* m_searchInput = nullptr;
    bool m_exiting = false;
    cocos2d::CCLabelBMFont* m_messageLabel = nullptr;
    cocos2d::CCDictionary* m_messageThreads = nullptr;
    int m_threadTag = -1;
    int m_selectedThread = 0;
    int m_threadMessageIndex = 0;
    int m_basicMessageCount = 0;
    int m_basicMessageIndex = -1;
    int m_errorMessageIndex = -1;
    CCMenuItemSpriteExtra* m_submitButton = nullptr;
};