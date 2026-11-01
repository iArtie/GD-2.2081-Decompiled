#pragma once

#include "Layers/FLAlertLayer.h"
#include "Delegates/UploadActionDelegate.h"
#include "Delegates/UploadPopupDelegate.h"
#include "Platform/cocos2d.h"

class GJFriendRequest;
class LoadingCircle;
class UploadActionPopup;
class CCMenuItemSpriteExtra;

class FriendRequestPopup : public FLAlertLayer, 
                           public UploadActionDelegate, 
                           public UploadPopupDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void blockUser();
    static FriendRequestPopup* create(GJFriendRequest* request);
    bool init(GJFriendRequest* request) override;
    void keyBackClicked() override;
    void loadFromGJFriendRequest(GJFriendRequest* request);
    void onAccept(cocos2d::CCObject* sender);
    void onBlock(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup) override;
    void onRemove(cocos2d::CCObject* sender);
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;
    
    virtual ~FriendRequestPopup();

private:
    GJFriendRequest* m_request;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    LoadingCircle* m_circle;
    CCMenuItemSpriteExtra* m_closeBtn;
    UploadActionPopup* m_popup;
    bool m_unk;
    int m_joystickConnected;
};