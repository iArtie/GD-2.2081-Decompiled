#include "DailyLevelPage.h"
#include "GJGameLevel.h"
#include "DailyLevelNode.h"
#include "LoadingCircle.h"
#include "GameLevelManager.h"
#include "GameStatsManager.h"
#include "GameManager.h"
#include "DialogLayer.h"
#include "DialogObject.h"
#include "LevelBrowserLayer.h"
#include "GJSearchObject.h"
#include "GameToolbox.h"
#include "extension/CCScale9Sprite.h"

void DailyLevelPage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    // Empty implementation
}

DailyLevelPage* DailyLevelPage::create(GJTimedLevelType type)
{
    DailyLevelPage* pRet = new DailyLevelPage();
    if (pRet && pRet->init(type))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void DailyLevelPage::createDailyNode(GJGameLevel* level, bool instant, float delay, bool isNew)
{
    if (m_dailyNode)
        return;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    DailyLevelNode* node = DailyLevelNode::create(level, this, isNew);
    m_mainLayer->addChild(node);
    node->setPosition(cocos2d::CCPoint(centerX, centerY - 8.0f));
    
    if (level)
    {
        int currentDailyID = GameLevelManager::sharedState()->getDailyID(m_type);
        int levelDailyID = level->m_dailyID.random - level->m_dailyID.seed;
        
        if (currentDailyID > levelDailyID)
        {
            node->showSkipButton();
            if (instant)
            {
                m_dailyNode = node;
                return;
            }
        }
        else if (instant)
        {
            m_dailyNode = node;
            return;
        }
    }
    else if (instant)
    {
        m_dailyNode = node;
        return;
    }
    
    node->setVisible(false);
    cocos2d::CCPoint targetPos = node->getPosition();
    node->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() + 180.0f, targetPos.y));
    
    node->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(delay),
        cocos2d::CCShow::create(),
        cocos2d::CCEaseElasticOut::create(cocos2d::CCMoveTo::create(0.5f, targetPos), 0.6f),
        nullptr
    ));
    
    m_dailyNode = node;
}

void DailyLevelPage::createNodeIfLoaded()
{
    m_downloadStarted = false;
    
    int dailyID = GameLevelManager::sharedState()->getDailyID(m_type);
    if (dailyID > 0)
    {
        GJGameLevel* level = GameLevelManager::sharedState()->getSavedDailyLevel(dailyID);
        if (level)
            createDailyNode(level, false, 0.0f, true);
        else
            m_nodeCircle->setVisible(true);
    }
}

void DailyLevelPage::dailyStatusFailed(GJTimedLevelType type, GJErrorCode errorType)
{
    if (errorType == UpdateApp)
    {
        m_gettingDailyStatus = true;
        
        DialogObject* dialog = DialogObject::create(
            "The Mechanic",
            "Please <cg>update</c> your <cl>application</c> to download this <cy>level</c>.",
            37,
            1.0f,
            false,
            cocos2d::ccColor3B(255, 255, 255)
        );
        
        DialogLayer* layer = DialogLayer::create(dialog);
        layer->addToMainScene();
        layer->updateChatPlacement(Center);
        layer->animateIn(FromCenter);
    }
    else
    {
        if (!GameStatsManager::sharedState()->areChallengesLoaded())
        {
            runAction(cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(5.0f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(DailyLevelPage::tryGetDailyStatus)),
                nullptr
            ));
        }
    }
    
    if (m_gettingDailyStatus)
    {
        m_timeCircle->setVisible(false);
        m_nodeCircle->setVisible(false);
    }
}

void DailyLevelPage::dailyStatusFinished(GJTimedLevelType type)
{
    updateTimers(0.0f);
    m_timeLabel->setVisible(true);
    updateTimers(0.0f);
    
    unschedule(schedule_selector(DailyLevelPage::updateTimers));
    schedule(schedule_selector(DailyLevelPage::updateTimers), 0.0f);
    
    m_timeCircle->setVisible(false);
    
    int dailyID = GameLevelManager::sharedState()->getDailyID(m_type);
    
    if (!GameStatsManager::sharedState()->hasCompletedDailyLevel(dailyID))
    {
        int activeID = GameLevelManager::sharedState()->getActiveDailyID(m_type);
        int currentID = GameLevelManager::sharedState()->getDailyID(m_type);
        
        if (activeID < currentID)
        {
            if (m_dailyNode)
            {
                if (m_dailyNode->m_needsDownloading)
                {
                    m_dailyNode->updateTimeLabel("00:00:00");
                    exitDailyNode(m_dailyNode, 0.0f);
                    downloadAndCreateNode();
                }
                else
                {
                    m_dailyNode->showSkipButton();
                }
            }
            else
            {
                m_nodeCircle->setVisible(true);
                GameLevelManager::sharedState()->downloadLevel(m_downloadLevelID, false, false);
            }
        }
    }
}

