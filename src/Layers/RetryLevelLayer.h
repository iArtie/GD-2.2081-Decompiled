#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJDropDownLayer.h"
#include "Delegates/RewardedVideoDelegate.h"

class CCMenuItemSpriteExtra;
class DialogLayer;

class RetryLevelLayer : public GJDropDownLayer, public RewardedVideoDelegate {
public:
    static RetryLevelLayer* create();

    void customSetup() override;
    void enterAnimFinished() override;
    const char* getEndText() override;

    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    void keyUp(cocos2d::enumKeyCodes key, double timestamp) override;

    void onEveryplay(cocos2d::CCObject* sender);
    void onFullVersion(cocos2d::CCObject* sender);
    void onMenu(cocos2d::CCObject* sender);
    void onReplay(cocos2d::CCObject* sender);
    void onRewardedVideo(cocos2d::CCObject* sender);
    void rewardedVideoFinished() override;

    void setupLastProgress();
    bool shouldOffsetRewardCurrency();
    void showLayer(bool instant);

    virtual ~RetryLevelLayer();

protected:
    bool m_exitingMenu = false;
    bool m_unk1e9 = false;
};