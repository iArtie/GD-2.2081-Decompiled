#include "RewardsPage.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameStatsManager.h"
#include "GameLevelManager.h"
#include "GameToolbox.h"
#include "RewardUnlockLayer.h"
#include "FollowRewardPage.h"
#include "GJRewardItem.h"

RewardsPage* RewardsPage::create()
{
    RewardsPage* returnVal = new RewardsPage();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

const char* RewardsPage::getRewardFrame(int type, int state)
{
    return cocos2d::CCString::createWithFormat("chest_%02d_%02d_001.png", type, state)->getCString();
}

bool RewardsPage::init()
{
    FLAlertLayer::incrementForcePrio();

    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png");
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(340.0f, 220.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCSprite* title = cocos2d::CCSprite::createWithSpriteFrameName("rewardsLabel_001.png");
    m_mainLayer->addChild(title, 20);
    title->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 110.0f - 32.0f));

    // Esquinas decorativas
    for (int i = 1; i <= 4; i++)
    {
        cocos2d::CCSprite* corner = cocos2d::CCSprite::createWithSpriteFrameName("rewardCorner_001.png");
        m_mainLayer->addChild(corner, 8);

        float cornerX, cornerY;
        bool flipX = false, flipY = false;

        switch (i)
        {
            case 1: cornerX = -145.0f; cornerY = -85.0f; break;
            case 2: cornerX = -145.0f; cornerY =  85.0f; flipY = true; break;
            case 3: cornerX =  145.0f; cornerY =  85.0f; flipX = true; flipY = true; break;
            case 4: cornerX =  145.0f; cornerY = -85.0f; flipX = true; break;
        }

        if (flipX) corner->setFlipX(true);
        if (flipY) corner->setFlipY(true);

        corner->setPosition(cocos2d::CCPoint(
            winSize.width * 0.5f + cornerX,
            winSize.height * 0.5f + cornerY));
    }

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(RewardsPage::onClose));
    closeBtn->setSizeMult(1.6f);

    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 170.0f + 10.0f, winSize.height * 0.5f + 110.0f - 10.0f)));

    // ¿Mostrar botón "Free Stuff"?
    GameStatsManager* gsm = GameStatsManager::sharedState();

    bool chest7 = gsm->isSpecialChestUnlocked("0007");
    bool chest8 = gsm->isSpecialChestUnlocked("0008");
    bool chest9 = gsm->isSpecialChestUnlocked("0009");
    bool chest10 = gsm->isSpecialChestUnlocked("0010");
    bool chest11 = gsm->isSpecialChestUnlocked("0011");
    bool chest24 = gsm->isSpecialChestUnlocked("0024");

    bool showFreeStuff = !(chest7 && chest8 && chest9 && chest10 && chest11 && chest24);

    if (showFreeStuff)
    {
        cocos2d::CCSprite* freeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_freeChestBtn_001.png");
        CCMenuItemSpriteExtra* freeBtn = CCMenuItemSpriteExtra::create(freeSprite, nullptr, menu_selector(RewardsPage::onFreeStuff));
        freeBtn->setSizeMult(1.6f);
        m_buttonMenu->addChild(freeBtn);
        freeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(winSize.width * 0.5f + 170.0f - 10.0f, winSize.height * 0.5f + 110.0f - 10.0f)));

        cocos2d::CCSequence* pulse = cocos2d::CCSequence::create(
            cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.5f, 1.1f), 2.0f),
            cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f), 2.0f),
            nullptr);
        freeSprite->runAction(cocos2d::CCRepeatForever::create(pulse));
    }

    m_mainLayer->addChild(m_buttonMenu, 10);

    // Labels de tiempo
    cocos2d::CCPoint labelsBase(winSize.width * 0.5f, winSize.height * 0.5f - 24.0f);

    m_leftLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_leftLabel, 20);
    m_leftLabel->setScale(0.5f);
    m_leftLabel->setPosition(labelsBase + cocos2d::CCPoint(-70.0f, -50.0f));

    m_rightLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_rightLabel, 20);
    m_rightLabel->setScale(0.5f);
    m_rightLabel->setPosition(labelsBase + cocos2d::CCPoint(70.0f, -50.0f));

    // Cofre derecho (Large)
    cocos2d::CCSprite* rightChestSprite = cocos2d::CCSprite::createWithSpriteFrameName(getRewardFrame(2, 1));
    m_rightChest = CCMenuItemSpriteExtra::create(rightChestSprite, nullptr, menu_selector(RewardsPage::onReward));
    m_rightChest->setTag(2);
    m_buttonMenu->addChild(m_rightChest);

    cocos2d::CCPoint rightChestWorld = m_mainLayer->convertToWorldSpace(labelsBase + cocos2d::CCPoint(70.0f, 14.0f));
    m_rightChest->setPosition(m_buttonMenu->convertToNodeSpace(rightChestWorld));
    m_rightChest->m_scaleMultiplier = 1.08f;

    // Cofre izquierdo (Small)
    cocos2d::CCSprite* leftChestSprite = cocos2d::CCSprite::createWithSpriteFrameName(getRewardFrame(1, 1));
    float leftOffset = rightChestSprite->getContentSize().height * 0.2f;

    m_leftChest = CCMenuItemSpriteExtra::create(leftChestSprite, nullptr, menu_selector(RewardsPage::onReward));
    m_leftChest->setTag(1);
    m_buttonMenu->addChild(m_leftChest);

    cocos2d::CCPoint leftChestWorld = m_mainLayer->convertToWorldSpace(
        labelsBase + cocos2d::CCPoint(-70.0f, 14.0f - leftOffset * 0.5f));
    m_leftChest->setPosition(m_buttonMenu->convertToNodeSpace(leftChestWorld));
    m_leftChest->m_scaleMultiplier = 1.08f;

    GameLevelManager::sharedState()->m_GJRewardDelegate = this;

    if (GameStatsManager::sharedState()->areRewardsLoaded())
    {
        updateTimers(0.0f);
    }
    else
    {
        m_leftLabel->setVisible(false);
        m_rightLabel->setVisible(false);
        GameLevelManager::sharedState()->getGJRewards(0);
    }

    schedule(schedule_selector(RewardsPage::updateTimers));

    return true;
}

