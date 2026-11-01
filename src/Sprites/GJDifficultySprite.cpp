// GJDifficultySprite.cpp
#include "Sprites/GJDifficultySprite.h"
#include "Models/GJGameLevel.h"

GJDifficultySprite* GJDifficultySprite::create(int difficulty, GJDifficultyName name) {
    GJDifficultySprite* instance = new GJDifficultySprite();
    if (instance && instance->init(difficulty, name)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

std::string GJDifficultySprite::getDifficultyFrame(int difficulty, GJDifficultyName name) {
    std::string frame = cocos2d::CCString::createWithFormat("difficulty_%02d_", difficulty)->getCString();
    
    // Check if difficulty is 6-10 and name is Long
    if (difficulty >= 6 && difficulty <= 10 && name == GJDifficultyName::Long) {
        frame += "btn2_001.png";
    } else {
        frame += "btn_001.png";
        if (difficulty == -1) {
            frame = "difficulty_auto_btn_001.png";
        }
    }
    
    return frame;
}

bool GJDifficultySprite::init(int difficulty, GJDifficultyName name) {
    std::string frameName = getDifficultyFrame(difficulty, name);
    if (!cocos2d::CCSprite::initWithSpriteFrameName(frameName)) {
        return false;
    }
    
    m_featureState = GJFeatureState::None;
    return true;
}

void GJDifficultySprite::updateDifficultyFrame(int difficulty, GJDifficultyName name) {
    std::string frameName = getDifficultyFrame(difficulty, name);
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName);
    setDisplayFrame(frame);
    
    // Update child positions if any
    if (m_pChildren && m_pChildren->count() > 0) {
        cocos2d::CCPoint center = cocos2d::CCPoint(getContentSize().width * 0.5f, getContentSize().height * 0.5f);
        for (int i = 0; i < m_pChildren->count(); ++i) {
            cocos2d::CCObject* child = m_pChildren->objectAtIndex(i);
            child->setPosition(center);
        }
    }
}

void GJDifficultySprite::updateFeatureState(GJFeatureState state) {
    if (m_featureState == state) {
        return;
    }
    
    m_featureState = state;
    removeAllChildren();
    
    if (m_featureState != GJFeatureState::None) {
        std::string coinFrame = "GJ_featuredCoin_001.png";
        
        switch (m_featureState) {
            case GJFeatureState::Mythic:
                coinFrame = "GJ_epicCoin3_001.png";
                break;
            case GJFeatureState::Legendary:
                coinFrame = "GJ_epicCoin2_001.png";
                break;
            case GJFeatureState::Epic:
                coinFrame = "GJ_epicCoin_001.png";
                break;
            default:
                break;
        }
        
        cocos2d::CCSprite* coin = cocos2d::CCSprite::createWithSpriteFrameName(coinFrame);
        addChild(coin, -2);
        coin->setPosition(cocos2d::CCPoint(getContentSize().width * 0.5f, getContentSize().height * 0.5f));
        
        // Add particle effects for Mythic
        if (m_featureState == GJFeatureState::Mythic) {
            std::string particleString = "30a-1a1.3a0.2a20a90a0a10a5a20a20a0a0a8a0a0a0a4a1a0a0a0a0a1a0a1a0a1a0a1a1a0a0a0a0a0.784314a0a1a0a1a0a0.27a0a0.27a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0.25a0a0a0a0a0a0a0a0a0a0a0";
            cocos2d::CCParticleSystemQuad* particles = GameToolbox::particleFromString(particleString, nullptr, nullptr);
            addChild(particles, -1);
            particles->setScale(0.9f);
            particles->setPosition(cocos2d::CCPoint(getContentSize().width * 0.5f, getContentSize().height * 0.5f));
            particles->resetSystem();
            
            // Update particles a few times to initialize
            for (int i = 0; i < 5; ++i) {
                particles->update(0.15f);
            }
        }
    }
}

void GJDifficultySprite::updateFeatureStateFromLevel(GJGameLevel* level) {
    if (!level) {
        return;
    }
    
    GJFeatureState state = GJFeatureState::None;
    
    if (level->m_featured > 0) {
        if (level->m_isEpic > 0) {
            if (level->m_isEpic > 2) {
                state = GJFeatureState::Mythic;
            } else if (level->m_isEpic == 2) {
                state = GJFeatureState::Legendary;
            } else {
                state = GJFeatureState::Epic;
            }
        } else {
            state = GJFeatureState::Featured;
        }
    } else if (level->m_isEpic > 0) {
        if (level->m_isEpic > 2) {
            state = GJFeatureState::Mythic;
        } else if (level->m_isEpic == 2) {
            state = GJFeatureState::Legendary;
        } else {
            state = GJFeatureState::Epic;
        }
    }
    
    updateFeatureState(state);
}

GJDifficultySprite::~GJDifficultySprite() {
    // Base class handles cleanup
}