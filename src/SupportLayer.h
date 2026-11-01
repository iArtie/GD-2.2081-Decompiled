#pragma once

#include "cocos2d.h"
#include "GJDropDownLayer.h"
#include "FLAlertLayerProtocol.h"
#include "UploadActionDelegate.h"
#include "UploadPopupDelegate.h"

class UploadActionPopup;

class SupportLayer : public GJDropDownLayer, public FLAlertLayerProtocol, public UploadActionDelegate, public UploadPopupDelegate {
public:
    static SupportLayer* create();
    void customSetup() override;
    void exitLayer() override;
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void onClosePopup(UploadActionPopup* popup) override;
    void onCocos2d(cocos2d::CCObject* sender);
    void onEmail(cocos2d::CCObject* sender);
    void onGetReward(cocos2d::CCObject* sender);
    void onLinks(cocos2d::CCObject* sender);
    void onLowDetail(cocos2d::CCObject* sender);
    void onPrivacy(cocos2d::CCObject* sender);
    void onRequestAccess(cocos2d::CCObject* sender);
    void onRobTop(cocos2d::CCObject* sender);
    void onSFX(cocos2d::CCObject* sender);
    void onTOS(cocos2d::CCObject* sender);
    void sendSupportMail();
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;
    virtual ~SupportLayer();

protected:
    UploadActionPopup* m_uploadPopup = nullptr;
};