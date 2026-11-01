#include "Layers/RewardUnlockLayer.h"
#include "Pages/RewardsPage.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/GJChestSprite.h"
#include "Models/GJRewardItem.h"
#include "Layers/CurrencyRewardLayer.h"
#include "Audio/FMODAudioEngine.h"

RewardUnlockLayer* RewardUnlockLayer::create(int type, RewardsPage* page)
{
    RewardUnlockLayer* returnVal = new RewardUnlockLayer();

    if (returnVal->init(type, page))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool RewardUnlockLayer::init(int type, RewardsPage* page)
{
    if (!FLAlertLayer::init(0))
        return false;

    m_backgroundObjects = cocos2d::CCArray::create();
    m_backgroundObjects->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    const char* bgName = (type > 5) ? "GJ_square06.png" : "GJ_square02.png";

    cocos2d::ccColor3B cornerColor = cocos2d::ccc3(255, 255, 255);
    float cornerHue = 0.0f;

    if (type == 6)
    {
        cornerColor = cocos2d::ccc3(13, 68, 104);
        cornerHue = 0.0f;
    }
    else if (type == 7)
    {
        cornerColor = cocos2d::ccc3(51, 101, 74);
        cornerHue = -50.0f;
    }
    else if (type == 8)
    {
        cornerColor = cocos2d::ccc3(73, 34, 94);
        cornerHue = 80.0f;
    }
    else if (type == 9)
    {
        cornerColor = cocos2d::ccc3(50, 37, 56);
        cornerHue = 0.0f;
    }

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(bgName);
    bg->setOpacity(0);
    bg->setContentSize(cocos2d::CCSize(340.0f, 220.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    bg->runAction(cocos2d::CCFadeIn::create(0.2f));
    bg->setColor(cornerColor);
    m_backgroundObjects->addObject(bg);

    if (type > 5)
    {
        cocos2d::extension::CCScale9Sprite* bg2 = cocos2d::extension::CCScale9Sprite::create("GJ_square07.png");
        bg2->setOpacity(0);
        bg2->setContentSize(cocos2d::CCSize(340.0f, 220.0f));
        m_mainLayer->addChild(bg2, -2);
        bg2->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
        bg2->runAction(cocos2d::CCFadeIn::create(0.2f));
        m_backgroundObjects->addObject(bg2);
    }

    runAction(cocos2d::CCFadeTo::create(0.2f, 150));

    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCSpriteWithHue* corner = cocos2d::CCSpriteWithHue::createWithSpriteFrameName("rewardCorner_001.png");
        corner->setColor(cornerColor);

        if (type > 5)
            corner->setHueDegrees(cornerHue);

        m_mainLayer->addChild(corner, 8);
        m_backgroundObjects->addObject(corner);

        float posX, posY;
        bool flipX = false, flipY = false;

        switch (i)
        {
            case 0: posX = -145.0f; posY = -85.0f; break;
            case 1: posX = -145.0f; posY =  85.0f; flipY = true; break;
            case 2: posX =  145.0f; posY =  85.0f; flipX = true; flipY = true; break;
            case 3: posX =  145.0f; posY = -85.0f; flipX = true; break;
        }

        if (flipX) corner->setScaleX(-corner->getScaleX());
        if (flipY) corner->setScaleY(-corner->getScaleY());

        corner->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + posX, winSize.height * 0.5f + posY));
        corner->runAction(cocos2d::CCFadeIn::create(0.2f));
    }

    m_rewardsPage = page;
    if (m_rewardsPage)
        m_rewardsPage->retain();

    m_chestType = type;

    m_chestSprite = GJChestSprite::create(type);
    m_mainLayer->addChild(m_chestSprite, 2);

    cocos2d::CCPoint chestTarget(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f);
    cocos2d::CCPoint chestStart = chestTarget + cocos2d::CCPoint(0.0f, winSize.height + m_chestSprite->getContentSize().height);
    m_chestSprite->setPosition(chestStart);

    cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(1.0f, chestTarget);
    m_chestSprite->runAction(cocos2d::CCEaseBounceOut::create(moveTo));

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.36f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::playDropSound)),
        nullptr));

    cocos2d::CCSprite* shadow = cocos2d::CCSprite::createWithSpriteFrameName("chest_shadow_001.png");
    float shadowY = (type == 1) ? -38.4f : -48.0f;
    float shadowScale = (type == 1) ? 0.8f : 1.0f;

    shadow->setPosition(chestTarget + cocos2d::CCPoint(0.0f, shadowY));
    m_mainLayer->addChild(shadow, 1);
    shadow->setOpacity(0);
    shadow->setScale(2.0f);
    shadow->runAction(cocos2d::CCFadeTo::create(0.4f, 130));
    shadow->runAction(cocos2d::CCEaseBounceOut::create(cocos2d::CCScaleTo::create(1.0f, shadowScale * 0.95f)));

    cocos2d::CCSequence* shakeSequence = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.2f),
        cocos2d::CCMoveBy::create(0.05f, cocos2d::CCPoint(-4.0f, 0.0f)),
        cocos2d::CCMoveBy::create(0.05f, cocos2d::CCPoint(4.0f, 0.0f)),
        cocos2d::CCMoveBy::create(0.05f, cocos2d::CCPoint(-4.0f, 0.0f)),
        cocos2d::CCMoveBy::create(0.05f, cocos2d::CCPoint(4.0f, 0.0f)),
        cocos2d::CCMoveBy::create(0.05f, cocos2d::CCPoint(-4.0f, 0.0f)),
        cocos2d::CCMoveBy::create(0.05f, cocos2d::CCPoint(4.0f, 0.0f)),
        cocos2d::CCDelayTime::create(0.2f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::step2)),
        nullptr);
    m_chestSprite->runAction(shakeSequence);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
    m_closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(RewardUnlockLayer::onClose));
    m_closeBtn->setSizeMult(1.6f);

    cocos2d::CCSprite* rewardSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png");
    m_rewardBtn = CCMenuItemSpriteExtra::create(rewardSprite, nullptr, menu_selector(RewardUnlockLayer::onClose));
    m_rewardBtn->setSizeMult(1.6f);

    m_buttonMenu = cocos2d::CCMenu::create(m_closeBtn, m_rewardBtn, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_closeBtn->setEnabled(false);
    m_closeBtn->setVisible(false);
    m_rewardBtn->setEnabled(false);
    m_rewardBtn->setVisible(false);

    m_closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 170.0f + 10.0f, winSize.height * 0.5f + 110.0f - 10.0f)));

    m_rewardBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 110.0f)));

    m_wrongLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_wrongLabel);
    m_wrongLabel->setScale(0.6f);
    m_wrongLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 90.0f));
    m_wrongLabel->setVisible(false);

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(10.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::connectionTimeout)),
        nullptr));

    return true;
}

