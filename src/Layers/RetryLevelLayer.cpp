#include "Layers/RetryLevelLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/ButtonSprite.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Utils/PlatformToolbox.h"
#include "Utils/PromoInterstitial.h"
#include "Utils/AdToolbox.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/PlayLayer.h"
#include "Models/DialogObject.h"
#include "Layers/DialogLayer.h"

RetryLevelLayer* RetryLevelLayer::create()
{
    RetryLevelLayer* returnVal = new RetryLevelLayer();

    if (GJDropDownLayer::init(" ", 230.0f, true))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

void RetryLevelLayer::customSetup()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    GameManager* gm = GameManager::sharedState();
    std::string levelName = gm->m_playLayer->m_level->m_levelName;

    cocos2d::CCLabelBMFont* levelLabel = cocos2d::CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
    levelLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 80.0f - 15.0f));
    m_mainLayer->addChild(levelLabel);
    levelLabel->limitLabelWidth(320.0f, 0.0f, 0.0f);

    setupLastProgress();

    m_mainLayer->removeChildByTag(0, true);
    m_mainLayer->removeChildByTag(1, true);
    m_buttonMenu->setVisible(false);

    m_endPosition = m_endPosition + cocos2d::CCPoint(0.0f, 5.0f);

    cocos2d::CCPoint statsPos(winSize.width * 0.5f, winSize.height * 0.5f - 10.0f - 15.0f);

    cocos2d::CCLabelBMFont* jumpsLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("Jumps: %i", gm->m_playLayer->m_jumps)->getCString(), "bigFont.fnt");
    m_mainLayer->addChild(jumpsLabel, 3);
    jumpsLabel->setPosition(statsPos);
    jumpsLabel->setScale(0.8f);

    int totalTime = (int)gm->m_playLayer->m_gameState.m_totalTime;
    int seconds = totalTime % 60;
    int minutes = totalTime / 60 % 60;

    std::string timeStr;
    if (totalTime / 3600)
        timeStr = cocos2d::CCString::createWithFormat("%02d:%02d:%02d", totalTime / 3600, minutes, seconds)->getCString();
    else
        timeStr = cocos2d::CCString::createWithFormat("%02d:%02d", minutes, seconds)->getCString();

    cocos2d::CCLabelBMFont* timeLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("Time: %s", timeStr.c_str())->getCString(), "bigFont.fnt");
    m_mainLayer->addChild(timeLabel, 3);
    timeLabel->setPosition(jumpsLabel->getPosition() + cocos2d::CCPoint(0.0f, -24.0f));
    timeLabel->setScale(0.8f);

    m_mainMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_mainMenu, 1);

    cocos2d::CCPoint buttonsWorld = m_mainLayer->convertToWorldSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 115.0f - 10.0f));
    cocos2d::CCPoint buttonsNode = m_mainMenu->convertToNodeSpace(buttonsWorld);

    cocos2d::CCSprite* replaySprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    CCMenuItemSpriteExtra* replayBtn = CCMenuItemSpriteExtra::create(replaySprite, nullptr, menu_selector(RetryLevelLayer::onReplay));
    m_mainMenu->addChild(replayBtn);
    replayBtn->setPosition(buttonsNode + cocos2d::CCPoint(-100.0f, 0.0f));

    cocos2d::CCSprite* menuSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
    CCMenuItemSpriteExtra* menuBtn = CCMenuItemSpriteExtra::create(menuSprite, nullptr, menu_selector(RetryLevelLayer::onMenu));
    m_mainMenu->addChild(menuBtn);
    menuBtn->setPosition(buttonsNode + cocos2d::CCPoint(100.0f, 0.0f));

    m_mainLayer->setPosition(m_endPosition);

    if (PlatformToolbox::isControllerConnected())
    {
        cocos2d::CCSprite* startIcon = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        m_mainLayer->addChild(startIcon, 10);

        cocos2d::CCPoint replayWorld = replayBtn->getParent()->convertToWorldSpace(replayBtn->getPosition());
        cocos2d::CCPoint replayNode = m_mainLayer->convertToNodeSpace(replayWorld);
        startIcon->setPosition(replayNode + cocos2d::CCPoint(0.0f, -25.0f));

        cocos2d::CCSprite* bIcon = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_B_001.png");
        m_mainLayer->addChild(bIcon, 10);

        cocos2d::CCPoint menuWorld = menuBtn->getParent()->convertToWorldSpace(menuBtn->getPosition());
        cocos2d::CCPoint menuNode = m_mainLayer->convertToNodeSpace(menuWorld);
        bIcon->setPosition(menuNode + cocos2d::CCPoint(0.0f, -25.0f));
        bIcon->setScale(0.8f);
    }

    PlatformToolbox::tryShowRateDialog("Geometry Dash");
}

void RetryLevelLayer::enterAnimFinished()
{
    if (GameManager::sharedState()->m_recordGameplay && !GameManager::sharedState()->m_playLayer->m_isPracticeMode)
    {
        // GameManager::sharedState()->startRecording() // decompilado no muestra la llamada
    }
}

const char* RetryLevelLayer::getEndText()
{
    int value = rand() % 5;

    switch (value)
    {
        case 3: return "Well Done!";
        case 4: return "Impressive!";
        case 2: return "Good Job!";
    }

    return "Awesome!";
}