void DailyLevelPage::downloadAndCreateNode()
{
    m_downloadStarted = true;
    GameLevelManager::sharedState()->downloadLevel(m_downloadLevelID, false, false);
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(DailyLevelPage::createNodeIfLoaded)),
        nullptr
    ));
}

void DailyLevelPage::exitDailyNode(DailyLevelNode* node, float delay)
{
    if (!node)
        return;
    
    if (m_dailyNode == node)
        m_dailyNode = nullptr;
    
    cocos2d::CCPoint pos = node->getPosition();
    node->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(delay),
        cocos2d::CCEaseElasticIn::create(cocos2d::CCMoveTo::create(0.5f, cocos2d::CCPoint(-160.0f, pos.y)), 1.2f),
        cocos2d::CCCallFunc::create(node, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup), 1),
        nullptr
    ));
}

int DailyLevelPage::getDailyTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double currentTime = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    
    int timeLeft = (int)((double)GameLevelManager::sharedState()->getDailyTimer(m_type) - currentTime);
    if (timeLeft < 0)
        return 0;
    return timeLeft;
}

std::string DailyLevelPage::getDailyTimeString(int timeLeft)
{
    std::string result;
    
    if (m_type == Event)
    {
        // Random event messages
        int random = (int)(((float)rand() * 4.6566e-10f) * 1000.0f);
        if (random == 187)
        {
            result = "This message is extremely rare.";
        }
        else
        {
            switch ((int)(((float)rand() * 4.6566e-10f) * 30.0f))
            {
                case 0: result = "New event level in ... ?"; break;
                case 1: result = "Events are pretty random"; break;
                case 2: result = "Next event in random hours from now"; break;
                case 3: result = "When is the next event?"; break;
                case 4: result = "NEW EVENT! .... soon"; break;
                case 5: result = "Why did I program this..."; break;
                case 6: result = "I believe in you"; break;
                case 7: result = "You can do it"; break;
                case 8: result = "Next one is a banger"; break;
                case 9: result = "This is fun"; break;
                case 10: result = "I can't find anything"; break;
                case 11: result = "Why is this a thing?"; break;
                case 12: result = "Do or do not. There is no try"; break;
                case 13: result = "The cake is a lie"; break;
                case 14: result = "Challenge accepted"; break;
                case 15: result = "I wonder what's next"; break;
                case 16: result = "Welcome to the event"; break;
                case 17: result = "Jump cube, jump"; break;
                case 18: result = "No event for you"; break;
                case 19: result = "This looks interesting"; break;
                case 20: result = "Something epic is coming..."; break;
                case 21: result = "Unlocking something... or not"; break;
                case 22: result = "This could be legendary"; break;
                case 23: result = "I wonder if it's a demon?"; break;
                case 24: result = "A wild event appears!"; break;
                case 25: result = "Let me cook"; break;
                case 26: result = "Please wait, cooking."; break;
                case 27: result = "I don't think it's coming"; break;
                case 28: result = "Geometry Dashn't"; break;
                case 29: result = "Uber gamer moment"; break;
                default: result = "Free event is free"; break;
            }
        }
    }
    else
    {
        std::string timeStr = GameToolbox::getTimeString(timeLeft, false);
        std::string typeStr;
        
        if (m_type == Weekly)
            typeStr = "Weekly Demon";
        else if (m_type == Event)
            typeStr = "Event Level";
        else
            typeStr = "Daily Level";
        
        result = cocos2d::CCString::createWithFormat("New %s in: %s", typeStr.c_str(), timeStr.c_str())->getCString();
    }
    
    return result;
}

