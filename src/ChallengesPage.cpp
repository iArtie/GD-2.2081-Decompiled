#include "ChallengesPage.h"
#include "CurrencyRewardLayer.h"
#include "LoadingCircle.h"
#include "GJChallengeItem.h"
#include "ChallengeNode.h"
#include "GameStatsManager.h"
#include "GameLevelManager.h"
#include "FMODAudioEngine.h"
#include "InfoAlertButton.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameToolbox.h"
#include "cocos2d.h"
#include <ctime>

void ChallengesPage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    // Empty implementation
}

void ChallengesPage::challengeStatusFailed() {
    GameStatsManager* stats = GameStatsManager::sharedState();
    if (!stats->areChallengesLoaded()) {
        cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(5.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(ChallengesPage::tryGetChallenges));
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delay, callFunc, nullptr);
        this->runAction(sequence);
    }

    if (m_circle && m_triedToLoad) {
        m_circle->setVisible(false);
    }
}

void ChallengesPage::challengeStatusFinished() {
    updateTimers(0.0f);

    int createdCount = 0;
    for (int i = 1; i <= 3; i++) {
        float delay = static_cast<float>(createdCount) * 0.15f;
        cocos2d::CCObject* obj = m_challengeNodes->objectForKey(i);
        
        if (obj && static_cast<ChallengeNode*>(obj)->m_unloaded) {
            GameStatsManager* stats = GameStatsManager::sharedState();
            if (stats->getQueuedChallenge(i)) {
                exitNodeAtSlot(i, delay);
                delay += 0.5f;
            }
        }

        if (createChallengeNode(i, false, delay, false)) {
            createdCount++;
        }
    }

    m_countdownLabel->setVisible(true);
    updateTimers(0.0f);
    this->unschedule(schedule_selector(ChallengesPage::updateTimers));
    this->schedule(schedule_selector(ChallengesPage::updateTimers));

    if (m_circle) {
        m_circle->setVisible(false);
    }

    updateDots();
}

ChallengesPage* ChallengesPage::create() {
    ChallengesPage* page = new ChallengesPage();
    if (page && page->init()) {
        page->autorelease();
        return page;
    }
    delete page;
    return nullptr;
}

void ChallengesPage::currencyWillExit(CurrencyRewardLayer* layer) {
    if (m_currencyRewardLayer == layer) {
        m_currencyRewardLayer = nullptr;
    }
}

void ChallengesPage::exitNodeAtSlot(int position, float delay) {
    ChallengeNode* node = static_cast<ChallengeNode*>(m_challengeNodes->objectForKey(position));
    if (!node) {
        return;
    }

    cocos2d::CCDelayTime* delayAction = cocos2d::CCDelayTime::create(delay);
    cocos2d::CCPoint pos = node->getPosition();
    pos.x = -160.0f;
    cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, pos);
    cocos2d::CCEaseElasticIn* ease = cocos2d::CCEaseElasticIn::create(moveTo, 1.2f);
    cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(node, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup));
    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(delayAction, ease, callFunc, nullptr);
    node->runAction(sequence);

    m_challengeNodes->removeObjectForKey(position);
}

