#pragma once

#include "Platform/cocos2d.h"

enum class UnlockType {
    Unknown = 0,
    Type1 = 1,
    Type2 = 2,
    Type3 = 3,
    Type4 = 4,
    Type5 = 5
};

enum class ShopType {
    Unknown = 0
};

class GJItemIcon : public cocos2d::CCSprite {
public:
    static GJItemIcon* createBrowserItem(UnlockType unlockType, int itemID);
    static float scaleForType(UnlockType type);
    static cocos2d::ccColor3B unlockedColorForType(int type);
    
    void changeToLockedState(float scale);
    void darkenStoreItem(ShopType type);
    void setOpacity(unsigned char opacity) override;
    void toggleEnabledState(bool enabled);
    
    virtual ~GJItemIcon();

private:
    bool m_isIcon;
    cocos2d::CCSprite* m_player;
};