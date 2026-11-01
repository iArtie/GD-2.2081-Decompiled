#pragma once

#include "cocos2d.h"

class GJMapObject : public cocos2d::CCNode {
public:
    static GJMapObject* create(cocos2d::CCNode* parent);
    static GJMapObject* createMonster(int objectID);
    bool init(cocos2d::CCNode* parent);
    void monsterIdle();
    void monsterJump();
    void moveFinished();
    void moveMonster();
    void playerJump();
    void setObjectOrigin(cocos2d::CCPoint position, float offset);
    void startMonsterJumpLoop();
    void startPlayerJumpLoop();
    void touchMonster();
    virtual ~GJMapObject();

private:
    cocos2d::CCNode* m_mainNode;
    cocos2d::CCSprite* m_shadowSprite;
    bool m_monsterMoving;
    float m_moveStopTime;
    bool m_monsterIdle;
    float m_objectOffset;
    cocos2d::CCPoint m_objectOrigin;
    float m_jumpStopTime;
    int m_monstersTouched;
};