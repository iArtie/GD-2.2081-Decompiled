#pragma once

#include "cocos2d.h"
#include "BoomScrollLayerDelegate.h"
#include "LevelManagerDelegate.h"
#include "RewardedVideoDelegate.h"

class BoomScrollLayer;
class LoadingCircle;
class TextArea;
class CCMenuItemSpriteExtra;
class GauntletNode;

class GauntletSelectLayer : public cocos2d::CCLayer,
                            public BoomScrollLayerDelegate,
                            public LevelManagerDelegate,
                            public RewardedVideoDelegate {
public:
    static GauntletSelectLayer* create(int unused);
    bool init(int unused);
    void keyBackClicked();
    void onBack(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void onPlay(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onRefresh(cocos2d::CCObject* sender);
    void onExit();
    void setupGauntlets();
    void goToPage(int page, bool instant);
    void updateArrows();
    void unblockPlay();

    // LevelManagerDelegate
    void loadLevelsFailed(const char* key, int type) override;
    void loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int type) override;

    // RewardedVideoDelegate
    void rewardedVideoFinished() override;

    void playUnlockAnimation();
    void unlockAnimationStep2();
    void unlockAnimationStep3();
    void unlockAnimationFinished();
    void showUnlockGauntlet();

    static cocos2d::CCScene* scene(int unused);

    virtual ~GauntletSelectLayer();

private:
    BoomScrollLayer* m_scrollLayer;
    LoadingCircle* m_loadingCircle;
    TextArea* m_tryAgainText;
    CCMenuItemSpriteExtra* m_refreshButton;
    CCMenuItemSpriteExtra* m_leftButton;
    CCMenuItemSpriteExtra* m_rightButton;
    cocos2d::CCSprite* m_backgroundSprite;
    cocos2d::CCDictionary* m_gauntlets;
    int m_gauntletID;
    bool m_exiting;
    bool m_playing;
    bool m_playBlocked;
    bool m_videoPlaying;
    bool m_locked;
};