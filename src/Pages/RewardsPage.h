#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/GJRewardDelegate.h"

class CCMenuItemSpriteExtra;
class RewardUnlockLayer;

class RewardsPage : public FLAlertLayer, public FLAlertLayerProtocol, public GJRewardDelegate {
public:
    static RewardsPage* create();

    static const char* getRewardFrame(int type, int state);

    bool init();
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onFreeStuff(cocos2d::CCObject* sender);
    void onReward(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void show() override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;

    void rewardsStatusFailed() override;
    void rewardsStatusFinished(int type) override;

    void tryGetRewards();
    void unlockLayerClosed(RewardUnlockLayer* layer);
    void updateTimers(float dt);

    virtual ~RewardsPage();

protected:
    cocos2d::CCLabelBMFont* m_leftLabel = nullptr;
    cocos2d::CCLabelBMFont* m_rightLabel = nullptr;
    bool m_leftOpen = false;
    CCMenuItemSpriteExtra* m_leftChest = nullptr;
    bool m_rightOpen = false;
    CCMenuItemSpriteExtra* m_rightChest = nullptr;
    RewardUnlockLayer* m_openLayer = nullptr;
};