#pragma once

#include "cocos2d.h"

class GJUserMessage;
class LoadingCircle;
class UploadActionPopup;
class CCMenuItemSpriteExtra;
class FLAlertLayer;

class GJMessagePopup : public FLAlertLayer {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void blockUser();
    static GJMessagePopup* create(GJUserMessage* message);
    void downloadMessageFailed(int id);
    void downloadMessageFinished(GJUserMessage* message);
    bool init(GJUserMessage* message);
    void keyBackClicked() override;
    void loadFromGJMessage(GJUserMessage* message);
    void onBlock(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup);
    void onRemove(cocos2d::CCObject* sender);
    void onReply(cocos2d::CCObject* sender);
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);
    virtual ~GJMessagePopup();

private:
    GJUserMessage* m_message;
    LoadingCircle* m_loadingCircle;
    cocos2d::CCLabelBMFont* m_errorLabel;
    UploadActionPopup* m_uploadPopup;
    bool m_actionSuccess;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    CCMenuItemSpriteExtra* m_closeButton;
};