// GJChestSprite.h
#pragma once

#include "cocos2d.h"

enum class ChestSpriteState {
    Closed = 0,
    Opening = 1,
    Opened = 2
};

class GJChestSprite : public cocos2d::CCSprite {
public:
    static GJChestSprite* create(int chestType);
    bool init(int chestType);
    void setColor(const cocos2d::ccColor3B& color);
    void setOpacity(unsigned char opacity);
    void switchToState(ChestSpriteState state, bool noGlow);
    virtual ~GJChestSprite();

private:
    ChestSpriteState m_spriteState;
    int m_chestType;
    bool m_dark;
};