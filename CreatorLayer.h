#pragma once

#include "cocos2d.h"
#include "DialogDelegate.h"

class CreatorLayer : public cocos2d::CCLayer, public DialogDelegate {
public:
    bool canPlayOnlineLevels();
    void checkQuestsStatus();
    static CreatorLayer* create();
    void dialogClosed(DialogLayer* layer) override;
    bool init();
    void keyBackClicked();
    void onAdventureMap(cocos2d::CCObject* sender);
    void onBack(cocos2d::CCObject* sender);
    void onChallenge(cocos2d::CCObject* sender);
    void onDailyLevel(cocos2d::CCObject* sender);
    void onEventLevel(cocos2d::CCObject* sender);
    void onFameLevels(cocos2d::CCObject* sender);
    void onFeaturedLevels(cocos2d::CCObject* sender);
    void onGauntlets(cocos2d::CCObject* sender);
    void onLeaderboards(cocos2d::CCObject* sender);
    void onMapPacks(cocos2d::CCObject* sender);
    void onMultiplayer(cocos2d::CCObject* sender);
    void onMyLevels(cocos2d::CCObject* sender);
    void onOnlineLevels(cocos2d::CCObject* sender);
    void onOnlyFullVersion(cocos2d::CCObject* sender);
    void onPaths(cocos2d::CCObject* sender);
    void onSavedLevels(cocos2d::CCObject* sender);
    void onSecretVault(cocos2d::CCObject* sender);
    void onTopLists(cocos2d::CCObject* sender);
    void onTreasureRoom(cocos2d::CCObject* sender);
    void onWeeklyLevel(cocos2d::CCObject* sender);
    static cocos2d::CCScene* scene();
    void sceneWillResume();
    virtual ~CreatorLayer();

protected:
    cocos2d::CCSprite* m_secretDoorSprite;
    int m_versusDialogIndex;
    int m_vaultDialogIndex;
    cocos2d::CCSprite* m_questsSprite;
};