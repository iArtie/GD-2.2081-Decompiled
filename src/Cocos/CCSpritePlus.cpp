#include "Cocos/CCSpritePlus.h"
#include "Platform/cocos2d.h"

void CCSpritePlus::addFollower(cocos2d::CCNode* follower) {
    m_hasFollower = true;
    if (!m_followers) {
        m_followers = cocos2d::CCArray::create();
        m_followers->retain();
    }
    m_followers->addObject(follower);
}

CCSpritePlus* CCSpritePlus::createWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
    if (!frame) {
        return nullptr;
    }

    CCSpritePlus* sprite = new CCSpritePlus();
    if (sprite && sprite->initWithSpriteFrame(frame)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

CCSpritePlus* CCSpritePlus::createWithSpriteFrameName(const char* frame) {
    cocos2d::CCSpriteFrame* spriteFrame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame);
    return createWithSpriteFrame(spriteFrame);
}

void CCSpritePlus::followSprite() {
    m_followingSprite = this;
    this->addFollower(this);
}

CCSpritePlus* CCSpritePlus::getFollower() {
    if (m_hasFollower && m_followers && m_followers->count() > 0) {
        return static_cast<CCSpritePlus*>(m_followers->objectAtIndex(0));
    }
    return nullptr;
}

bool CCSpritePlus::initWithSpriteFrameName(const char* frameName) {
    m_eObjType = GameObjectObjectType;
    return cocos2d::CCSprite::initWithSpriteFrameName(frameName);
}

bool CCSpritePlus::initWithTexture(cocos2d::CCTexture2D* texture) {
    m_eObjType = GameObjectObjectType;
    return cocos2d::CCSprite::initWithTexture(texture);
}

void CCSpritePlus::removeFollower(cocos2d::CCNode* sprite) {
    if (m_followers) {
        m_followers->removeObject(sprite);
        if (m_followers->count() == 0) {
            m_hasFollower = false;
        }
    }
}

void CCSpritePlus::setFlipX(bool flipX) {
    cocos2d::CCSprite::setFlipX(flipX);

    if (m_propagateFlipChanges) {
        if (m_pChildren) {
            for (unsigned int i = 0; i < m_pChildren->count(); i++) {
                cocos2d::CCSprite* child = static_cast<cocos2d::CCSprite*>(m_pChildren->objectAtIndex(i));
                child->setFlipX(flipX);
            }
        }
    }

    if (m_hasFollower && m_followers) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCSprite* follower = static_cast<cocos2d::CCSprite*>(m_followers->objectAtIndex(i));
            follower->setFlipX(flipX);
        }
    }
}

void CCSpritePlus::setFlipY(bool flipY) {
    cocos2d::CCSprite::setFlipY(flipY);

    if (m_propagateFlipChanges) {
        if (m_pChildren) {
            for (unsigned int i = 0; i < m_pChildren->count(); i++) {
                cocos2d::CCSprite* child = static_cast<cocos2d::CCSprite*>(m_pChildren->objectAtIndex(i));
                child->setFlipY(flipY);
            }
        }
    }

    if (m_hasFollower && m_followers) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCSprite* follower = static_cast<cocos2d::CCSprite*>(m_followers->objectAtIndex(i));
            follower->setFlipY(flipY);
        }
    }
}

void CCSpritePlus::setPosition(const cocos2d::CCPoint* position) {
    cocos2d::CCSprite::setPosition(*position);

    if (m_hasFollower && m_followers) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setPosition(*position);
        }
    }
}

void CCSpritePlus::setRotation(float rotation) {
    cocos2d::CCSprite::setRotation(rotation);

    if (m_hasFollower && m_followers) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setRotation(rotation);
        }
    }
}

void CCSpritePlus::setRotationX(float rotationX) {
    cocos2d::CCSprite::setRotationX(rotationX);

    if (m_hasFollower && m_followers) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setRotationX(rotationX);
        }
    }
}

void CCSpritePlus::setRotationY(float rotationY) {
    cocos2d::CCSprite::setRotationY(rotationY);

    if (m_hasFollower && m_followers) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setRotationY(rotationY);
        }
    }
}

void CCSpritePlus::setScale(float scale) {
    cocos2d::CCSprite::setScale(scale);

    if (m_hasFollower && m_followers && m_propagateScaleChanges) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setScale(scale);
        }
    }
}

void CCSpritePlus::setScaleX(float scaleX) {
    cocos2d::CCSprite::setScaleX(scaleX);

    if (m_hasFollower && m_followers && m_propagateScaleChanges) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setScaleX(scaleX);
        }
    }
}

void CCSpritePlus::setScaleY(float scaleY) {
    cocos2d::CCSprite::setScaleY(scaleY);

    if (m_hasFollower && m_followers && m_propagateScaleChanges) {
        for (unsigned int i = 0; i < m_followers->count(); i++) {
            cocos2d::CCObject* obj = m_followers->objectAtIndex(i);
            obj->setScaleY(scaleY);
        }
    }
}

void CCSpritePlus::stopFollow() {
    if (m_followingSprite) {
        m_followingSprite->removeFollower(this);
    }
}

CCSpritePlus::~CCSpritePlus() {
    CC_SAFE_RELEASE_NULL(m_followers);
}