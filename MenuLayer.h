#pragma once

#include "cocos2d.h"

class MenuGameLayer;
class CCMenuItemSpriteExtra;
class FLAlertLayer;
class FLAlertLayerProtocol;

class MenuLayer : public cocos2d::CCLayer, public FLAlertLayerProtocol {
public:
    static cocos2d::CCScene* scene(bool isVideoOptionsOpen);

    bool init() override;
    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void endGame();
    void firstNetworkTest();
    void googlePlaySignedIn();
    void onAchievements(cocos2d::CCObject* sender);
    void onCreator(cocos2d::CCObject* sender);
    void onDaily(cocos2d::CCObject* sender);
    void onDiscord(cocos2d::CCObject* sender);
    void onEveryplay(cocos2d::CCObject* sender);
    void onFacebook(cocos2d::CCObject* sender);
    void onFreeLevels(cocos2d::CCObject* sender);
    void onFullVersion(cocos2d::CCObject* sender);
    void onGameCenter(cocos2d::CCObject* sender);
    void onGarage(cocos2d::CCObject* sender);
    void onGooglePlayGames(cocos2d::CCObject* sender);
    void onMoreGames(cocos2d::CCObject* sender);
    void onMyProfile(cocos2d::CCObject* sender);
    void onNewgrounds(cocos2d::CCObject* sender);
    void onOptions(cocos2d::CCObject* sender);
    void onOptionsInstant();
    void onPlay(cocos2d::CCObject* sender);
    void onQuit(cocos2d::CCObject* sender);
    void onRobTop(cocos2d::CCObject* sender);
    void onStats(cocos2d::CCObject* sender);
    void onTrailer(cocos2d::CCObject* sender);
    void onTwitch(cocos2d::CCObject* sender);
    void onTwitter(cocos2d::CCObject* sender);
    void onYouTube(cocos2d::CCObject* sender);
    void openOptions(bool videoOptions);
    void showGCQuestion();
    void showMeltdownPromo();
    void showTOS();
    void syncPlatformAchievements(float dt);
    void tryShowAd(float dt);
    void updateUserProfileButton();
    void videoOptionsClosed();
    void videoOptionsOpened();
    void willClose();

    virtual ~MenuLayer();

protected:
    bool m_showingTOS = false;
    cocos2d::CCSprite* m_gpSprite = nullptr;
    CCMenuItemSpriteExtra* m_profileButton = nullptr;
    cocos2d::CCLabelBMFont* m_profileLabel = nullptr;
    cocos2d::CCSprite* m_viewProfileSprite = nullptr;
    MenuGameLayer* m_menuGameLayer = nullptr;
};