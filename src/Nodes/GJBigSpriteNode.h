// GJBigSpriteNode.h
#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJBigSpriteNode : public cocos2d::CCNode {
public:
    static GJBigSpriteNode* create();
    virtual ~GJBigSpriteNode();

    cocos2d::CCSprite* m_sprite;
    std::string m_spriteName;
    cocos2d::CCRect m_spriteRect;
    bool m_spriteVisible;
};