void RewardUnlockLayer::connectionTimeout()
{
    if (m_rewardCollected)
        return;

    m_closeBtn->setVisible(true);
    m_closeBtn->setEnabled(true);
    m_wrongLabel->setVisible(true);
}

void RewardUnlockLayer::currencyWillExit(CurrencyRewardLayer* layer)
{
    m_buttonMenu->setVisible(true);
}

void RewardUnlockLayer::keyBackClicked()
{
}

void RewardUnlockLayer::labelEnterFinishedO(cocos2d::CCObject* object)
{
}

void RewardUnlockLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_rewardsPage)
        m_rewardsPage->unlockLayerClosed(this);

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void RewardUnlockLayer::playDropSound()
{
    FMODAudioEngine::sharedEngine()->playEffect("chestLand.ogg");
}

bool RewardUnlockLayer::readyToCollect(GJRewardItem* item)
{
    return item && item->m_rewardType == m_chestType;
}

void RewardUnlockLayer::showCloseButton()
{
    m_rewardBtn->setVisible(true);
    m_rewardBtn->setEnabled(true);

    cocos2d::CCNode* sprite = m_rewardBtn->getNormalImage();
    sprite->setOpacity(0);
    sprite->runAction(cocos2d::CCFadeIn::create(1.0f));
}

bool RewardUnlockLayer::showCollectReward(GJRewardItem* item)
{
    if (!item)
        return false;

    if (m_rewardItem)
        m_rewardItem->release();

    m_rewardItem = item;
    m_rewardItem->retain();

    if (item->m_unk && item->m_rewardType != m_chestType)
        return false;

    m_closeBtn->setVisible(false);
    m_closeBtn->setEnabled(false);
    m_wrongLabel->setVisible(false);
    m_rewardCollected = true;

    if (!m_animationPlayed)
        return true;

    playRewardEffect();
    return true;
}

void RewardUnlockLayer::step2()
{
    FMODAudioEngine::sharedEngine()->playEffect("chestOpen01.ogg");
    m_chestSprite->switchToState(ChestState::Opening, 0);

    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.3f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::step3)),
        nullptr));
}

void RewardUnlockLayer::step3()
{
    m_animationPlayed = true;

    if (m_rewardCollected)
        playRewardEffect();
}

RewardUnlockLayer::~RewardUnlockLayer()
{
}