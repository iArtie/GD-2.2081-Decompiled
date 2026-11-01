#include "Cocos/CCSpriteCOpacity.h"
#include "Platform/cocos2d.h"

CCSpriteCOpacity* CCSpriteCOpacity::createWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
    if (!frame) {
        return nullptr;
    }

    CCSpriteCOpacity* sprite = new CCSpriteCOpacity();
    if (sprite && sprite->initWithSpriteFrame(frame)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

CCSpriteCOpacity* CCSpriteCOpacity::createWithSpriteFrameName(const char* frameName) {
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName);
    return createWithSpriteFrame(frame);
}

void CCSpriteCOpacity::setOpacity(unsigned __int8 opacity) {
    cocos2d::CCSprite::setOpacity(opacity);
    
    cocos2d::CCArray* children = this->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCObject* obj = children->objectAtIndex(i);
            obj->setOpacity(opacity);
        }
    }
}

CCSpriteCOpacity::~CCSpriteCOpacity() {
    // Destructor
}