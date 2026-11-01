#pragma once

#include "cocos2d.h"
#include "DialogDelegate.h"
#include "GJEnums.h"

class GJGameLevel;
class GameObject;
class CCMenuItemSpriteExtra;
class DialogLayer;

class LevelPage : public cocos2d::CCLayer, public DialogDelegate {
public:
    void addSecretCoin();
    void addSecretDoor();
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    static LevelPage* create(GJGameLevel* level);
    void dialogClosed(DialogLayer* layer) override;
    bool init(GJGameLevel* level) override;
    void onInfo(cocos2d::CCObject* sender);
    void onMoreGames(cocos2d::CCObject* sender);
    void onPlay(cocos2d::CCObject* sender);
    void onSecretDoor(cocos2d::CCObject* sender);
    void onTheTower(cocos2d::CCObject* sender);
    void playCoinEffect();
    void playStep2();
    void playStep3();
    void registerWithTouchDispatcher() override;
    void updateDynamicPage(GJGameLevel* level);
    ~LevelPage() override;

protected:
    GJGameLevel* m_level = nullptr;
    cocos2d::CCMenu* m_levelMenu = nullptr;
    cocos2d::CCSize m_levelDisplaySize;
    GameObject* m_coinObject = nullptr;
    cocos2d::CCSprite* m_secretDoor = nullptr;
    bool m_isBusy = false;
    cocos2d::extension::CCScale9Sprite* m_levelDisplay = nullptr;
    cocos2d::CCSprite* m_normalProgressBar = nullptr;
    float m_progressWidth = 0.0f;
    cocos2d::CCSprite* m_practiceProgressBar = nullptr;
    cocos2d::CCLabelBMFont* m_normalProgressLabel = nullptr;
    cocos2d::CCLabelBMFont* m_practiceProgressLabel = nullptr;
    cocos2d::CCLabelBMFont* m_nameLabel = nullptr;
    cocos2d::CCSprite* m_difficultySprite = nullptr;
    cocos2d::CCSprite* m_starsSprite = nullptr;
    cocos2d::CCLabelBMFont* m_starsLabel = nullptr;
    cocos2d::CCArray* m_progressObjects = nullptr;
    cocos2d::CCArray* m_levelObjects = nullptr;
    cocos2d::CCArray* m_coins = nullptr;
    cocos2d::CCArray* m_dynamicObjects = nullptr;
};