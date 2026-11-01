// EndLevelLayer.h
#pragma once

#include "GJDropDownLayer.h"
#include "cocos2d.h"
#include <string>

class PlayLayer;
class GJGameLevel;

class EndLevelLayer : public GJDropDownLayer {
public:
    static EndLevelLayer* create(PlayLayer* playLayer);
    bool init(PlayLayer* playLayer);
    virtual ~EndLevelLayer();

    void customSetup();
    void showLayer(bool instant);
    void enterAnimFinished();
    void onMenu(cocos2d::CCObject* sender);
    void onReplay(cocos2d::CCObject* sender);
    void onEdit(cocos2d::CCObject* sender);
    void onRestartCheckpoint(cocos2d::CCObject* sender);
    void onHideLayer(cocos2d::CCObject* sender);
    void onLevelLeaderboard(cocos2d::CCObject* sender);
    void onEveryplay(cocos2d::CCObject* sender);

    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    void keyUp(cocos2d::enumKeyCodes key, double timestamp) override;

    void coinEnterFinished(cocos2d::CCPoint position);
    void coinEnterFinishedO(cocos2d::CCObject* sender);
    void currencyEnterFinished();
    void diamondEnterFinished();
    void starEnterFinished();
    void playEndEffect();
    void playCoinEffect(float duration);
    void playStarEffect(float duration);
    void playCurrencyEffect(float duration);
    void playDiamondEffect(float duration);
    void goEdit();
    void tryShowBanner(float dt);

    std::string getCoinString();
    const char* getEndText();

private:
    PlayLayer* m_playLayer;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_sideMenu;
    cocos2d::CCArray* m_coinsToAnimate;
    bool m_hidden;
    bool m_exiting;
    bool m_notLocal;
    bool m_coinsVerified;
    bool m_animateCoins;
    bool m_endEffectPlayed;
    bool m_secretKey;

    cocos2d::CCPoint m_startPosition;
    cocos2d::CCPoint m_endPosition;
    cocos2d::CCPoint m_starsPosition;
    cocos2d::CCPoint m_orbsPosition;
    cocos2d::CCPoint m_diamondsPosition;

    int m_stars;
    int m_moons;
    int m_orbs;
    int m_diamonds;
};