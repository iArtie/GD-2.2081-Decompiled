#pragma once

#include "cocos2d.h"
#include <string>

class CCMenuItemSpriteExtra;

class SecretGame01Layer : public cocos2d::CCLayer {
public:
    static SecretGame01Layer* create();

    bool init() override;

    void didSelectCorrectObject(CCMenuItemSpriteExtra* button);
    void gameStep01();
    void gameStep02();
    int getCountForDifficulty(int difficulty);
    std::string getFrameForDifficulty(int difficulty);
    int getRowsForDifficulty(int difficulty);
    int getTimeForDifficulty(int difficulty);
    void onSelectButton(cocos2d::CCObject* sender);
    void resetGame();
    void resetGameTimer();
    void scaleOutGame(bool won);
    void showGameOver();
    void showGameWon();
    void update(float dt) override;

    virtual ~SecretGame01Layer();

protected:
    int m_gameDifficulty = 0;
    cocos2d::CCArray* m_gameObjects = nullptr;
    cocos2d::CCArray* m_requiredObjects = nullptr;
    cocos2d::CCMenu* m_mainMenu = nullptr;
    int m_selectedObject = 0;
    bool m_gameActive = false;
    float m_timeElapsed = 0.0f;
    float m_gameTime = 0.0f;
    cocos2d::CCSprite* m_timeBarSprite = nullptr;
};