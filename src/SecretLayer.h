#pragma once

#include "cocos2d.h"
#include "TextInputDelegate.h"
#include "FLAlertLayerProtocol.h"
#include <string>

class CCTextInputNode;
class CCMenuItemSpriteExtra;
class FLAlertLayer;

class SecretLayer : public cocos2d::CCLayer, public TextInputDelegate, public FLAlertLayerProtocol {
public:
    static SecretLayer* create();
    static cocos2d::CCScene* scene();

    bool init() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;

    std::string getBasicMessage();
    std::string getMessage();
    std::string getThreadMessage();

    void keyBackClicked() override;

    void onBack(cocos2d::CCObject* sender);
    void onSubmit(cocos2d::CCObject* sender);
    void playCoinEffect();
    void selectAThread();

    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;

    void updateMessageLabel(std::string text);
    void updateSearchLabel(const char* text);

    virtual ~SecretLayer();

protected:
    CCTextInputNode* m_searchInput = nullptr;
    cocos2d::CCLabelBMFont* m_messageLabel = nullptr;
    cocos2d::CCDictionary* m_messageThreads = nullptr;
    int m_threadTag = -1;
    int m_selectedThread = 0;
    int m_threadMessageIndex = 0;
    int m_basicMessageCount = 0;
    int m_basicMessageIndex = -1;
    int m_doomedIndex = 0;
    CCMenuItemSpriteExtra* m_submitButton = nullptr;
};