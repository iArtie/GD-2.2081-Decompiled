#pragma once

#include "cocos2d.h"
#include "TextInputDelegate.h"
#include "FLAlertLayerProtocol.h"
#include "DialogDelegate.h"
#include <string>

class CCTextInputNode;
class CCMenuItemSpriteExtra;
class FLAlertLayer;
class DialogLayer;

class SecretLayer2 : public cocos2d::CCLayer, public TextInputDelegate, public FLAlertLayerProtocol, public DialogDelegate {
public:
    static SecretLayer2* create();
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
    void onDoor(cocos2d::CCObject* sender);
    void onExit() override;
    void onSecretLevel(cocos2d::CCObject* sender);
    void onSubmit(cocos2d::CCObject* sender);
    void playCoinEffect();
    void selectAThread();
    void showCompletedLevel();
    void showSecretLevel();

    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;

    void updateMessageLabel(std::string text);
    void updateSearchLabel(const char* text);

    virtual ~SecretLayer2();

protected:
    CCMenuItemSpriteExtra* m_secretDoorButton = nullptr;
    CCTextInputNode* m_searchInput = nullptr;
    bool m_exiting = false;
    CCMenuItemSpriteExtra* m_secretLevelButton = nullptr;
    cocos2d::CCArray* m_secretLevelLabels = nullptr;
    cocos2d::CCLabelBMFont* m_messageLabel = nullptr;
    cocos2d::CCDictionary* m_messageThreads = nullptr;
    int m_threadTag = -1;
    int m_selectedThread = 0;
    int m_threadMessageIndex = 0;
    int m_basicMessageCount = 0;
    int m_basicMessageIndex = -1;
    int m_errorMessageIndex = -1;
    CCMenuItemSpriteExtra* m_submitButton = nullptr;
    int m_doorDialogIndex = -1;
};