#include "CCPartAnimSprite.h"
#include "CCAnimateFrameCache.h"
#include "CCSpritePart.h"
#include "SpriteDescription.h"
#include "cocos2d.h"

void CCPartAnimSprite::changeTextureOfID(const char* key, const char* frame) {
    CCSpritePart* part = static_cast<CCSpritePart*>(m_spritePartIDs->objectForKey(key));
    if (part) {
        if (frame) {
            part->updateDisplayFrame(frame);
        } else {
            part->resetTextureRect();
        }
    }
}

int CCPartAnimSprite::countParts() {
    return this->getChildren()->count();
}

void CCPartAnimSprite::dirtify() {
    cocos2d::CCArray* children = this->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCObject* obj = children->objectAtIndex(i);
            obj->setTag(1);
        }
    }
}

cocos2d::CCSpriteFrame* CCPartAnimSprite::displayFrame() {
    return m_spriteFrame;
}

CCSpritePart* CCPartAnimSprite::getSpriteForKey(const char* key) {
    return static_cast<CCSpritePart*>(m_spritePartIDs->objectForKey(key));
}

bool CCPartAnimSprite::isFrameDisplayed(cocos2d::CCSpriteFrame* frame) {
    return frame->isEqual(m_spriteFrame);
}

void CCPartAnimSprite::setBlendFunc(cocos2d::ccBlendFunc blendFunc) {
    m_sBlendFunc = blendFunc;
    cocos2d::CCArray* children = this->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCObject* obj = children->objectAtIndex(i);
            obj->setBlendFunc(blendFunc);
        }
    }
}

void CCPartAnimSprite::setDisplayFrame(cocos2d::CCSpriteFrame* frame) {
    if (!frame || frame == m_spriteFrame) {
        return;
    }

    if (m_spriteFrame) {
        m_spriteFrame->release();
    }
    m_spriteFrame = frame;
    m_spriteFrame->retain();

    CCAnimateFrameCache* cache = CCAnimateFrameCache::sharedSpriteFrameCache();
    std::string frameName = frame->m_strFrameName;
    cocos2d::CCArray* spriteDescriptions = cache->spriteFrameByName(frameName.c_str());

    if (!spriteDescriptions) {
        m_hasChanged = true;
        return;
    }

    // Mark all parts as not being used
    if (m_spriteParts && m_spriteParts->data->num > 0) {
        for (unsigned int i = 0; i < m_spriteParts->count(); i++) {
            CCSpritePart* part = static_cast<CCSpritePart*>(m_spriteParts->objectAtIndex(i));
            part->markAsNotBeingUsed();
        }
    }

    // Transform sprites for each description
    if (spriteDescriptions->data->num > 0) {
        for (int i = 0; i < spriteDescriptions->data->num; i++) {
            SpriteDescription* desc = static_cast<SpriteDescription*>(spriteDescriptions->data->arr[i]);
            transformSprite(desc);
        }
    }

    // Hide inactive parts
    if (m_spriteParts && m_spriteParts->data->num > 0) {
        for (unsigned int i = 0; i < m_spriteParts->count(); i++) {
            CCSpritePart* part = static_cast<CCSpritePart*>(m_spriteParts->objectAtIndex(i));
            part->hideInactive();
        }
    }

    m_hasChanged = true;
}

void CCPartAnimSprite::setFlipX(bool flipX) {
    bool currentFlipX = cocos2d::CCSprite::isFlipX();
    if (currentFlipX != flipX) {
        this->setScaleX(currentFlipX ? -1.0f : 1.0f);
    }
    cocos2d::CCSprite::setFlipX(flipX);
    m_hasChanged = true;
}

void CCPartAnimSprite::setFlipY(bool flipY) {
    bool currentFlipY = cocos2d::CCSprite::isFlipY();
    this->setScaleY(currentFlipY ? -1.0f : 1.0f);
    cocos2d::CCSprite::setFlipY(flipY);
    m_hasChanged = true;
}

void CCPartAnimSprite::setOpacity(unsigned __int8 opacity) {
    cocos2d::CCSprite::setOpacity(opacity);
    cocos2d::CCArray* children = this->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCObject* obj = children->objectAtIndex(i);
            obj->setOpacity(opacity);
        }
    }
}

