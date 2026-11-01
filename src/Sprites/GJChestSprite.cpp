// GJChestSprite.cpp
#include "Sprites/GJChestSprite.h"
#include "Pages/RewardsPage.h"
#include <cstdlib>

GJChestSprite* GJChestSprite::create(int chestType) {
    GJChestSprite* instance = new GJChestSprite();
    if (instance && instance->init(chestType)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

GJChestSprite::GJChestSprite() : m_spriteState(ChestSpriteState::Closed), 
                                 m_chestType(0), m_dark(false) {}

bool GJChestSprite::init(int chestType) {
    if (!cocos2d::CCSprite::init()) {
        return false;
    }
    
    m_chestType = chestType;
    setContentSize(cocos2d::CCSize(120.0f, 50.0f));
    switchToState(ChestSpriteState::Closed, false);
    return true;
}

void GJChestSprite::setColor(const cocos2d::ccColor3B& color) {
    cocos2d::ccColor3B adjustedColor = color;
    
    if (m_dark) {
        adjustedColor.r = static_cast<unsigned int>(adjustedColor.r * 0.4f);
        adjustedColor.g = static_cast<unsigned int>(adjustedColor.g * 0.4f);
        adjustedColor.b = static_cast<unsigned int>(adjustedColor.b * 0.4f);
    }
    
    cocos2d::CCSprite::setColor(adjustedColor);
    
    // Apply color to first child if exists
    if (getChildren() && getChildren()->count() > 0) {
        cocos2d::CCObject* child = getChildren()->objectAtIndex(0);
        if (child) {
            child->setColor(adjustedColor);
        }
    }
}

void GJChestSprite::setOpacity(unsigned char opacity) {
    cocos2d::CCSprite::setOpacity(opacity);
    
    // Apply opacity to first child if exists
    if (getChildren() && getChildren()->count() > 0) {
        cocos2d::CCObject* child = getChildren()->objectAtIndex(0);
        if (child) {
            child->setOpacity(opacity);
        }
    }
}

void GJChestSprite::switchToState(ChestSpriteState state, bool noGlow) {
    if (m_spriteState == state) {
        return;
    }
    
    m_spriteState = state;
    removeAllChildren();
    
    float scale = (m_chestType == 1) ? 0.8f : 1.0f;
    
    if (state == ChestSpriteState::Opening) {
        // Opening state
        std::string frameName = RewardsPage::getRewardFrame(m_chestType, state);
        cocos2d::CCSprite* mainSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        addChild(mainSprite, 3);
        
        // Back sprite
        std::string backFrame = frameName + "_back_001";
        cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName(backFrame);
        addChild(backSprite, 0);
        
        if (!noGlow) {
            // Glow sprite
            std::string glowFrame = frameName + "_glow_001";
            cocos2d::CCSprite* glowSprite = cocos2d::CCSprite::createWithSpriteFrameName(glowFrame);
            addChild(glowSprite, 1);
            
            // Inner glow
            cocos2d::CCSprite* innerGlow = cocos2d::CCSprite::createWithSpriteFrameName(glowFrame);
            glowSprite->addChild(innerGlow, 1);
            
            cocos2d::CCPoint center = cocos2d::CCPoint(glowSprite->getContentSize().width * 0.5f,
                                                        glowSprite->getContentSize().height * 0.5f);
            innerGlow->setPosition(center);
            innerGlow->setScaleX(0.8f);
            
            glowSprite->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
            innerGlow->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
            
            // Opening particles
            cocos2d::CCParticleSystem* particles = cocos2d::CCParticleSystemQuad::create("chestOpen.plist");
            particles->setAutoRemoveOnFinish(true);
            addChild(particles, 4);
            
            // Set particle colors based on chest type
            cocos2d::ccColor4F startColor;
            cocos2d::ccColor4F endColor;
            
            switch (m_chestType) {
                case 1:
                    startColor = cocos2d::ccc4f(1.0f, 1.0f, 0.19608f, 1.0f);
                    endColor = cocos2d::ccc4f(1.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 3:
                    startColor = cocos2d::ccc4f(0.0f, 1.0f, 1.0f, 1.0f);
                    endColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 4:
                    startColor = cocos2d::ccc4f(0.58824f, 1.0f, 0.0f, 1.0f);
                    endColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                default:
                    startColor = cocos2d::ccc4f(1.0f, 1.0f, 1.0f, 1.0f);
                    endColor = cocos2d::ccc4f(0.0f, 1.0f, 0.0f, 1.0f);
                    break;
            }
            
            particles->setStartColor(startColor);
            particles->setEndColor(endColor);
            particles->setScale(scale);
            particles->setPosition(cocos2d::CCPointZero);
            particles->resetSystem();
        }
    } else if (state == ChestSpriteState::Opened) {
        // Opened state
        std::string frameName = RewardsPage::getRewardFrame(m_chestType, 4);
        cocos2d::CCSprite* mainSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        addChild(mainSprite, 4);
        
        // Back sprite
        std::string backFrame = frameName + "_back_001";
        cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName(backFrame);
        addChild(backSprite, 0);
        
        if (!noGlow) {
            // Glow sprite with animation
            std::string glowFrame = frameName + "_glow_001";
            cocos2d::CCSprite* glowSprite = cocos2d::CCSprite::createWithSpriteFrameName(glowFrame);
            addChild(glowSprite, 1);
            glowSprite->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
            
            // Fade animation for glow
            cocos2d::CCAction* fadeOut = cocos2d::CCEaseInOut::create(
                cocos2d::CCFadeTo::create(1.0f, 175), 2.0f);
            cocos2d::CCAction* fadeIn = cocos2d::CCEaseInOut::create(
                cocos2d::CCFadeTo::create(1.0f, 255), 2.0f);
            cocos2d::CCAction* sequence = cocos2d::CCSequence::create(fadeOut, fadeIn, nullptr);
            glowSprite->runAction(cocos2d::CCRepeatForever::create(sequence));
            
            // Shine burst sprites
            cocos2d::CCSprite* shine1 = cocos2d::CCSprite::createWithSpriteFrameName("shineBurst_001.png");
            addChild(shine1, 1);
            shine1->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
            shine1->setPosition(cocos2d::CCPoint(0.0f, -5.0f));
            shine1->setScale(scale * 4.0f);
            
            // Set shine color based on chest type
            cocos2d::ccColor3B shineColor1;
            cocos2d::ccColor3B shineColor2;
            cocos2d::ccColor3B glowColor;
            
            switch (m_chestType) {
                case 1:
                    shineColor1 = cocos2d::ccc3(255, 200, 100);
                    shineColor2 = cocos2d::ccc3(0, 100, 255);
                    glowColor = cocos2d::ccc3(100, 255, 150);
                    break;
                case 2:
                case 5:
                    shineColor1 = cocos2d::ccc3(255, 200, 100);
                    shineColor2 = cocos2d::ccc3(100, 255, 100);
                    glowColor = cocos2d::ccc3(0, 255, 255);
                    break;
                case 4:
                    shineColor1 = cocos2d::ccc3(255, 255, 200);
                    shineColor2 = cocos2d::ccc3(255, 200, 50);
                    glowColor = cocos2d::ccc3(0, 255, 150);
                    break;
                case 6:
                    shineColor1 = cocos2d::ccc3(255, 200, 100);
                    shineColor2 = cocos2d::ccc3(100, 255, 100);
                    glowColor = cocos2d::ccc3(0, 255, 255);
                    break;
                case 7:
                    shineColor1 = cocos2d::ccc3(255, 255, 200);
                    shineColor2 = cocos2d::ccc3(255, 200, 50);
                    glowColor = cocos2d::ccc3(0, 255, 150);
                    break;
                case 8:
                    shineColor1 = cocos2d::ccc3(255, 0, 255);
                    shineColor2 = cocos2d::ccc3(0, 58, 27);
                    glowColor = cocos2d::ccc3(255, 75, 255);
                    break;
                case 9:
                    shineColor1 = cocos2d::ccc3(255, 39, 255);
                    shineColor2 = cocos2d::ccc3(255, 200, 29);
                    glowColor = cocos2d::ccc3(255, 44, 255);
                    break;
                default:
                    shineColor1 = cocos2d::ccc3(255, 255, 255);
                    shineColor2 = cocos2d::ccc3(255, 255, 255);
                    glowColor = cocos2d::ccc3(255, 255, 255);
                    break;
            }
            
            shine1->setColor(shineColor1);
            
            // Rotation animation
            float rotationDirection = (static_cast<float>(rand()) / RAND_MAX > 0.5f) ? 1.0f : -1.0f;
            float rotationAngle = (static_cast<float>(rand()) / RAND_MAX * 0.5f + 0.5f) * 180.0f;
            cocos2d::CCAction* rotate = cocos2d::CCRepeatForever::create(
                cocos2d::CCRotateBy::create(1.0f, rotationDirection * rotationAngle));
            shine1->runAction(rotate);
            
            // Second shine burst
            cocos2d::CCSprite* shine2 = cocos2d::CCSprite::createWithSpriteFrameName("shineBurst_001.png");
            addChild(shine2, 1);
            shine2->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
            shine2->setPosition(cocos2d::CCPoint(0.0f, -5.0f));
            shine2->setScale(scale * 3.5f);
            shine2->setColor(shineColor2);
            
            float rotationAngle2 = (static_cast<float>(rand()) / RAND_MAX * 0.5f + 0.5f) * 180.0f;
            cocos2d::CCAction* rotate2 = cocos2d::CCRepeatForever::create(
                cocos2d::CCRotateBy::create(1.0f, -rotationDirection * rotationAngle2));
            shine2->runAction(rotate2);
            
            // Opened particles
            cocos2d::CCParticleSystem* particles = cocos2d::CCParticleSystemQuad::create("chestOpened.plist");
            particles->setAutoRemoveOnFinish(true);
            addChild(particles, 1);
            particles->setPosition(cocos2d::CCPointZero);
            
            // Particle colors based on chest type
            cocos2d::ccColor4F particleStartColor;
            cocos2d::ccColor4F particleEndColor;
            
            switch (m_chestType) {
                case 1:
                    particleStartColor = cocos2d::ccc4f(1.0f, 1.0f, 0.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 2:
                case 5:
                    particleStartColor = cocos2d::ccc4f(0.0f, 1.0f, 0.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 4:
                    particleStartColor = cocos2d::ccc4f(0.0f, 0.0f, 1.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 6:
                    particleStartColor = cocos2d::ccc4f(0.0f, 1.0f, 0.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 7:
                    particleStartColor = cocos2d::ccc4f(0.0f, 0.0f, 1.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 8:
                    particleStartColor = cocos2d::ccc4f(1.0f, 0.0f, 1.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case 9:
                    particleStartColor = cocos2d::ccc4f(1.0f, 0.0f, 1.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                default:
                    particleStartColor = cocos2d::ccc4f(1.0f, 1.0f, 1.0f, 1.0f);
                    particleEndColor = cocos2d::ccc4f(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
            }
            
            particles->setStartColor(particleStartColor);
            particles->setEndColor(particleEndColor);
            particles->setScale(scale);
            particles->resetSystem();
            
            // Glow backgrounds
            cocos2d::CCSprite* glowBg1 = cocos2d::CCSprite::createWithSpriteFrameName("chest_glow_bg_001.png");
            addChild(glowBg1, 2);
            glowBg1->setScale(scale);
            glowBg1->setColor(glowColor);
            
            cocos2d::CCSprite* glowBg2 = cocos2d::CCSprite::createWithSpriteFrameName("chest_glow_bg_001.png");
            glowBg2->setFlipX(true);
            addChild(glowBg2, 2);
            glowBg2->setScale(scale);
            glowBg2->setColor(glowColor);
            
            // White square for glow effect
            cocos2d::CCSprite* whiteSquare = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
            whiteSquare->setPosition(cocos2d::CCPoint(0.0f, scale * -52.0f));
            whiteSquare->setScaleX(scale * 5.2f);
            whiteSquare->setScaleY(scale * 2.2f);
            whiteSquare->setColor(glowColor);
            addChild(whiteSquare, 2);
            
            // Shadow
            cocos2d::CCSprite* shadow = cocos2d::CCSprite::createWithSpriteFrameName("chest_shadow_001.png");
            shadow->setPosition(cocos2d::CCPoint(0.0f, scale * -48.0f));
            addChild(shadow, 3);
            shadow->setScale(scale * 0.95f);
            shadow->setOpacity(90);
        }
    } else if (state == ChestSpriteState::Closed) {
        // Closed state
        std::string frameName = RewardsPage::getRewardFrame(m_chestType, 2);
        cocos2d::CCSprite* mainSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        addChild(mainSprite, 3);
    }
}

GJChestSprite::~GJChestSprite() {
    // Cleanup handled by base class
}