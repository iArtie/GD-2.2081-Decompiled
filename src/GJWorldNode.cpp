#include "GJWorldNode.h"
#include "WorldSelectLayer.h"
#include "GJGameLevel.h"
#include "GameStatsManager.h"
#include "GameLevelManager.h"
#include "GameManager.h"
#include "CCMenuItemSpriteExtra.h"
#include "FMODAudioEngine.h"
#include "WorldLevelPage.h"
#include "PlayLayer.h"
#include "CCCircleWave.h"
#include "LocalLevelManager.h"

GJWorldNode* GJWorldNode::create(int type, WorldSelectLayer* layer) {
    auto node = new GJWorldNode();
    if (node && node->init(type, layer)) {
        node->autorelease();
        return node;
    }
    delete node;
    return nullptr;
}

bool GJWorldNode::init(int type, WorldSelectLayer* layer) {
    if (!CCNode::init()) {
        return false;
    }
    
    m_islandType = type;
    m_selectLayer = layer;
    m_activeButton = nullptr;
    m_activeObjects = nullptr;
    m_level = nullptr;
    m_unlocked = false;
    m_islandUnlocked = false;
    m_arrowSprite = nullptr;
    m_particleColor = cocos2d::ccc3(type == 1 ? 255 : 50, 255, 0);
    
    const char* islandFrame = cocos2d::CCString::createWithFormat("worldIsland_%02d.png", (type > 2) ? 1 : type)->getCString();
    auto island = cocos2d::CCSprite::createWithSpriteFrameName(islandFrame);
    addChild(island);
    
    if (type > 2) {
        island->setColor(cocos2d::ccc3(100, 100, 100));
        island->setOpacity(75);
        island->setScale(0.8f);
        return true;
    }
    
    auto menu = cocos2d::CCMenu::create();
    addChild(menu, 2);
    
    int completedCount = 0;
    int startId = 5 * (type - 1) + 2001;
    
    for (int i = 0; i < 5; ++i) {
        if (GameStatsManager::sharedState()->hasCompletedMainLevel(startId + i)) {
            ++completedCount;
        } else {
            break;
        }
    }
    
    if (type > 1 && GameStatsManager::sharedState()->hasCompletedMainLevel(startId - 1)) {
        ++completedCount;
    }
    
    int activeLevelIdx = completedCount - 1;
    bool hasUnlocked = false;
    cocos2d::CCPoint lastPos(0.0f, 0.0f);
    
    for (int i = 0; i < 5; ++i) {
        int levelId = startId + i;
        auto mainLevel = GameLevelManager::sharedState()->getMainLevel(levelId, true);
        bool isActive = (activeLevelIdx == i && levelId > 2001 && !mainLevel->m_gauntletLevel2);
        bool isLocked = (i >= completedCount);
        
        cocos2d::CCPoint pos = positionForLevelButton(levelId);
        const char* frame = isLocked ? "worldLevelBtn_locked_001.png" : "worldLevelBtn_001.png";
        
        auto btn = cocos2d::CCSprite::createWithSpriteFrameName(frame);
        auto item = CCMenuItemSpriteExtra::create(btn, this, menu_selector(GJWorldNode::onLevel));
        item->setSizeMult(1.2f);
        item->setTag(levelId);
        menu->addChild(item);
        item->setPosition(menu->convertToNodeSpace(pos));
        
        auto node1 = cocos2d::CCNode::create();
        node1->setContentSize(cocos2d::CCSize(0.0f, 0.0f));
        btn->addChild(node1, 1);
        cocos2d::CCPoint scaledSize = btn->getScaledContentSize();
        node1->setPosition(cocos2d::CCPoint(scaledSize.x * 0.5f, scaledSize.y * 0.5f));
        
        auto node2 = cocos2d::CCNode::create();
        node2->setContentSize(cocos2d::CCSize(0.0f, 0.0f));
        btn->addChild(node2, -1);
        node2->setPosition(cocos2d::CCPoint(scaledSize.x * 0.5f, scaledSize.y * 0.5f));
        
        if (isLocked) {
            item->setEnabled(false);
        } else {
            lastPos = pos;
            if (isActive) {
                m_levelPosition = pos;
                m_activeObjects = cocos2d::CCArray::create();
                m_activeObjects->retain();
                m_activeButton = item;
                item->setEnabled(false);
                m_islandUnlocked = true;
            }
        }
        
        if (i == 0 && m_activeButton && !m_unlocked) {
            float delay = addDotsToLevel(levelId, isActive);
            m_unlocked = true;
            runAction(cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(delay + 0.2f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(GJWorldNode::unlockActiveItem)),
                nullptr
            ));
        }
    }
    
    m_arrowSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    m_arrowSprite->setScale(0.6f);
    m_arrowSprite->setRotation(-90.0f);
    m_arrowSprite->setPosition(lastPos + cocos2d::CCPoint(0.0f, 40.0f));
    addChild(m_arrowSprite, 10);
    
    if (completedCount > 0) {
        auto moveUp = cocos2d::CCMoveBy::create(0.5f, cocos2d::CCPoint(0.0f, -5.0f));
        auto easeUp = cocos2d::CCEaseInOut::create(moveUp, 1.8f);
        auto moveDown = cocos2d::CCMoveBy::create(0.5f, cocos2d::CCPoint(0.0f, 5.0f));
        auto easeDown = cocos2d::CCEaseInOut::create(moveDown, 1.8f);
        auto sequence = cocos2d::CCSequence::create(easeUp, easeDown, nullptr);
        auto repeat = cocos2d::CCRepeatForever::create(sequence);
        m_arrowSprite->runAction(repeat);
    }
    
    if (activeLevelIdx > 3 || m_activeButton) {
        m_arrowSprite->setVisible(false);
    }
    
    return true;
}

