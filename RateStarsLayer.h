#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "UploadActionDelegate.h"
#include "UploadPopupDelegate.h"

class RateLevelDelegate;
class UploadActionPopup;
class GJDifficultySprite;
class CCMenuItemSpriteExtra;

class RateStarsLayer : public FLAlertLayer, public UploadActionDelegate, public UploadPopupDelegate {
public:
    static RateStarsLayer* create(int levelID, bool platformer, bool moderator);

    bool init(int levelID, bool platformer, bool moderator);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onClosePopup(UploadActionPopup* popup) override;
    void onFeature(cocos2d::CCObject* sender);
    void onRate(cocos2d::CCObject* sender);
    void onToggleCoins(cocos2d::CCObject* sender);
    void selectRating(cocos2d::CCObject* sender);
    void uploadActionFailed(int id, int response) override;
    void uploadActionFinished(int id, int response) override;

    virtual ~RateStarsLayer();

protected:
    RateLevelDelegate* m_delegate = nullptr;
    int m_featureState = 0;
    GJDifficultySprite* m_difficultySprite = nullptr;
    int m_starsRate = -1;
    cocos2d::CCArray* m_buttons = nullptr;
    CCMenuItemSpriteExtra* m_submitButton = nullptr;
    UploadActionPopup* m_popup = nullptr;
    bool m_uploadFinished = false;
    int m_levelID = 0;
    bool m_moderator = false;
    cocos2d::CCSprite* m_coinSprite = nullptr;
    bool m_coinsToggled = false;
};