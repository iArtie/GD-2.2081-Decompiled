#pragma once

#include "Platform/cocos2d.h"
#include <string>

class SpriteAnimationManager;

enum ShopType {
    Normal = 0,
    Secret = 1,
    Community = 2,
    Mechanic = 3,
    Diamond = 4
};

class AnimatedShopKeeper {
public:
    void animationFinished(const char* key);
    static AnimatedShopKeeper* create(ShopType type);
    bool init(ShopType type);
    void playReactAnimation();
    void startAnimating();
    ~AnimatedShopKeeper();

private:
    int m_idleInt1;
    int m_idleInt2;
    bool m_reacting;
    ShopType m_type;
    bool m_looking;
    SpriteAnimationManager* m_animationManager;
    int m_gruntIndex;
    int m_reactCount;
};