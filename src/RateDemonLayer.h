#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "UploadActionDelegate.h"
#include "UploadPopupDelegate.h"

class RateLevelDelegate;
class UploadActionPopup;
class CCMenuItemSpriteExtra;

class RateDemonLayer : public FLAlertLayer, public UploadActionDelegate, public UploadPopupDelegate {
public:
    static RateDemonLayer* create(int levelID);

    bool init(int levelID);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup) override;
    void onRate(cocos2d::CCObject* sender);
    void selectRating(cocos2d::CCObject* sender);
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;

    virtual ~RateDemonLayer();

protected:
    RateLevelDelegate* m_delegate = nullptr;
    int m_demonRate = -1;
    cocos2d::CCArray* m_demons = nullptr;
    CCMenuItemSpriteExtra* m_submitButton = nullptr;
    UploadActionPopup* m_popup = nullptr;
    bool m_uploadFinished = false;
    int m_levelID = 0;
    bool m_moderator = false;
};