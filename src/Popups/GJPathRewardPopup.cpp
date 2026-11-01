#include "Popups/GJPathRewardPopup.h"
#include "Managers/GameStatsManager.h"
#include "Models/GJRewardItem.h"
#include "Layers/RewardUnlockLayer.h"
#include "Sprites/GJChestSprite.h"

GJPathRewardPopup* GJPathRewardPopup::create(int path) {
    auto popup = new GJPathRewardPopup();
    if (popup && popup->init(path)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool GJPathRewardPopup::init(int path) {
    m_pathNumber = path;
    FLAlertLayer::incrementForcePrio();
    
    if (!FLAlertLayer::initWithColor(cocos2d::ccc4(0, 0, 0, 150))) {
        return false;
    }
    
    m_noElasticity = true;
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    auto invisibleBtn = cocos2d::CCNode::create();
    invisibleBtn->setContentSize(cocos2d::CCSize(100.0f, 100.0f));
    auto claimItem = CCMenuItemSpriteExtra::create(invisibleBtn, this, menu_selector(GJPathRewardPopup::onClaim));
    m_buttonMenu = cocos2d::CCMenu::create(claimItem, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(centerX, centerY));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    cocos2d::CCPoint basePos(0.0f, 10.0f);
    auto content = cocos2d::CCNode::create();
    content->setContentSize(cocos2d::CCSize(100.0f, 100.0f));
    invisibleBtn->addChild(content);
    content->setPosition(cocos2d::CCPoint(50.0f, 50.0f));
    
    auto titleLabel = cocos2d::CCLabelBMFont::create("Path Complete!", "bigFont.fnt");
    titleLabel->setScale(0.65f);
    content->addChild(titleLabel, 22);
    titleLabel->setPosition(basePos + cocos2d::CCPoint(0.0f, 40.0f));
    
    auto subLabel = cocos2d::CCLabelBMFont::create("Claim your reward", "bigFont.fnt");
    subLabel->setScale(titleLabel->getScale());
    content->addChild(subLabel, 21);
    subLabel->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -5.0f));
    subLabel->setColor(cocos2d::ccc3(125, 125, 125));
    subLabel->setOpacity(50);
    
    auto chest = GJChestSprite::create(0);
    content->addChild(chest, 22);
    chest->setScale(0.6f);
    chest->setPosition(basePos + cocos2d::CCPoint(0.0f, -6.0f));
    
    auto chestGlow = GJChestSprite::create(0);
    content->addChild(chestGlow, 21);
    chestGlow->setScale(chest->getScale());
    chestGlow->setPosition(chest->getPosition() + cocos2d::CCPoint(0.0f, -5.0f));
    
    auto glowChildren = chestGlow->getChildren();
    if (glowChildren && glowChildren->count() > 0) {
        auto glow = glowChildren->objectAtIndex(0);
        glow->setColor(cocos2d::ccc3(125, 125, 125));
        glow->setOpacity(50);
    }
    
    auto moveUp = cocos2d::CCMoveBy::create(1.0f, cocos2d::CCPoint(0.0f, 5.0f));
    auto easeUp = cocos2d::CCEaseInOut::create(moveUp, 2.0f);
    auto moveDown = cocos2d::CCMoveBy::create(1.0f, cocos2d::CCPoint(0.0f, -5.0f));
    auto easeDown = cocos2d::CCEaseInOut::create(moveDown, 2.0f);
    auto sequence = cocos2d::CCSequence::create(easeUp, easeDown, nullptr);
    auto repeat = cocos2d::CCRepeatForever::create(sequence);
    content->runAction(repeat);
    
    return true;
}

void GJPathRewardPopup::closePopup() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    stopAllActions();
    unscheduleAllSelectors();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJPathRewardPopup::keyBackClicked() {
}

void GJPathRewardPopup::onClaim(cocos2d::CCObject* sender) {
    auto stats = GameStatsManager::sharedState();
    auto reward = stats->unlockPathChest(m_pathNumber);
    
    if (reward) {
        runAction(cocos2d::CCFadeTo::create(0.2f, 0));
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.2f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(GJPathRewardPopup::closePopup)),
            nullptr
        ));
        
        auto unlockLayer = RewardUnlockLayer::create(reward->m_rewardType, 0);
        auto parent = getParent();
        int zOrder = getZOrder();
        parent->addChild(unlockLayer, zOrder + 1);
        unlockLayer->showCollectReward(reward);
    } else {
        closePopup();
    }
}

GJPathRewardPopup::~GJPathRewardPopup() {
}