bool ChallengesPage::init() {
    if (!FLAlertLayer::init(this, 150)) {
        return false;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_dots = cocos2d::CCArray::create();
    m_dots->retain();

    m_challengeNodes = cocos2d::CCDictionary::create();
    m_challengeNodes->retain();

    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    bg->setTag(-1);
    bg->setContentSize(cocos2d::CCSize(420.0f, 280.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    // Corner decorations
    for (int i = 1; i <= 4; i++) {
        cocos2d::CCSprite* corner = cocos2d::CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        m_mainLayer->addChild(corner, 8);

        float xOffset = 0.0f;
        float yOffset = 0.0f;

        if (i == 2) {
            corner->setFlipY(true);
            xOffset = -185.0f;
            yOffset = 115.0f;
        } else if (i == 3) {
            corner->setFlipX(true);
            corner->setFlipY(true);
            xOffset = 185.0f;
            yOffset = 115.0f;
        } else if (i == 4) {
            corner->setFlipX(true);
            xOffset = 185.0f;
            yOffset = -115.0f;
        } else {
            xOffset = -185.0f;
            yOffset = -115.0f;
        }

        corner->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + xOffset, winSize.height * 0.5f + yOffset));
    }

    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(ChallengesPage::onClose)
    );
    closeBtn->setSizeMult(1.6f);

    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 210.0f + 10.0f, winSize.height * 0.5f + 140.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);

    // Countdown label
    m_countdownLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(m_countdownLabel, 20);
    m_countdownLabel->setScale(0.5f);
    m_countdownLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    
    float labelWidth = m_countdownLabel->getContentSize().width;
    float labelScale = m_countdownLabel->getScale();
    m_countdownLabel->setPosition(cocos2d::CCPoint(
        -(labelWidth * labelScale * 0.5f - winSize.width * 0.5f),
        winSize.height * 0.5f - 140.0f + 15.0f
    ));

    this->schedule(schedule_selector(ChallengesPage::updateTimers));

    GameLevelManager::sharedState()->m_GJChallengeDelegate = this;

    // Create empty challenge slots
    for (int i = 1; i <= 3; i++) {
        createChallengeNode(i, true, 0.0f, false);
    }

    // Loading circle
    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = m_mainLayer;
    m_circle->m_fade = false;
    m_circle->show();

    m_circle->m_sprite->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f - 140.0f + 17.0f
    ));
    m_circle->m_sprite->setScale(0.28f);
    m_circle->setVisible(false);

    // Dot indicators
    cocos2d::CCPoint dotPos = cocos2d::CCPoint(
        winSize.width * 0.5f + 210.0f - 30.0f,
        winSize.height * 0.5f + 80.0f + 5.0f
    );

    for (int i = 0; i < 3; i++) {
        cocos2d::CCLabelBMFont* dot = cocos2d::CCLabelBMFont::create();
        dot->setScale(1.4f);
        dot->setColor(cocos2d::ccc3(255, 255, 150));
        dot->setOpacity(100);
        m_mainLayer->addChild(dot, 1);
        dot->setPosition(cocos2d::CCPoint(dotPos.x, dotPos.y - static_cast<float>(i) * 80.0f));
        dot->setVisible(false);
        m_dots->addObject(dot);
    }

    // Check if challenges are loaded
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double currentTime = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1000000000.0;

    if (GameStatsManager::sharedState()->m_challengeTime < currentTime) {
        GameStatsManager::sharedState()->resetChallengeTimer();
    }

    if (GameStatsManager::sharedState()->areChallengesLoaded()) {
        challengeStatusFinished();
    } else {
        m_countdownLabel->setVisible(false);
        GameLevelManager::sharedState()->getGJChallenges();
        m_circle->setVisible(true);
    }

    // Info button
    std::string title = "Quests";
    std::string desc = "<cg>Quests</c> are small daily challenges that give <cy>rewards</c>. The type of quest is decided randomly. You can have maximum 3 active and 3 queued at a time. Queued quests are indicated by <cl>dots</c> to the right of a quest.";
    InfoAlertButton* infoBtn = InfoAlertButton::create(title, desc, 1.0f);
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 210.0f - 5.0f, winSize.height * 0.5f + 140.0f - 5.0f)
    ));

    return true;
}

void ChallengesPage::keyBackClicked() {
    onClose(nullptr);
}

void ChallengesPage::onClose(cocos2d::CCObject* sender) {
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->setKeypadEnabled(false);
    this->removeFromParentAndCleanup(true);
}

void ChallengesPage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void ChallengesPage::show() {
    if (!m_noElasticity) {
        unsigned char opacity = this->getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();

        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));

        cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, pos);
        cocos2d::CCEaseElasticOut* ease = cocos2d::CCEaseElasticOut::create(moveTo, 0.6f);
        m_mainLayer->runAction(ease);

        if (m_scene && m_ZOrder > 0) {
            m_scene->addChild(this, m_ZOrder);
        } else {
            m_scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
            int highestZ = m_scene->getHighestChildZ();
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
            m_scene->addChild(this, m_ZOrder);
        }

        this->setOpacity(0);
        cocos2d::CCFadeTo* fadeIn = cocos2d::CCFadeTo::create(0.14f, opacity);
        this->runAction(fadeIn);
        this->setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void ChallengesPage::tryGetChallenges() {
    if (m_triedToLoad) {
        if (m_circle) {
            m_circle->setVisible(false);
        }
    } else {
        m_triedToLoad = true;
        GameLevelManager::sharedState()->getGJChallenges();
    }
}

void ChallengesPage::updateDots() {
    for (int i = 1; i <= 3; i++) {
        GJChallengeItem* queued = GameStatsManager::sharedState()->getQueuedChallenge(i);
        cocos2d::CCObject* dot = m_dots->objectAtIndex(i - 1);
        dot->setVisible(queued != nullptr);
    }
}

void ChallengesPage::updateTimers(float dt) {
    GameStatsManager* stats = GameStatsManager::sharedState();
    if (!stats->areChallengesLoaded()) {
        return;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double currentTime = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1000000000.0;

    int remaining = static_cast<int>(stats->m_challengeTime - currentTime);
    if (remaining < 0) {
        remaining = 0;
    }

    std::string timeString = GameToolbox::getTimeString(remaining, false);
    std::string labelText = "New quests in: " + timeString;
    m_countdownLabel->setString(labelText.c_str());

    // Update challenge node timers
    if (m_challengeNodes) {
        CCDictElement* element = m_challengeNodes->m_pElements;
        while (element) {
            ChallengeNode* node = static_cast<ChallengeNode*>(element->m_pObject);
            if (node) {
                // Update time labels for each challenge node
            }
            element = static_cast<CCDictElement*>(element->hh.next);
        }
    }

    if (remaining == 0 && !m_unkBool) {
        m_circle->setVisible(true);
        m_countdownLabel->setVisible(false);
        m_triedToLoad = false;
        m_unkBool = true;
        stats->resetChallengeTimer();
        tryGetChallenges();
    }
}

ChallengesPage::~ChallengesPage() {
    CC_SAFE_RELEASE_NULL(m_dots);
    CC_SAFE_RELEASE_NULL(m_challengeNodes);
}