void CCPartAnimSprite::setScale(float scale) {
    cocos2d::CCSprite::setScale(scale);
    m_hasChanged = true;
}

void CCPartAnimSprite::setScaleX(float scaleX) {
    cocos2d::CCSprite::setScaleX(scaleX);
    m_hasChanged = true;
}

void CCPartAnimSprite::setScaleY(float scaleY) {
    cocos2d::CCSprite::setScaleY(scaleY);
    m_hasChanged = true;
}

void CCPartAnimSprite::transformSprite(SpriteDescription* description) {
    CCSpritePart* part = static_cast<CCSpritePart*>(m_spriteParts->objectAtIndex(description->m_tag));
    part->stopAllActions();

    part->setPosition(description->m_position);

    float scaleX = description->m_scale.x;
    if (description->m_flipped.x != 0.0f) {
        scaleX = -scaleX;
    }
    part->setScaleX(scaleX);

    float scaleY = description->m_scale.y;
    if (description->m_flipped.y != 0.0f) {
        scaleY = -scaleY;
    }
    part->setScaleY(scaleY);

    part->setRotation(description->m_rotation);

    if (description->m_usesCustomTag) {
        part->setDisplayFrame(description->m_texture);
        part->frameChanged(description->m_texture->m_strFrameName);
    }

    if (part->getZOrder() != description->m_zValue) {
        this->reorderChild(part, description->m_zValue);
    }

    part->setBeingUsed(true);
    part->setVisible(true);
}

void CCPartAnimSprite::tweenSpriteTo(SpriteDescription* description, float duration) {
    CCSpritePart* part = static_cast<CCSpritePart*>(m_spriteParts->objectAtIndex(description->m_tag));
    part->stopAllActions();

    float scaleX = description->m_scale.x;
    if (description->m_flipped.x != 0.0f) {
        scaleX = -scaleX;
    }

    part->runAction(cocos2d::CCMoveTo::create(duration, description->m_position));
    part->runAction(cocos2d::CCScaleTo::create(duration, scaleX));
    part->runAction(cocos2d::CCRotateTo::create(duration, description->m_rotation));

    if (description->m_usesCustomTag) {
        part->setDisplayFrame(description->m_texture);
        part->frameChanged(description->m_texture->m_strFrameName);
    }

    if (part->getZOrder() != description->m_zValue) {
        this->reorderChild(part, description->m_zValue);
    }

    part->setBeingUsed(true);
    part->setVisible(true);
}

void CCPartAnimSprite::tweenToFrame(cocos2d::CCSpriteFrame* frame, float duration) {
    if (!m_spriteFrame) {
        setDisplayFrame(frame);
        return;
    }

    std::string frameName = frame->m_strFrameName;
    CCAnimateFrameCache* cache = CCAnimateFrameCache::sharedSpriteFrameCache();
    cocos2d::CCArray* spriteDescriptions = cache->spriteFrameByName(frameName.c_str());

    if (!spriteDescriptions) {
        m_hasChanged = true;
        return;
    }

    // Mark all parts as not being used
    if (m_spriteParts && m_spriteParts->data->num > 0) {
        for (unsigned int i = 0; i < m_spriteParts->count(); i++) {
            CCSpritePart* part = static_cast<CCSpritePart*>(m_spriteParts->objectAtIndex(i));
            part->markAsNotBeingUsed();
        }
    }

    // Tween sprites for each description
    if (spriteDescriptions->data->num > 0) {
        for (int i = 0; i < spriteDescriptions->data->num; i++) {
            SpriteDescription* desc = static_cast<SpriteDescription*>(spriteDescriptions->data->arr[i]);
            tweenSpriteTo(desc, duration);
        }
    }

    // Hide inactive parts
    if (m_spriteParts && m_spriteParts->data->num > 0) {
        for (unsigned int i = 0; i < m_spriteParts->count(); i++) {
            CCSpritePart* part = static_cast<CCSpritePart*>(m_spriteParts->objectAtIndex(i));
            part->hideInactive();
        }
    }

    m_hasChanged = true;
}

CCPartAnimSprite::~CCPartAnimSprite() {
    CC_SAFE_RELEASE_NULL(m_spriteFrame);
    CC_SAFE_RELEASE_NULL(m_spritePartIDs);
    CC_SAFE_RELEASE_NULL(m_spriteParts);
}