void RewardsPage::keyBackClicked()
{
    onClose(nullptr);
}

void RewardsPage::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void RewardsPage::onFreeStuff(cocos2d::CCObject* sender)
{
    FollowRewardPage* page = FollowRewardPage::create();
    page->m_scene = this;
    page->show();
}

void RewardsPage::onReward(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    bool isReady;

    if (tag == 1)
        isReady = m_leftOpen;
    else
        isReady = m_rightOpen;

    if (!isReady)
    {
        cocos2d::CCLabelBMFont* targetLabel = (tag == 1) ? m_leftLabel : m_rightLabel;
        if (targetLabel)
        {
            targetLabel->stopAllActions();
            targetLabel->setColor(cocos2d::ccc3(255, 50, 50));
            targetLabel->runAction(cocos2d::CCTintTo::create(1.0f, 255, 255, 255));
        }
        return;
    }

    if (m_openLayer)
        return;

    m_openLayer = RewardUnlockLayer::create(tag, this);
    addChild(m_openLayer);
    GameLevelManager::sharedState()->getGJRewards(tag);
}

void RewardsPage::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void RewardsPage::rewardsStatusFailed()
{
    if (!GameStatsManager::sharedState()->areRewardsLoaded())
        runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(RewardsPage::tryGetRewards)),
            nullptr));
}

