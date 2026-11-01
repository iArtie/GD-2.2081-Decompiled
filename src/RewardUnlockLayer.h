#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"

class CCMenuItemSpriteExtra;
class GJChestSprite;
class GJRewardItem;
class RewardsPage;
class CurrencyRewardLayer;

class RewardUnlockLayer : public FLAlertLayer {
public:
    static RewardUnlockLayer* create(int type, RewardsPage* page);

    bool init(int type, RewardsPage* page);
    void connectionTimeout();
    void currencyWillExit(CurrencyRewardLayer* layer);
    void keyBackClicked() override;
    void labelEnterFinishedO(cocos2d::CCObject* object);
    void onClose(cocos2d::CCObject* sender);
    void playDropSound();
    bool readyToCollect(GJRewardItem* item);
    void showCloseButton();
    bool showCollectReward(GJRewardItem* item);
    void step2();
    void step3();

    virtual ~RewardUnlockLayer();

protected:
    cocos2d::CCMenu* m_buttonMenu = nullptr;
    CCMenuItemSpriteExtra* m_rewardBtn = nullptr;
    bool m_rewardCollected = false;
    CCMenuItemSpriteExtra* m_closeBtn = nullptr;
    cocos2d::CCLabelBMFont* m_wrongLabel = nullptr;
    RewardsPage* m_rewardsPage = nullptr;
    int m_chestType = 0;
    GJChestSprite* m_chestSprite = nullptr;
    cocos2d::CCArray* m_backgroundObjects = nullptr;
    GJRewardItem* m_rewardItem = nullptr;
    bool m_animationPlayed = false;
};