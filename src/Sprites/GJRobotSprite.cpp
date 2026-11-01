#include "Sprites/GJRobotSprite.h"
#include "Cocos/CCPartAnimSprite.h"
#include "Cocos/CCSpritePart.h"
#include "Managers/GameManager.h"

GJRobotSprite* GJRobotSprite::create(int frame) {
    auto sprite = new GJRobotSprite();
    if (sprite && sprite->init(frame)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

bool GJRobotSprite::init(int frame) {
    m_iconType = IconType::Robot;
    m_iconRequestID = 0;
    m_hasExtra = false;
    return true;
}

void GJRobotSprite::hideGlow() {
    if (m_glowSprite) {
        m_glowSprite->setVisible(false);
    }
}

void GJRobotSprite::hideSecondary() {
    if (m_secondArray) {
        for (int i = 0; i < m_secondArray->count(); ++i) {
            auto obj = m_secondArray->objectAtIndex(i);
            obj->setVisible(false);
        }
    }
    if (m_extraSprite) {
        m_extraSprite->setVisible(false);
    }
}

void GJRobotSprite::setOpacity(unsigned char opacity) {
    if (m_secondArray) {
        for (int i = 0; i < m_secondArray->count(); ++i) {
            auto obj = m_secondArray->objectAtIndex(i);
            obj->setOpacity(opacity);
        }
    }
    
    if (m_paSprite) {
        m_paSprite->setOpacity(opacity);
    }
    
    if (m_glowSprite) {
        auto children = m_glowSprite->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                auto obj = children->objectAtIndex(i);
                obj->setOpacity(opacity);
            }
        }
    }
    
    if (m_extraSprite) {
        m_extraSprite->setOpacity(opacity);
    }
}

void GJRobotSprite::showGlow() {
    if (m_glowSprite) {
        m_glowSprite->setVisible(true);
    }
}

void GJRobotSprite::updateColor01(cocos2d::ccColor3B color) {
    m_color = color;
    updateColors();
}

void GJRobotSprite::updateColor02(cocos2d::ccColor3B color) {
    m_secondColor = color;
    updateColors();
}

void GJRobotSprite::updateColors() {
    if (!m_paSprite) {
        return;
    }
    
    float darkFactor = (m_iconType == IconType::Spider) ? 0.5f : 0.7f;
    
    cocos2d::ccColor3B darkenedMain = cocos2d::ccc3(
        static_cast<unsigned char>(m_color.r * darkFactor),
        static_cast<unsigned char>(m_color.g * darkFactor),
        static_cast<unsigned char>(m_color.b * darkFactor)
    );
    
    cocos2d::ccColor3B darkenedSecond = cocos2d::ccc3(
        static_cast<unsigned char>(m_secondColor.r * darkFactor),
        static_cast<unsigned char>(m_secondColor.g * darkFactor),
        static_cast<unsigned char>(m_secondColor.b * darkFactor)
    );
    
    m_paSprite->setColor(m_color);
    
    if (m_secondArray) {
        for (int i = 0; i < m_secondArray->count(); ++i) {
            auto obj = m_secondArray->objectAtIndex(i);
            obj->setColor(m_secondColor);
        }
    }
    
    auto back1 = m_paSprite->getSpriteForKey("back01");
    auto back2 = m_paSprite->getSpriteForKey("back02");
    auto back3 = m_paSprite->getSpriteForKey("back03");
    
    if (back1) {
        back1->setColor(darkenedMain);
        auto children = back1->getChildren();
        if (children && children->count() > 0) {
            auto child = children->objectAtIndex(0);
            child->setColor(darkenedSecond);
        }
    }
    
    if (back2) {
        back2->setColor(darkenedMain);
        auto children = back2->getChildren();
        if (children && children->count() > 0) {
            auto child = children->objectAtIndex(0);
            child->setColor(darkenedSecond);
        }
    }
    
    if (back3) {
        back3->setColor(darkenedMain);
        auto children = back3->getChildren();
        if (children && children->count() > 0) {
            auto child = children->objectAtIndex(0);
            child->setColor(darkenedSecond);
        }
    }
}

void GJRobotSprite::updateFrame(int frame) {
    int maxFrame = (m_iconType == IconType::Robot) ? 68 : 69;
    if (frame > maxFrame) {
        frame = maxFrame;
    }
    if (frame < 1) {
        frame = 1;
    }
    
    auto manager = GameManager::sharedState();
    auto texture = manager->loadIcon(frame, m_iconType, m_iconRequestID);
    setTexture(texture);
    
    if (!m_paSprite) {
        return;
    }
    
    auto parts = m_paSprite->m_spriteParts;
    if (!parts) {
        return;
    }
    
    for (int i = 0; i < parts->count(); ++i) {
        auto part = static_cast<CCSpritePart*>(parts->objectAtIndex(i));
        int partId = part->getTag();
        
        std::string baseName;
        if (m_iconType == IconType::Robot) {
            baseName = cocos2d::CCString::createWithFormat("robot_%02d_%02d", frame, partId)->getCString();
        } else {
            baseName = cocos2d::CCString::createWithFormat("spider_%02d_%02d", frame, partId)->getCString();
        }
        
        std::string mainFrame = baseName + "_001.png";
        std::string secondFrame = baseName + "_2_001.png";
        std::string glowFrame = baseName + "_glow_001.png";
        
        auto cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
        
        auto mainSpriteFrame = cache->spriteFrameByName(mainFrame.c_str());
        if (mainSpriteFrame) {
            part->setDisplayFrame(mainSpriteFrame);
        }
        
        if (i < m_secondArray->count()) {
            auto obj = m_secondArray->objectAtIndex(i);
            auto secondSpriteFrame = cache->spriteFrameByName(secondFrame.c_str());
            if (secondSpriteFrame && obj) {
                obj->setDisplayFrame(secondSpriteFrame);
                cocos2d::CCPoint size = cocos2d::CCPoint(
                    part->getContentSize().width * 0.5f,
                    part->getContentSize().height * 0.5f
                );
                obj->setPosition(size);
            }
        }
        
        if (i < m_glowSprite->getChildren()->count()) {
            auto obj = m_glowSprite->getChildren()->objectAtIndex(i);
            auto glowSpriteFrame = cache->spriteFrameByName(glowFrame.c_str());
            if (glowSpriteFrame && obj) {
                obj->setDisplayFrame(glowSpriteFrame);
            }
        }
        
        if (part == m_headSprite) {
            std::string extraFrame = baseName + "_extra_001.png";
            auto extraSpriteFrame = cache->spriteFrameByName(extraFrame.c_str());
            
            if (extraSpriteFrame) {
                if (!m_extraSprite) {
                    m_extraSprite = cocos2d::CCSprite::createWithSpriteFrame(extraSpriteFrame);
                    if (m_extraSprite && m_headSprite) {
                        m_headSprite->addChild(m_extraSprite, 2);
                    }
                }
                if (m_extraSprite) {
                    m_extraSprite->setDisplayFrame(extraSpriteFrame);
                    m_extraSprite->setPosition(cocos2d::CCPoint(
                        part->getContentSize().width * 0.5f,
                        part->getContentSize().height * 0.5f
                    ));
                    m_hasExtra = true;
                    m_extraSprite->setVisible(true);
                }
            } else {
                m_hasExtra = false;
                if (m_extraSprite) {
                    m_extraSprite->setVisible(false);
                }
            }
        }
    }
}

GJRobotSprite::~GJRobotSprite() {
}