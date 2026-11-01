#include "Cocos/CCSpritePart.h"
#include "Delegates/SpritePartDelegate.h"
#include "Platform/cocos2d.h"

CCSpritePart* CCSpritePart::create(cocos2d::CCTexture2D* texture) {
    CCSpritePart* part = new CCSpritePart();
    if (part && part->initWithTexture(texture)) {
        part->autorelease();
        return part;
    }
    delete part;
    return nullptr;
}

CCSpritePart* CCSpritePart::createWithSpriteFrameName(const char* frame) {
    CCSpritePart* part = new CCSpritePart();
    if (part && part->initWithSpriteFrameName(frame)) {
        part->m_spriteFrameName = frame;
        part->autorelease();
        return part;
    }
    delete part;
    return nullptr;
}

void CCSpritePart::frameChanged(std::string frame) {
    if (m_delegate && m_spriteFrameName != frame) {
        m_delegate->displayFrameChanged(this, frame);
    }
    m_spriteFrameName = frame;
}

bool CCSpritePart::getBeingUsed() {
    return m_isBeingUsed;
}

void CCSpritePart::hideInactive() {
    if (!m_isBeingUsed) {
        this->setVisible(false);
    }
}

void CCSpritePart::markAsNotBeingUsed() {
    m_isBeingUsed = false;
}

void CCSpritePart::resetTextureRect() {
    this->setTextureRect(cocos2d::CCRect(0, 0, 0, 0));
}

void CCSpritePart::setBeingUsed(bool beingUsed) {
    m_isBeingUsed = beingUsed;
}

void CCSpritePart::setVisible(bool visible) {
    if (!m_isBeingUsed) {
        visible = false;
    }
    cocos2d::CCSprite::setVisible(visible);
}

void CCSpritePart::updateDisplayFrame(std::string frame) {
    frameChanged(frame);
    cocos2d::CCSpriteFrame* spriteFrame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame.c_str());
    this->setDisplayFrame(spriteFrame);
}

CCSpritePart::~CCSpritePart() {
    // Destructor
}