bool DailyLevelPage::init(GJTimedLevelType type)
{
    incrementForcePrio();
    
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 150)))
        return false;
    
    m_type = type;
    m_dailyNode = nullptr;
    m_gettingDailyStatus = false;
    m_downloadStarted = false;
    m_noElasticity = false;
    m_joystickConnected = -1;
    m_ZOrder = 0;
    m_scene = nullptr;
    
    std::string labelTexture;
    std::string cornerTexture;
    std::string bgTexture;
    
    if (type == Weekly)
    {
        labelTexture = "weeklyLevelLabel_001.png";
        cornerTexture = "gauntletCorner_001.png";
        bgTexture = "GJ_square05.png";
        m_downloadLevelID = -2;
    }
    else if (type == Event)
    {
        labelTexture = "eventLevelLabel_001.png";
        cornerTexture = "dailyLevelCorner_001.png";
        bgTexture = "GJ_square05.png";
        m_downloadLevelID = -3;
    }
    else
    {
        labelTexture = "dailyLevelLabel_001.png";
        cornerTexture = "dailyLevelCorner_001.png";
        bgTexture = "GJ_square01.png";
        m_downloadLevelID = -1;
    }
    
    GameManager::sharedState()->m_dailyLevelPage = this;
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        bgTexture.c_str(), cocos2d::CCRect(0, 0, 80, 80));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(420, 280));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    if (m_type == Event)
        bg->setColor(cocos2d::ccColor3B(150, 200, 255));
    
    // Label
    cocos2d::CCSprite* label = cocos2d::CCSprite::createWithSpriteFrameName(labelTexture);
    m_mainLayer->addChild(label, 20);
    label->setPosition(cocos2d::CCPoint(centerX, centerY + 140.0f - 29.0f));
    
    // Corner decorations
    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCSprite* corner = cocos2d::CCSprite::createWithSpriteFrameName(cornerTexture);
        m_mainLayer->addChild(corner, 8);
        
        if (m_type == Weekly)
        {
            cocos2d::CCSprite* whiteSquare = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
            whiteSquare->setScaleX(0.3f);
            whiteSquare->setScaleY(0.8f);
            whiteSquare->setColor(cocos2d::ccColor3B(255, 255, 255));
            whiteSquare->setRotation(-45.0f);
            whiteSquare->setPosition(cocos2d::CCPoint(2.0f, 2.0f));
            corner->addChild(whiteSquare, 1);
            corner->setScale(0.63f);
        }
        
        float xOffset, yOffset;
        if (i == 2)
        {
            corner->setScaleX(-corner->getScaleX());
            corner->setScaleY(-corner->getScaleY());
            corner->setVisible(false);
            xOffset = 185.0f;
            yOffset = 115.0f;
        }
        else if (i == 3)
        {
            corner->setScaleX(-corner->getScaleX());
            xOffset = 185.0f;
            yOffset = -115.0f;
        }
        else if (i == 1)
        {
            corner->setScaleY(-corner->getScaleY());
            xOffset = -185.0f;
            yOffset = 115.0f;
        }
        else
        {
            xOffset = -185.0f;
            yOffset = -115.0f;
        }
        
        corner->setPosition(cocos2d::CCPoint(centerX + xOffset, centerY + yOffset));
    }
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(DailyLevelPage::onClose));
    closeBtn->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(centerX - 210.0f + 10.0f, centerY + 140.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // The Safe button
    cocos2d::CCSprite* safeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_safeBtn_001.png");
    CCMenuItemSpriteExtra* safeBtn = CCMenuItemSpriteExtra::create(
        safeSprite, nullptr, this, menu_selector(DailyLevelPage::onTheSafe));
    safeBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(safeBtn);
    safeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(centerX + 210.0f - 30.0f, centerY + 140.0f - 30.0f)));
    
    // Time label
    m_timeLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_timeLabel->setScale(type == Event ? 1.0f : 0.5f);
    m_timeLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(m_timeLabel);
    m_timeLabel->setPosition(cocos2d::CCPoint(
        centerX - (m_timeLabel->getContentSize().width * m_timeLabel->getScale() * 0.5f),
        centerY - 140.0f + 15.0f));
    
    schedule(schedule_selector(DailyLevelPage::updateTimers), 0.0f);
    
    // Time circle loader
    m_timeCircle = LoadingCircle::create();
    m_timeCircle->m_parentLayer = m_mainLayer;
    m_timeCircle->m_fade = false;
    m_timeCircle->show();
    m_timeCircle->m_sprite->setPosition(cocos2d::CCPoint(centerX, centerY - 140.0f + 17.0f));
    m_timeCircle->m_sprite->setScale(0.28f);
    m_timeCircle->setVisible(false);
    
    // Node circle loader
    m_nodeCircle = LoadingCircle::create();
    m_nodeCircle->m_parentLayer = m_mainLayer;
    m_nodeCircle->m_fade = false;
    m_nodeCircle->show();
    m_nodeCircle->m_sprite->setPosition(cocos2d::CCPoint(centerX, centerY));
    m_nodeCircle->m_sprite->setScale(0.5f);
    m_nodeCircle->setVisible(false);
    
    // Check for new daily level
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double currentTime = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    
    if ((double)GameLevelManager::sharedState()->getDailyTimer(m_type) < currentTime)
        GameLevelManager::sharedState()->resetDailyLevelState(m_type);
    
    int activeID = GameLevelManager::sharedState()->getActiveDailyID(m_type);
    if (activeID > 0)
    {
        GJGameLevel* level = nullptr;
        if (!GameStatsManager::sharedState()->hasCompletedDailyLevel(activeID))
            level = GameLevelManager::sharedState()->getSavedDailyLevel(activeID);
        createDailyNode(level, true, 0.0f, false);
    }
    
    if (GameLevelManager::sharedState()->hasDailyStateBeenLoaded(m_type))
    {
        dailyStatusFinished(m_type);
    }
    else
    {
        if (m_type != Event)
        {
            m_timeLabel->setVisible(false);
            m_timeCircle->setVisible(true);
        }
        GameLevelManager::sharedState()->getGJDailyLevelState(m_type);
        if (!m_dailyNode)
            m_nodeCircle->setVisible(true);
    }
    
    GameLevelManager::sharedState()->m_GJDailyLevelDelegate = this;
    GameLevelManager::sharedState()->m_levelDownloadDelegate = this;
    
    return true;
}

