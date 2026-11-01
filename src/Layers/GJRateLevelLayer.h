#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJGameLevel;
class GJDifficultySprite;
class LoadingCircle;
class UploadActionPopup;
class FLAlertLayer;

class GJRateLevelLayer : public SetupTriggerPopup {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    static GJRateLevelLayer* create(GJGameLevel* level);
    bool init(GJGameLevel* level) override;
    void keyBackClicked() override;
    void onClosePopup(UploadActionPopup* popup);
    void onCustomButton(cocos2d::CCObject* sender) override;
    void onFeature(cocos2d::CCObject* sender);
    void onRate(cocos2d::CCObject* sender);
    void onToggleCoins(cocos2d::CCObject* sender);
    void rateInfoFailed(int id, int response);
    void rateInfoFinished(int id, const std::string& info);
    void selectRating(cocos2d::CCObject* sender);
    void setupRateInfo(const std::string& info);
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);
    virtual ~GJRateLevelLayer();

private:
    cocos2d::CCSprite* m_coinSprite;
    bool m_coinsToggled;
    GJFeatureState m_featureState;
    GJDifficultySprite* m_difficultySprite;
    int m_stars;
    cocos2d::CCArray* m_starButtons;
    cocos2d::CCObject* m_submitButton;
    UploadActionPopup* m_uploadPopup;
    bool m_uploadFinished;
    GJGameLevel* m_level;
    LoadingCircle* m_loadingCircle;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCPoint m_center;
};