void RetryLevelLayer::keyBackClicked()
{
    if (m_exitingMenu)
        return;

    m_exitingMenu = true;
    onMenu(nullptr);
}

void RetryLevelLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == cocos2d::KEY_Space || key == cocos2d::CONTROLLER_A || key == cocos2d::CONTROLLER_Start)
    {
        onReplay(nullptr);
        return;
    }

    if (key == cocos2d::KEY_Escape || key == cocos2d::CONTROLLER_B)
    {
        onMenu(nullptr);
        return;
    }

    cocos2d::CCLayer::keyDown(key, timestamp);
}

void RetryLevelLayer::keyUp(cocos2d::enumKeyCodes key, double timestamp)
{
}

void RetryLevelLayer::onEveryplay(cocos2d::CCObject* sender)
{
}

void RetryLevelLayer::onFullVersion(cocos2d::CCObject* sender)
{
    AdToolbox::disableBanner();
    PromoInterstitial::create(true)->show();
}

void RetryLevelLayer::onMenu(cocos2d::CCObject* sender)
{
    PlayLayer::onQuit(GameManager::sharedState()->m_playLayer);
    exitLayer(false);
    FMODAudioEngine::sharedEngine()->playEffect("quitSound_01.ogg", 1.0f, 0.0f, 0.7f);
}

void RetryLevelLayer::onReplay(cocos2d::CCObject* sender)
{
    if (!GameManager::sharedState()->getGameVariable("0024"))
        PlatformToolbox::hideCursor();

    GameManager::sharedState()->m_playLayer->resetLevel();
    removeMeAndCleanup();
}

void RetryLevelLayer::onRewardedVideo(cocos2d::CCObject* sender)
{
    if (m_unk1e9 || !AdToolbox::hasCachedRewardedVideo())
        return;

    if (GameManager::sharedState()->canShowRewardedVideo())
    {
        m_unk1e9 = true;
        GameManager::sharedState()->m_rewardedVideoDelegate = this;
        AdToolbox::showRewardedVideo();
        return;
    }

    GameStatsManager::sharedState()->getNextVideoAdReward();

    // Scratch dialogs (no hay recompensa)
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    DialogObject* d1 = DialogObject::create("Scratch", "I don't have any <co>chests</c> left<d010>.<d010>.<d010>.", 12, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d1);

    DialogObject* d2 = DialogObject::create("Scratch", "You took <cr>EVERYTHING</c> I had!", 14, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d2);

    DialogObject* d3 = DialogObject::create("Scratch", "I need to find <cy>new stuff</c>...", 13, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d3);

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
    m_mainLayer->addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogChatPlacement::Center);
    dialogLayer->animateInRandomSide();
}

void RetryLevelLayer::rewardedVideoFinished()
{
    m_unk1e9 = false;
}

void RetryLevelLayer::setupLastProgress()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCSprite* barBG = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    barBG->setColor(cocos2d::ccc3(0, 0, 0));
    barBG->setOpacity(125);
    barBG->setScale(1.0f);
    m_mainLayer->addChild(barBG, 3);
    barBG->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 25.0f - 15.0f));

    cocos2d::CCSprite* barFill = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    barFill->setScaleX(0.992f);
    barFill->setScaleY(0.86f);
    barFill->setColor(cocos2d::ccc3(0, 255, 0));

    float barWidth = barFill->getContentSize().width;
    float offsetX = barWidth - barWidth * 0.992f;

    barBG->addChild(barFill, 1);
    barFill->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    barFill->setPosition(cocos2d::CCPoint(offsetX * 0.5f, barBG->getContentSize().height * 0.5f));

    int percent = GameManager::sharedState()->m_playLayer->m_lastAttemptPercent;
    float fillWidth = barWidth;
    if (barWidth > barWidth * (percent / 100.0f))
        fillWidth = barWidth * (percent / 100.0f);

    barFill->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, fillWidth, barFill->getContentSize().height));

    cocos2d::CCLabelBMFont* percentLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i%%", percent)->getCString(), "bigFont.fnt");
    m_mainLayer->addChild(percentLabel, 4);
    percentLabel->setPosition(barBG->getPosition());
    percentLabel->setScale(0.5f);

    cocos2d::CCLabelBMFont* attemptLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("Attempt %i", GameManager::sharedState()->m_playLayer->m_attempts)->getCString(),
        "bigFont.fnt");
    m_mainLayer->addChild(attemptLabel, 4);
    attemptLabel->setPosition(barBG->getPosition() + cocos2d::CCPoint(0.0f, 23.0f));
    attemptLabel->setScale(0.5f);
}

bool RetryLevelLayer::shouldOffsetRewardCurrency()
{
    return true;
}

void RetryLevelLayer::showLayer(bool instant)
{
    m_mainLayer->stopAllActions();
    layerVisible();

    if (instant)
    {
        m_mainLayer->setPosition(m_endPosition);
        setOpacity(100);
        enterAnimFinished();
        return;
    }

    m_mainLayer->setScale(0.1f);
    m_mainLayer->runAction(cocos2d::CCSequence::create(
        cocos2d::CCEaseElasticOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f), 0.6f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(RetryLevelLayer::enterAnimFinished)),
        nullptr));

    setOpacity(0);
    runAction(cocos2d::CCFadeTo::create(0.5f, 100));
}

RetryLevelLayer::~RetryLevelLayer()
{
}