void RewardsPage::rewardsStatusFinished(int type)
{
    if (m_openLayer && (unsigned int)(type - 1) <= 1)
    {
        GJRewardItem* rewardItem = GameStatsManager::sharedState()->getRewardItem((GJRewardType)type);

        if (rewardItem
            && !GameStatsManager::sharedState()->hasRewardBeenCollected((GJRewardType)type, rewardItem->m_chestID)
            && m_openLayer->readyToCollect(rewardItem))
        {
            GameStatsManager::sharedState()->preProcessReward(rewardItem);

            if (m_openLayer->showCollectReward(rewardItem))
                GameStatsManager::sharedState()->collectReward((GJRewardType)type, rewardItem);
        }
    }

    updateTimers(0.0f);
    m_leftLabel->setVisible(true);
    m_rightLabel->setVisible(true);

    unschedule(schedule_selector(RewardsPage::updateTimers));
    schedule(schedule_selector(RewardsPage::updateTimers));
}

void RewardsPage::show()
{
    if (m_noElasticity)
    {
        FLAlertLayer::show();
        return;
    }

    unsigned __int8 opacity = getOpacity();

    m_mainLayer->setScale(0.1f);
    m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
        cocos2d::CCScaleTo::create(0.3f, 1.0f), 1.6f));

    cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;

    if (!m_scene)
    {
        int highestZ = cocos2d::CCScene::getHighestChildZ(scene);
        m_ZOrder = (highestZ > 104) ? (highestZ + 1) : 105;
        m_scene = scene;
    }
    else if (!m_ZOrder)
    {
        m_ZOrder = 105;
    }

    m_scene->addChild(this, m_ZOrder);
    setOpacity(0);
    runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
    setVisible(true);
}

void RewardsPage::tryGetRewards()
{
    GameLevelManager::sharedState()->getGJRewards(0);
}

void RewardsPage::unlockLayerClosed(RewardUnlockLayer* layer)
{
    if (m_openLayer == layer)
        m_openLayer = nullptr;
}

void RewardsPage::updateTimers(float dt)
{
    if (!GameStatsManager::sharedState()->areRewardsLoaded())
        return;

    double now = (double)time(nullptr);

    GJRewardItem* smallItem = GameStatsManager::sharedState()->getRewardItem(GJRewardType::Small);
    GJRewardItem* largeItem = GameStatsManager::sharedState()->getRewardItem(GJRewardType::Large);

    int smallTimeRemaining = (int)((double)smallItem->m_timeRemaining - now);
    int largeTimeRemaining = (int)((double)largeItem->m_timeRemaining - now);

    bool smallReady;
    bool largeReady;

    if (smallTimeRemaining <= 0)
        smallReady = true;
    else
        smallReady = !GameStatsManager::sharedState()->hasRewardBeenCollected(GJRewardType::Small, smallItem->m_chestID);

    if (largeTimeRemaining <= 0)
        largeReady = true;
    else
        largeReady = !GameStatsManager::sharedState()->hasRewardBeenCollected(GJRewardType::Large, largeItem->m_chestID);

    if (smallReady)
        m_leftLabel->setString("Open");
    else
        m_leftLabel->setString(GameToolbox::getTimeString(smallTimeRemaining, false).c_str());

    if (largeReady)
        m_rightLabel->setString("Open");
    else
        m_rightLabel->setString(GameToolbox::getTimeString(largeTimeRemaining, false).c_str());

    if (m_leftOpen != smallReady)
    {
        m_leftOpen = smallReady;
        cocos2d::CCSprite* sprite = static_cast<cocos2d::CCSprite*>(m_leftChest->getNormalImage());
        sprite->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
            getRewardFrame(1, smallReady ? 2 : 1)));
    }

    if (m_rightOpen != largeReady)
    {
        m_rightOpen = largeReady;
        cocos2d::CCSprite* sprite = static_cast<cocos2d::CCSprite*>(m_rightChest->getNormalImage());
        sprite->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
            getRewardFrame(2, largeReady ? 2 : 1)));
    }
}

RewardsPage::~RewardsPage()
{
}