void DailyLevelPage::keyBackClicked()
{
    onClose(nullptr);
}

void DailyLevelPage::levelDownloadFailed(int response)
{
    if (response == -1)
        m_nodeCircle->setVisible(false);
}

void DailyLevelPage::levelDownloadFinished(GJGameLevel* level)
{
    if (level && level->m_dailyID.random - level->m_dailyID.seed > 0)
    {
        m_nodeCircle->setVisible(false);
        if (!m_downloadStarted)
            createDailyNode(level, false, 0.0f, true);
    }
}

void DailyLevelPage::onClose(cocos2d::CCObject* sender)
{
    stopAllActions();
    unscheduleAllSelectors();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void DailyLevelPage::onTheSafe(cocos2d::CCObject* sender)
{
    int searchType;
    if (m_type == Weekly)
        searchType = 22;
    else if (m_type == Event)
        searchType = 23;
    else
        searchType = 20;
    
    GJSearchObject* searchObj = GJSearchObject::create((GJSearchObject::Type)searchType);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
    GameManager::sharedState()->m_sceneEnum = 1;
}

void DailyLevelPage::refreshDailyPage()
{
    if (!m_dailyNode || m_dailyNode->m_needsDownloading)
        return;
    
    GJGameLevel* level = m_dailyNode->m_level;
    if (!level)
        return;
    
    GJGameLevel* savedLevel = GameLevelManager::sharedState()->getSavedLevel(level);
    if (!savedLevel)
        savedLevel = level;
    
    DailyLevelNode* oldNode = m_dailyNode;
    m_dailyNode = nullptr;
    createDailyNode(savedLevel, true, 0.0f, false);
    oldNode->removeMeAndCleanup();
}

void DailyLevelPage::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void DailyLevelPage::show()
{
    if (!m_noElasticity)
    {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint targetPos = m_mainLayer->getPosition();
        
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, targetPos.y));
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
            cocos2d::CCMoveTo::create(0.5f, targetPos), 0.6f));
        
        cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
        if (!m_scene)
            m_scene = scene;
        if (!m_ZOrder)
            m_ZOrder = 105;
        
        m_scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    }
    else
    {
        FLAlertLayer::show();
    }
}

void DailyLevelPage::skipDailyLevel(DailyLevelNode* node, GJGameLevel* level)
{
    if (m_dailyNode != node)
        return;
    
    int dailyID = level->m_dailyID.random - level->m_dailyID.seed;
    int activeID = GameLevelManager::sharedState()->getActiveDailyID(m_type);
    
    if (activeID == dailyID && activeID < GameLevelManager::sharedState()->getDailyID(m_type))
    {
        exitDailyNode(m_dailyNode, 0.0f);
        m_downloadStarted = true;
        GameLevelManager::sharedState()->downloadLevel(m_downloadLevelID, false, false);
        
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.5f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(DailyLevelPage::createNodeIfLoaded)),
            nullptr
        ));
    }
}

void DailyLevelPage::tryGetDailyStatus()
{
    if (m_gettingDailyStatus)
    {
        m_timeCircle->setVisible(false);
        m_nodeCircle->setVisible(false);
    }
    else
    {
        m_gettingDailyStatus = true;
        GameLevelManager::sharedState()->getGJDailyLevelState(m_type);
    }
}

void DailyLevelPage::updateTimers(float dt)
{
    if (!GameLevelManager::sharedState()->hasDailyStateBeenLoaded(m_type))
        return;
    
    int timeLeft = getDailyTime();
    std::string timeStr = getDailyTimeString(timeLeft);
    
    if (m_type != Event)
    {
        m_timeLabel->setString(timeStr.c_str());
        if (m_dailyNode)
            m_dailyNode->updateTimeLabel(timeStr);
    }
    
    if (timeLeft <= 0 && !m_gettingDailyStatus)
    {
        m_gettingDailyStatus = true;
        if (m_type != Event)
        {
            m_timeCircle->setVisible(true);
            m_timeLabel->setVisible(false);
        }
        GameLevelManager::sharedState()->resetDailyLevelState(m_type);
        tryGetDailyStatus();
        m_gettingDailyStatus = false;
    }
}

DailyLevelPage::~DailyLevelPage()
{
    // Destructor - base class handles cleanup
}