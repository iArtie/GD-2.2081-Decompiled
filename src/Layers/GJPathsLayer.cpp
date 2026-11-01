#include "Layers/GJPathsLayer.h"
#include "Sprites/GJPathSprite.h"
#include "Pages/GJPathPage.h"
#include "Popups/GJPathRewardPopup.h"
#include "Managers/GameStatsManager.h"

GJPathsLayer* GJPathsLayer::create() {
    auto layer = new GJPathsLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJPathsLayer::init() {
    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->addImage("GJ_PathSheet.png");
    
    auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    frameCache->addSpriteFramesWithFile("GJ_PathSheet.plist");
    
    m_exiting = true;
    
    auto stats = GameStatsManager::sharedState();
    stats->trySelectActivePath();
    
    FLAlertLayer::incrementForcePrio();
    
    if (!FLAlertLayer::initWithColor(cocos2d::ccc4(0, 0, 0, 150))) {
        return false;
    }
    
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    auto bg = cocos2d::CCSprite::createWithSpriteFrameName("pathSelectBG_001.png");
    m_mainLayer->addChild(bg, -1);
    bg->setScale(1.2f);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    m_closeSprite = closeSprite;
    auto closeItem = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(GJPathsLayer::onClose));
    closeItem->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(closeItem, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(centerX - 120.0f, centerY + 120.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    auto labelPaths = cocos2d::CCSprite::createWithSpriteFrameName("label_paths_001.png");
    m_mainLayer->addChild(labelPaths, 20);
    labelPaths->setPosition(cocos2d::CCPoint(centerX, centerY + 29.0f));
    labelPaths->setScale(0.8f);
    
    auto labelOf = cocos2d::CCSprite::createWithSpriteFrameName("label_of_001.png");
    m_mainLayer->addChild(labelOf, 20);
    labelOf->setPosition(cocos2d::CCPoint(centerX, centerY));
    labelOf->setScale(0.8f);
    
    auto labelPower = cocos2d::CCSprite::createWithSpriteFrameName("label_power_001.png");
    m_mainLayer->addChild(labelPower, 20);
    labelPower->setPosition(cocos2d::CCPoint(centerX, centerY - 34.0f));
    labelPower->setScale(0.8f);
    
    cocos2d::CCPoint centerPos(centerX, centerY);
    
    for (int i = 0; i < 10; ++i) {
        auto pathSprite = GJPathSprite::create(i + 1);
        pathSprite->updateState();
        
        auto pathItem = CCMenuItemSpriteExtra::create(pathSprite, this, menu_selector(GJPathsLayer::onPath));
        pathItem->setTag(i + 1);
        m_buttonMenu->addChild(pathItem);
        
        float angle = (90.0f - i * 36.0f) * 0.0174532925f;
        float x = cosf(angle) * 100.0f;
        float y = sinf(angle) * 100.0f;
        
        cocos2d::CCPoint pos = centerPos + cocos2d::CCPoint(x, y);
        pathItem->setPosition(m_buttonMenu->convertToNodeSpace(pos));
    }
    
    return true;
}

void GJPathsLayer::darkenButtons(bool darken) {
    if (darken) {
        m_closeSprite->setColor(cocos2d::ccc3(75, 75, 75));
    } else {
        m_closeSprite->setColor(cocos2d::ccc3(255, 255, 255));
    }
}

void GJPathsLayer::keyBackClicked() {
    onClose(nullptr);
}

std::string GJPathsLayer::nameForPath(int path) {
    switch (path) {
        case 1: return "Fire";
        case 2: return "Ice";
        case 3: return "Poison";
        case 4: return "Shadow";
        case 5: return "Lava";
        case 6: return "Earth";
        case 7: return "Blood";
        case 8: return "Metal";
        case 9: return "Light";
        case 10: return "Souls";
        default: return "NA";
    }
}

void GJPathsLayer::onClose(cocos2d::CCObject* sender) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    stopAllActions();
    unscheduleAllSelectors();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJPathsLayer::onExit() {
    cocos2d::CCLayer::onExit();
    
    if (m_exiting) {
        auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
        frameCache->removeSpriteFramesFromFile("GJ_PathSheet.plist");
        
        auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
        textureCache->removeTextureForKey("GJ_PathSheet.png");
    }
}

void GJPathsLayer::onPath(cocos2d::CCObject* sender) {
    int path = sender->getTag();
    
    auto page = GJPathPage::create(path, this);
    page->m_noElasticity = true;
    page->show();
    
    auto stats = GameStatsManager::sharedState();
    if (stats->getStatFromKey(static_cast<StatKey>(path + 29)) > 999) {
        if (!stats->isPathChestUnlocked(path)) {
            auto rewardPopup = GJPathRewardPopup::create(path);
            rewardPopup->show();
        }
    }
}

void GJPathsLayer::registerWithTouchDispatcher() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void GJPathsLayer::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();
        
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));
        
        auto move = cocos2d::CCMoveTo::create(0.5f, pos);
        auto ease = cocos2d::CCEaseElasticOut::create(move, 0.6f);
        m_mainLayer->runAction(ease);
        
        auto director = cocos2d::CCDirector::sharedDirector();
        auto scene = director->getRunningScene();
        
        if (!m_scene) {
            m_scene = scene;
            int highestZ = scene->getHighestChildZ();
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
        }
        
        scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

GJPathsLayer::~GJPathsLayer() {
}