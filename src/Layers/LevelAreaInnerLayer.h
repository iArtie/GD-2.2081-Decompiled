#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/DialogLayer.h"
#include "Delegates/DialogDelegate.h"
#include "GameLevelManager.h"
#include "Managers/GameStatsManager.h"
#include "Managers/GameManager.h"
#include "Layers/LevelAreaLayer.h"
#include "Layers/PlayLayer.h"
#include "Layers/SecretLayer5.h"
#include "Layers/URLViewLayer.h"

class LevelAreaInnerLayer : public cocos2d::CCLayer, public DialogDelegate {
public:
    static LevelAreaInnerLayer* create(bool returning);
    static cocos2d::CCScene* scene(bool returning);

    virtual bool init(bool returning) override;
    virtual void keyBackClicked() override;
    virtual void onExit() override;
    virtual ~LevelAreaInnerLayer() override;

    void dialogClosed(DialogLayer* layer) override;
    void onBack(cocos2d::CCObject* sender);
    void onDoor(cocos2d::CCObject* sender);
    void onExit();
    void onInfo(cocos2d::CCObject* sender);
    void onNextFloor(cocos2d::CCObject* sender);
    void onOnlineVault(cocos2d::CCObject* sender);
    bool playStep1();
    void showFloor1CompleteDialog();
    void tryResumeTowerMusic();
    void tryShowAd();

private:
    CCMenuItemSpriteExtra* m_nextFloorButton = nullptr;
    bool m_enteringLevel = false;
    bool m_exiting = false;
    int m_levelID = 0;
};