float GJWorldNode::addDotsToLevel(int levelID, bool animate) {
    int count = 0;
    
    for (int i = 0; ; ++i) {
        cocos2d::CCPoint pos = dotPositionForLevel(levelID, i);
        if (pos.x == 0.0f && pos.y == 0.0f) {
            break;
        }
        
        auto dot = cocos2d::CCSprite::createWithSpriteFrameName("worldDot_001.png");
        addChild(dot, 1);
        dot->setPosition(pos);
        
        if (animate) {
            dot->setOpacity(0);
            float scale = dot->getScale();
            dot->setScale(scale * 2.5f);
            
            auto scaleTo = cocos2d::CCScaleTo::create(0.3f, scale);
            auto easeScale = cocos2d::CCEaseBounceOut::create(scaleTo);
            auto fadeIn = cocos2d::CCFadeIn::create(0.1f);
            
            auto tint = cocos2d::CCTintTo::create(0.5f, 
                dot->getColor().r, 
                dot->getColor().g, 
                dot->getColor().b);
            dot->setColor(cocos2d::ccc3(100, 100, 200));
            
            auto spawn = cocos2d::CCSpawn::create(easeScale, fadeIn, tint, nullptr);
            float delay = count * 0.2f + 0.8f;
            auto sequence = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(delay),
                spawn,
                nullptr
            );
            dot->runAction(sequence);
        }
        
        ++count;
    }
    
    return count * 0.2f + 0.8f;
}

cocos2d::CCPoint GJWorldNode::positionForLevelButton(int levelID) {
    switch (levelID) {
        case 2001: return cocos2d::CCPoint(-184.0f, 18.0f);
        case 2002: return cocos2d::CCPoint(-75.0f, 0.0f);
        case 2003: return cocos2d::CCPoint(-2.0f, 40.0f);
        case 2004: return cocos2d::CCPoint(58.0f, 4.0f);
        case 2005: return cocos2d::CCPoint(160.0f, 19.0f);
        case 2006: return cocos2d::CCPoint(-175.0f, 19.0f);
        case 2007: return cocos2d::CCPoint(-70.0f, 2.0f);
        case 2008: return cocos2d::CCPoint(-10.0f, 46.0f);
        case 2009: return cocos2d::CCPoint(78.0f, -9.0f);
        case 2010: return cocos2d::CCPoint(168.0f, 20.0f);
        default: return cocos2d::CCPoint(0.0f, 0.0f);
    }
}

