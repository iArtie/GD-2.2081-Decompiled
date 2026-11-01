// GameObjectCopy.h
#pragma once

#include "Platform/cocos2d.h"
#include "Objects/GameObject.h"

class GameObjectCopy : public cocos2d::CCObject {
public:
    static GameObjectCopy* create(GameObject* object);
    bool init(GameObject* object);
    void resetObject();
    ~GameObjectCopy();

private:
    GameObject* m_object;
    cocos2d::CCPoint m_position;
    float m_rotationX;
    float m_rotationY;
    bool m_isFlipX;
    bool m_isFlipY;
    float m_customScaleX;
    float m_customScaleY;
};