#include "Cocos/CCAnimatedSprite.h"
#include "Delegates/AnimatedSpriteDelegate.h"
#include "Cocos/CCPartAnimSprite.h"
#include "Managers/SpriteAnimationManager.h"
#include "Platform/cocos2d.h"

void CCAnimatedSprite::animationFinished(const char* key) {
    if (m_delegate) {
        m_delegate->animationFinished(key);
    }
}

void CCAnimatedSprite::animationFinishedO(cocos2d::CCObject* str) {
    CCString* string = static_cast<CCString*>(str);
    animationFinished(string->getCString());
}

void CCAnimatedSprite::cleanupSprite() {
    if (m_paSprite) {
        m_paSprite->cleanup();
    }
    
    if (m_fbfSprite) {
        m_fbfSprite->cleanup();
        if (!m_animationManager) {
            goto cleanup_fbf;
        }
    } else {
        if (!m_animationManager) {
            goto cleanup_pa;
        }
    }
    
    if (m_animationManager) {
        m_animationManager->doCleanup();
    }

cleanup_fbf:
    if (m_fbfSprite) {
        m_fbfSprite->removeFromParentAndCleanup(true);
    }

cleanup_pa:
    if (m_paSprite) {
        m_paSprite->removeFromParentAndCleanup(true);
    }
    
    m_spriteMode = spriteMode::FrameByFrame;
    m_animationManager = nullptr;
    m_fbfSprite = nullptr;
    m_paSprite = nullptr;
    m_sprite = nullptr;
}

void CCAnimatedSprite::runAnimation(std::string animation) {
    m_animationManager->runAnimation(animation);
}

void CCAnimatedSprite::runAnimationForced(std::string animation) {
    m_animationManager->overridePrio();
    m_animationManager->runAnimation(animation);
}

void CCAnimatedSprite::setColor(const cocos2d::ccColor3B* color) {
    if (m_fbfSprite) {
        m_fbfSprite->setColor(*color);
    }
    if (m_paSprite) {
        m_paSprite->setColor(*color);
    }
    cocos2d::CCSprite::setColor(*color);
}

void CCAnimatedSprite::setOpacity(unsigned __int8 opacity) {
    if (m_fbfSprite) {
        m_fbfSprite->setOpacity(opacity);
    }
    if (m_paSprite) {
        m_paSprite->setOpacity(opacity);
    }
    cocos2d::CCSprite::setOpacity(opacity);
}

void CCAnimatedSprite::stopTween() {
    if (m_paSprite) {
        m_paSprite->stopAllActions();
    }
    this->stopActionByTag(1);
    m_activeTween.clear();
}

void CCAnimatedSprite::switchToMode(spriteMode mode) {
    if (m_spriteMode == mode) {
        return;
    }
    
    if (m_sprite) {
        m_sprite->stopAllActions();
        m_sprite->setVisible(false);
    }
    
    if (mode == spriteMode::FrameByFrame) {
        if (m_fbfSprite) {
            m_fbfSprite->setColor(m_sprite->getColor());
            m_fbfSprite->setFlipX(m_sprite->isFlipX());
            m_sprite = static_cast<CCPartAnimSprite*>(m_fbfSprite);
        } else {
            m_sprite = static_cast<CCPartAnimSprite*>(this);
        }
    } else {
        if (m_paSprite) {
            m_paSprite->setColor(m_sprite->getColor());
            m_paSprite->setFlipX(m_sprite->isFlipX());
            m_sprite = m_paSprite;
        }
    }
    
    if (m_sprite) {
        m_sprite->stopAllActions();
        m_sprite->setVisible(true);
    }
    
    m_spriteMode = mode;
}

void CCAnimatedSprite::tweenToAnimation(std::string animation, float duration) {
    spriteMode animType = m_animationManager->getAnimType(animation);
    
    if (animType != spriteMode::PartAnim) {
        return;
    }
    
    if (m_activeTween == animation && this->getActionByTag(1)) {
        return;
    }
    
    m_animationManager->stopAnimations();
    this->stopActionByTag(1);
    m_activeTween = animation;
    
    cocos2d::CCDictionary* frameDict = m_animationManager->m_frameDict;
    if (!frameDict) {
        return;
    }
    
    cocos2d::CCSpriteFrame* frame = static_cast<cocos2d::CCSpriteFrame*>(frameDict->objectForKey(animation));
    if (!frame) {
        return;
    }
    
    if (frame->m_frameCount > 0) {
        m_paSprite->tweenToFrame(frame, duration);
    } else {
        CCString* firstKey = CCString::createWithFormat("%s_first", animation.c_str());
        cocos2d::CCSpriteFrame* firstFrame = static_cast<cocos2d::CCSpriteFrame*>(frameDict->objectForKey(firstKey->getCString()));
        m_paSprite->tweenToFrame(firstFrame, duration);
    }
    
    m_activeTween = animation;
    
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(duration),
        cocos2d::CCCallFunc::create(this, callfunc_selector(CCAnimatedSprite::tweenToAnimationFinished)),
        nullptr
    );
    sequence->setTag(1);
    this->runAction(sequence);
}

void CCAnimatedSprite::tweenToAnimationFinished() {
    runAnimation(m_activeTween);
    m_activeTween.clear();
}

void CCAnimatedSprite::willPlayAnimation() {
    this->stopActionByTag(1);
    m_activeTween.clear();
}

CCAnimatedSprite::~CCAnimatedSprite() {
    // Destructor
}