cocos2d::CCPoint GJWorldNode::dotPositionForLevel(int levelID, int index) {
    switch (levelID) {
        case 2002: {
            switch (index) {
                case 0: return cocos2d::CCPoint(-164.0f, 10.0f);
                case 1: return cocos2d::CCPoint(-152.0f, 4.0f);
                case 2: return cocos2d::CCPoint(-134.0f, 6.0f);
                case 3: return cocos2d::CCPoint(-111.0f, 6.0f);
                case 4: return cocos2d::CCPoint(-96.0f, 3.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2003: {
            switch (index) {
                case 0: return cocos2d::CCPoint(-68.0f, 8.0f);
                case 1: return cocos2d::CCPoint(-61.0f, 17.0f);
                case 2: return cocos2d::CCPoint(-51.0f, 26.0f);
                case 3: return cocos2d::CCPoint(-35.0f, 34.0f);
                case 4: return cocos2d::CCPoint(-20.0f, 36.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2004: {
            switch (index) {
                case 0: return cocos2d::CCPoint(9.0f, 32.0f);
                case 1: return cocos2d::CCPoint(11.0f, 21.0f);
                case 2: return cocos2d::CCPoint(12.0f, 13.0f);
                case 3: return cocos2d::CCPoint(20.0f, 5.0f);
                case 4: return cocos2d::CCPoint(38.0f, 3.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2005: {
            switch (index) {
                case 0: return cocos2d::CCPoint(75.0f, 8.0f);
                case 1: return cocos2d::CCPoint(89.0f, 9.0f);
                case 2: return cocos2d::CCPoint(106.0f, 9.0f);
                case 3: return cocos2d::CCPoint(126.0f, 7.0f);
                case 4: return cocos2d::CCPoint(143.0f, 11.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2007: {
            switch (index) {
                case 0: return cocos2d::CCPoint(-156.0f, 14.0f);
                case 1: return cocos2d::CCPoint(-137.0f, 14.0f);
                case 2: return cocos2d::CCPoint(-120.0f, 18.0f);
                case 3: return cocos2d::CCPoint(-103.0f, 17.0f);
                case 4: return cocos2d::CCPoint(-89.0f, 11.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2008: {
            switch (index) {
                case 0: return cocos2d::CCPoint(-63.0f, 23.0f);
                case 1: return cocos2d::CCPoint(-56.0f, 31.0f);
                case 2: return cocos2d::CCPoint(-42.0f, 37.0f);
                case 3: return cocos2d::CCPoint(-29.0f, 40.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2009: {
            switch (index) {
                case 0: return cocos2d::CCPoint(1.0f, 33.0f);
                case 1: return cocos2d::CCPoint(-8.0f, 24.0f);
                case 2: return cocos2d::CCPoint(-19.0f, 18.0f);
                case 3: return cocos2d::CCPoint(-23.0f, 9.0f);
                case 4: return cocos2d::CCPoint(-13.0f, -3.0f);
                case 5: return cocos2d::CCPoint(7.0f, -7.0f);
                case 6: return cocos2d::CCPoint(31.0f, -10.0f);
                case 7: return cocos2d::CCPoint(54.0f, -10.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        case 2010: {
            switch (index) {
                case 0: return cocos2d::CCPoint(95.0f, -4.0f);
                case 1: return cocos2d::CCPoint(104.0f, 3.0f);
                case 2: return cocos2d::CCPoint(114.0f, 10.0f);
                case 3: return cocos2d::CCPoint(127.0f, 14.0f);
                case 4: return cocos2d::CCPoint(147.0f, 18.0f);
                default: return cocos2d::CCPoint(0.0f, 0.0f);
            }
        }
        default:
            return cocos2d::CCPoint(0.0f, 0.0f);
    }
}

void GJWorldNode::onLevel(cocos2d::CCObject* sender) {
    if (m_selectLayer->m_enteringLevel || m_selectLayer->m_buttonsLocked) {
        return;
    }
    
    int levelId = sender->getTag();
    auto level = GameLevelManager::sharedState()->getMainLevel(levelId, true);
    m_level = level;
    
    auto page = WorldLevelPage::create(level, this);
    page->show();
}

void GJWorldNode::unlockActiveItem() {
    if (!m_activeButton) {
        return;
    }
    
    auto node = m_activeButton->getNormalImage();
    auto cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    node->setDisplayFrame(cache->spriteFrameByName("worldLevelBtn_001.png"));
    
    if (m_activeObjects) {
        for (int i = 0; i < m_activeObjects->count(); ++i) {
            auto obj = m_activeObjects->objectAtIndex(i);
            obj->setVisible(true);
        }
    }
    
    if (m_arrowSprite) {
        m_arrowSprite->setVisible(true);
    }
    
    for (int i = 0; i < 3; ++i) {
        auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("worldLevelBtn_001.png");
        node->addChild(sprite, 2);
        cocos2d::CCPoint scaledSize = node->getScaledContentSize();
        sprite->setPosition(cocos2d::CCPoint(scaledSize.x * 0.5f, scaledSize.y * 0.5f));
        sprite->setColor(cocos2d::ccc3(100, 100, 200));
        sprite->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
        sprite->runAction(cocos2d::CCFadeOut::create(0.5f));
    }
    
    float scale = node->getScale();
    auto scaleTo = cocos2d::CCScaleTo::create(0.5f, scale);
    auto easeBounce = cocos2d::CCEaseBounceOut::create(scaleTo);
    node->runAction(easeBounce);
    
    m_activeButton->setEnabled(true);
    
    int levelId = m_activeButton->getTag();
    auto mainLevel = GameLevelManager::sharedState()->getMainLevel(levelId, false);
    if (mainLevel) {
        mainLevel->m_gauntletLevel2 = true;
    }
    
    auto particles = cocos2d::CCParticleSystemQuad::create("coinPickupEffect.plist");
    particles->setAutoRemoveOnFinish(true);
    addChild(particles, 10);
    particles->setPosition(m_levelPosition);
    
    auto color = m_particleColor;
    particles->setStartColor(cocos2d::ccColor4F(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f));
    particles->setEndColor(cocos2d::ccColor4F(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f));
    particles->setScale(0.8f);
    
    auto wave1 = CCCircleWave::create(0, 16.0f, 4.0f, 0.3f, false);
    wave1->setColor(color);
    wave1->setPosition(m_levelPosition);
    addChild(wave1, 0);
    
    auto wave2 = CCCircleWave::create(4.0f, 40.0f, 0.3f, false);
    wave2->setColor(color);
    wave2->setPosition(m_levelPosition);
    wave2->m_uIndexInArray = 1;
    wave2->m_uUnknown = 4;
    addChild(wave2, 10);
}

void GJWorldNode::playStep1() {
    if (m_selectLayer->m_enteringLevel || m_selectLayer->m_buttonsLocked) {
        return;
    }
    
    m_selectLayer->m_enteringLevel = true;
    GameManager::sharedState()->m_unkSize4_13 = m_islandType;
    
    auto audio = FMODAudioEngine::sharedEngine();
    audio->stopAllMusic(true);
    audio->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJWorldNode::playStep2)),
        nullptr
    ));
}

void GJWorldNode::playStep2() {
    auto audio = FMODAudioEngine::sharedEngine();
    std::string fileName = GJGameLevel::getAudioFileName(m_level);
    audio->loadMusic(fileName, 1.0f, 0.0f, 1.0f, true, false, false, false);
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJWorldNode::playStep3)),
        nullptr
    ));
}

void GJWorldNode::playStep3() {
    auto manager = GameManager::sharedState();
    manager->m_sceneEnum = 9;
    
    auto localManager = LocalLevelManager::sharedState();
    int levelId = m_level->m_levelID.random - m_level->m_levelID.seed;
    m_level->m_levelString = localManager->getMainLevelString(levelId);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = PlayLayer::scene(m_level, false, false);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);
}

GJWorldNode::~GJWorldNode() {
    if (m_activeObjects) {
        m_activeObjects->release();
    }
}