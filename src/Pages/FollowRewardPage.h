#pragma once

#include "Layers/FLAlertLayer.h"
#include "Delegates/GameRateDelegate.h"
#include "Delegates/RewardedVideoDelegate.h"
#include "Platform/cocos2d.h"

class CCMenuItemSpriteExtra;
class GJChestSprite;

class FollowRewardPage : public FLAlertLayer, public GameRateDelegate, public RewardedVideoDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    static FollowRewardPage* create();
    bool init() override;
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onRewardedVideo(cocos2d::CCObject* sender);
    void onSpecialItem(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void rewardedVideoFinished() override;
    void show() override;
    void switchToOpenedState(CCMenuItemSpriteExtra* button);
    void updateRate();
    
    virtual ~FollowRewardPage();

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCNode* m_scene;
    cocos2d::CCArray* m_chests;
    int m_ZOrder;
    bool m_noElasticity;
    bool m_videoPlaying;
};