#pragma once

#include "Platform/cocos2d.h"

class PlayerObject;
class GJGroundLayer;

class MenuGameLayer : public cocos2d::CCLayer {
public:
    static MenuGameLayer* create();

    bool init() override;
    void registerWithTouchDispatcher() override;
    void update(float dt) override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    void destroyPlayer();
    cocos2d::ccColor3B getBGColor(int index);
    void resetPlayer();
    void tryJump(float dt);
    void updateColor(float dt);
    void updateColors();

    virtual ~MenuGameLayer();

protected:
    bool m_isDestroyingPlayer = false;
    PlayerObject* m_playerObject = nullptr;
    GJGroundLayer* m_groundLayer = nullptr;
    int m_initCount = 0;
    cocos2d::CCSprite* m_backgroundSprite = nullptr;
    bool m_videoOptionsOpen = false;
    float m_deltaCount = 0.0f;
    cocos2d::CCPoint m_realBackgroundPosition;
    float m_backgroundWrapPosition = 0.0f;
};