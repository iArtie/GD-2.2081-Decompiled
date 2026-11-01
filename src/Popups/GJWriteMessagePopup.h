#pragma once

#include "Platform/cocos2d.h"
#include <string>

class CCTextInputNode;
class UploadActionPopup;
class FLAlertLayer;
class TextArea;

class GJWriteMessagePopup : public FLAlertLayer {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void closeMessagePopup(bool fullExit);
    static GJWriteMessagePopup* create(int accountID, int messageID);
    bool init(int accountID, int messageID) override;
    void keyBackClicked() override;
    void onClearBody(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup);
    void onSend(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    void textInputReturn(CCTextInputNode* node) override;
    void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    void updateBody(const std::string& body);
    void updateCharCountLabel(int type);
    void updateSubject(const std::string& subject);
    void updateText(const std::string& text, int type);
    void uploadMessageFailed(int accountID);
    void uploadMessageFinished(int accountID);
    virtual ~GJWriteMessagePopup();

private:
    cocos2d::CCLayer* m_mainLayer;
    CCTextInputNode* m_messageInput;
    CCTextInputNode* m_subjectInput;
    std::string m_subjectText;
    std::string m_messageText;
    int m_messageID;
    int m_accountID;
    UploadActionPopup* m_uploadPopup;
    bool m_uploadSuccess;
    cocos2d::CCLabelBMFont* m_messageCountLabel;
    cocos2d::CCLabelBMFont* m_subjectCountLabel;
    cocos2d::CCMenu* m_buttonMenu;
};