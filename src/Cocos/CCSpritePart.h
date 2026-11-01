#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCSpritePlus.h"
#include <string>

class SpritePartDelegate;

class CCSpritePart : public CCSpritePlus {
public:
    static CCSpritePart* create(cocos2d::CCTexture2D* texture);
    static CCSpritePart* createWithSpriteFrameName(const char* frame);

    void frameChanged(std::string frame);
    bool getBeingUsed();
    void hideInactive();
    void markAsNotBeingUsed();
    void resetTextureRect();
    void setBeingUsed(bool beingUsed);
    virtual void setVisible(bool visible) override;
    void updateDisplayFrame(std::string frame);

    void setDelegate(SpritePartDelegate* delegate) { m_delegate = delegate; }

    ~CCSpritePart();

private:
    bool m_isBeingUsed;
    std::string m_spriteFrameName;
    SpritePartDelegate* m_delegate;
};