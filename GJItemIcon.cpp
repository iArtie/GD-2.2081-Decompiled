#include "GJItemIcon.h"

void GJItemIcon::changeToLockedState(float scale) {
    auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
    addChild(sprite, 10);
    sprite->setPosition(m_player->getPosition());
    sprite->setScale(scale * 0.8f);
    sprite->setOpacity(0x7F);
    sprite->setColor(cocos2d::ccc3(175, 175, 175));
}

GJItemIcon* GJItemIcon::createBrowserItem(UnlockType unlockType, int itemID) {
    cocos2d::ccColor3B white = cocos2d::ccc3(255, 255, 255);
    return GJItemIcon::create(unlockType, itemID, cocos2d::ccc3(175, 175, 175), cocos2d::ccc3(255, 255, 255), 0, 1, 1, white);
}

void GJItemIcon::darkenStoreItem(ShopType type) {
    auto color = GJItemIcon::unlockedColorForType(static_cast<int>(type));
    darkenStoreItem(color);
}

float GJItemIcon::scaleForType(UnlockType type) {
    static float scales[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int index = static_cast<unsigned int>(type) - 1;
    if (index > 14) {
        return 1.0f;
    }
    return scales[index];
}

void GJItemIcon::setOpacity(unsigned char opacity) {
    cocos2d::CCSprite::setOpacity(opacity);
    m_player->setOpacity(opacity);
    
    auto children = m_player->getChildren();
    if (children) {
        unsigned int count = children->count();
        for (unsigned int i = 0; i < count; ++i) {
            auto child = children->objectAtIndex(i);
            child->setOpacity(opacity);
        }
    }
}

void GJItemIcon::toggleEnabledState(bool enabled) {
    if (m_isIcon) {
        return;
    }
    
    if (enabled) {
        m_player->setColor(cocos2d::ccc3(255, 255, 255));
    } else {
        m_player->setColor(cocos2d::ccc3(100, 100, 100));
    }
    
    auto children = m_player->getChildren();
    if (children) {
        unsigned int count = children->count();
        for (unsigned int i = 0; i < count; ++i) {
            auto child = children->objectAtIndex(i);
            child->setVisible(true);
        }
    }
}

cocos2d::ccColor3B GJItemIcon::unlockedColorForType(int type) {
    unsigned char r, g, b;
    
    switch (type) {
        case 1:
            r = 10;
            g = 30;
            b = 20;
            break;
        case 2:
            r = 33;
            g = 22;
            b = 66;
            break;
        case 3:
            r = 20;
            g = 20;
            b = 40;
            break;
        case 4:
            r = 20;
            g = 30;
            b = 50;
            break;
        case 5:
            r = 50;
            g = 50;
            b = 50;
            break;
        default:
            r = 60;
            g = 30;
            b = 20;
            break;
    }
    
    return cocos2d::ccc3(r, g, b);
}

GJItemIcon::~GJItemIcon() {
}