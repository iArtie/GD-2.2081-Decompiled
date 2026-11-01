#include "PauseLayer.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "FMODAudioEngine.h"
#include "PlayLayer.h"
#include "GJGameLevel.h"
#include "GJGameLoadingLayer.h"
#include "GameOptionsLayer.h"
#include "TutorialLayer.h"
#include "FLAlertLayer.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "CCMenuItemSpriteExtra.h"
#include "AdToolbox.h"
#include "PlatformToolbox.h"
#include "cocos2d-ext/CCScale9Sprite.h"
#include <cstring>

PauseLayer* PauseLayer::create(bool unfocused)
{
    PauseLayer* returnVal = new PauseLayer();
    if (returnVal->init(unfocused))
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool PauseLayer::init(bool unfocused)
{
    m_unfocused = unfocused;
    return CCBlockLayer::init();
}

void PauseLayer::customSetup()
{
    setColor(cocos2d::ccc3(100, 100, 100));
    setOpacity(75);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    float width = (cocos2d::CCDirector::sharedDirector()->getScreenRight() - 20.0f) -
                  cocos2d::CCDirector::sharedDirector()->getScreenLeft();
    float height = (cocos2d::CCDirector::sharedDirector()->getScreenTop() - 20.0f) -
                   cocos2d::CCDirector::sharedDirector()->getScreenBottom();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "square04_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(width, height));
    addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    bg->setColor(cocos2d::ccc3(100, 100, 100));

    GameManager* gm = GameManager::sharedState();
    std::string levelName = gm->m_playLayer->m_level->m_levelName;

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        (winSize.height * 0.5f) + (winSize.height * 0.5f) - 20.0f));
    addChild(titleLabel);
    titleLabel->setVisible(true);

    setupProgressBars();

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    addChild(menu);

    cocos2d::CCSprite* menuSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");

    if (GameManager::sharedState()->m_playLayer->m_level->m_levelType == GJLevelType::Editor ||
        (gm->m_hasRP.random - gm->m_hasRP.seed > 0))
    {
        cocos2d::CCSprite* editSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
        float scale = menuSprite->getContentSize().width / editSprite->getContentSize().width;
        editSprite->setScale(scale);
        CCMenuItemSpriteExtra* editBtn = CCMenuItemSpriteExtra::create(
            editSprite, nullptr, menu_selector(PauseLayer::onTryEdit));
        menu->addChild(editBtn);
    }

    if (GameManager::sharedState()->m_playLayer->m_levelSettings->m_platformerMode)
    {
        cocos2d::CCSprite* replayFullSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayFullBtn_001.png");
        CCMenuItemSpriteExtra* replayFullBtn = CCMenuItemSpriteExtra::create(
            replayFullSprite, nullptr, menu_selector(PauseLayer::onRestartFull));
        menu->addChild(replayFullBtn);
    }

    cocos2d::CCSprite* modeSprite;
    cocos2d::SEL_MenuHandler modeHandler;
    if (GameManager::sharedState()->m_playLayer->m_isPracticeMode)
    {
        modeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_normalBtn_001.png");
        modeHandler = menu_selector(PauseLayer::onNormalMode);
    }
    else
    {
        modeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_practiceBtn_001.png");
        modeHandler = menu_selector(PauseLayer::onPracticeMode);
    }
    CCMenuItemSpriteExtra* modeBtn = CCMenuItemSpriteExtra::create(modeSprite, nullptr, modeHandler);
    menu->addChild(modeBtn);

    cocos2d::CCSprite* playSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    CCMenuItemSpriteExtra* resumeBtn = CCMenuItemSpriteExtra::create(
        playSprite, nullptr, menu_selector(PauseLayer::onResume));
    menu->addChild(resumeBtn);

    CCMenuItemSpriteExtra* quitBtn = CCMenuItemSpriteExtra::create(
        menuSprite, nullptr, menu_selector(PauseLayer::tryQuit));
    menu->addChild(quitBtn);

    cocos2d::CCSprite* replaySprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    CCMenuItemSpriteExtra* restartBtn = CCMenuItemSpriteExtra::create(
        replaySprite, nullptr, menu_selector(PauseLayer::onRestart));
    menu->addChild(restartBtn);

    float padding = (GameManager::sharedState()->m_playLayer->m_level->m_levelType == GJLevelType::Editor) ? 10.0f : 15.0f;
    menu->alignItemsHorizontallyWithPadding(padding);

    cocos2d::CCPoint menuPos = menu->getPosition();
    menu->setPosition(cocos2d::CCPoint(menuPos.x, (winSize.height * 0.5f) - 30.0f));

    if (!GameManager::sharedState()->m_clickedPractice &&
        GameManager::sharedState()->m_playLayer->m_level->m_levelType != GJLevelType::Editor &&
        !GameManager::sharedState()->m_playLayer->m_levelSettings->m_platformerMode)
    {
        cocos2d::CCSprite* practiceTxt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_practiceTxt_001.png");
        addChild(practiceTxt);
        practiceTxt->setPosition(cocos2d::CCPoint(
            (winSize.width * 0.5f) - 190.0f,
            (winSize.height * 0.5f) - 12.0f));
    }

    cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optionsSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(
        optionsSprite, nullptr, menu_selector(PauseLayer::onSettings));
    menu->addChild(optionsBtn);

    cocos2d::CCPoint optionsPos = cocos2d::CCNode::convertToNodeSpace(
        menu,
        cocos2d::CCPoint(
            (width * 0.5f) + (winSize.width * 0.5f) - 26.0f,
            (height * 0.5f) + (winSize.height * 0.5f) - 26.0f));
    optionsBtn->setPosition(optionsPos);

    cocos2d::CCPoint sliderBase = cocos2d::CCPoint(
        winSize.width * 0.5f,
        (winSize.height * 0.5f) - 105.0f);

    Slider* musicSlider = Slider::create(this, menu_selector(PauseLayer::musicSliderChanged), 0, 0.8f);
    musicSlider->setBarVisibility(true);
    addChild(musicSlider);
    musicSlider->setPosition(sliderBase + cocos2d::CCPoint(-100.0f, 0.0f));
    musicSlider->setValue(FMODAudioEngine::sharedEngine()->getBackgroundMusicVolume());

    Slider* sfxSlider = Slider::create(this, menu_selector(PauseLayer::sfxSliderChanged), 0, 0.8f);
    sfxSlider->setBarVisibility(true);
    addChild(sfxSlider);
    sfxSlider->setPosition(sliderBase + cocos2d::CCPoint(100.0f, 0.0f));
    sfxSlider->setValue(FMODAudioEngine::sharedEngine()->getEffectsVolume());

    cocos2d::CCLabelBMFont* musicLabel = cocos2d::CCLabelBMFont::create("Music", "bigFont.fnt");
    addChild(musicLabel, 1);
    musicLabel->setPosition(musicSlider->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
    musicLabel->setScale(0.5f);

    cocos2d::CCLabelBMFont* sfxLabel = cocos2d::CCLabelBMFont::create("SFX", "bigFont.fnt");
    addChild(sfxLabel, 1);
    sfxLabel->setPosition(sfxSlider->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
    sfxLabel->setScale(0.5f);

    if (PlatformToolbox::isControllerConnected())
    {
        cocos2d::CCSprite* startBtn = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        addChild(startBtn);
        cocos2d::CCPoint resumeWorld = resumeBtn->getParent()->convertToWorldSpace(resumeBtn->getPosition());
        cocos2d::CCPoint resumeLocal = convertToNodeSpace(resumeWorld);
        startBtn->setPosition(resumeLocal + cocos2d::CCPoint(0.0f, -40.0f));

        cocos2d::CCSprite* bBtn = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_B_001.png");
        addChild(bBtn);
        cocos2d::CCPoint quitWorld = quitBtn->getParent()->convertToWorldSpace(quitBtn->getPosition());
        cocos2d::CCPoint quitLocal = convertToNodeSpace(quitWorld);
        bBtn->setPosition(quitLocal + cocos2d::CCPoint(0.0f, -30.0f));
        bBtn->setScale(0.8f);

        cocos2d::CCSprite* xBtn = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_X_001.png");
        addChild(xBtn);
        cocos2d::CCPoint modeWorld = modeBtn->getParent()->convertToWorldSpace(modeBtn->getPosition());
        cocos2d::CCPoint modeLocal = convertToNodeSpace(modeWorld);
        xBtn->setPosition(modeLocal + cocos2d::CCPoint(0.0f, -30.0f));
        xBtn->setScale(0.8f);
    }
}

void PauseLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    int tag = layer->getTag();
    if (tag)
    {
        if (tag == 1 && btn2)
            onEdit(nullptr);
    }
    else if (btn2)
    {
        onQuit(nullptr);
    }
    else
    {
        m_tryingQuit = false;
    }
}

void PauseLayer::goEdit()
{
    CCBlockLayer::decrementForcePrio();

    GJGameLevel* level = GameManager::sharedState()->m_playLayer->m_level;
    PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;

    GJGameLoadingLayer* loadingLayer = GJGameLoadingLayer::transitionToLoadingLayer(level, true);

    if (loadingLayer != playLayer->m_loadingLayer)
    {
        if (loadingLayer)
        {
            loadingLayer->retain();
            playLayer->m_loadingLayer = loadingLayer;
        }
    }
}

void PauseLayer::keyBackClicked()
{
    if (!m_tryingQuit)
    {
        m_tryingQuit = true;
        tryQuit(nullptr);
    }
}

void PauseLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == cocos2d::enumKeyCodes::CONTROLLER_B)
    {
        tryQuit(nullptr);
        return;
    }

    if (key <= cocos2d::enumKeyCodes::CONTROLLER_B)
    {
        if (key == cocos2d::enumKeyCodes::KEY_Escape)
        {
            tryQuit(nullptr);
            return;
        }
        if (key == cocos2d::enumKeyCodes::KEY_Space)
        {
            onResume(nullptr);
            return;
        }
        return;
    }

    if (key == cocos2d::enumKeyCodes::CONTROLLER_X)
    {
        if (GameManager::sharedState()->m_playLayer->m_isPracticeMode)
            onNormalMode(nullptr);
        else
            onPracticeMode(nullptr);
        return;
    }

    if (key == cocos2d::enumKeyCodes::CONTROLLER_Start)
    {
        onResume(nullptr);
        return;
    }
}

void PauseLayer::keyUp(cocos2d::enumKeyCodes key, double timestamp)
{
}

void PauseLayer::musicSliderChanged(cocos2d::CCObject* sender)
{
    float value = SliderThumb::getValue((SliderThumb*)sender);
    if (value < 0.03f)
        value = 0.0f;
    FMODAudioEngine::sharedEngine()->setBackgroundMusicVolume(value);
}

void PauseLayer::onEdit(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->m_playLayer->m_level->m_levelType == GJLevelType::Editor ||
        (GameManager::sharedState()->m_hasRP.random - GameManager::sharedState()->m_hasRP.seed > 0))
    {
        cocos2d::CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
        PlayLayer::stopRecording(GameManager::sharedState()->m_playLayer);
        GameManager::sharedState()->m_playLayer->stopAllActions();
        GameManager::sharedState()->m_playLayer->unscheduleAllSelectors();
        GameManager::sharedState()->m_playLayer->resetAudio();

        cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.0f);
        cocos2d::CCCallFunc* call = cocos2d::CCCallFunc::create(this, callfunc_selector(PauseLayer::goEdit));
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, call, nullptr);
        runAction(seq);
    }
}

void PauseLayer::onHelp(cocos2d::CCObject* sender)
{
    TutorialLayer* tutorialLayer = TutorialLayer::create();
    tutorialLayer->show();
}

void PauseLayer::onNormalMode(cocos2d::CCObject* sender)
{
    PlayLayer::togglePracticeMode(GameManager::sharedState()->m_playLayer, false);
    onResume(nullptr);
}

void PauseLayer::onPracticeMode(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_clickedPractice = true;
    PlayLayer::togglePracticeMode(GameManager::sharedState()->m_playLayer, true);
    onResume(nullptr);
}

void PauseLayer::onQuit(cocos2d::CCObject* sender)
{
    CCBlockLayer::decrementForcePrio();
    unscheduleAllSelectors();
    PlayLayer::onQuit(GameManager::sharedState()->m_playLayer);

    FMODAudioEngine::sharedEngine()->playEffect("quitSound_01.ogg", 1.0f, 0.0f, 0.7f);
}

void PauseLayer::onRecordReplays(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_recordGameplay = !GameManager::sharedState()->m_recordGameplay;
}

void PauseLayer::onReplay(cocos2d::CCObject* sender)
{
}

void PauseLayer::onRestart(cocos2d::CCObject* sender)
{
    CCBlockLayer::decrementForcePrio();
    PlayLayer::resumeAndRestart(GameManager::sharedState()->m_playLayer, false);
    removeMeAndCleanup();
}

void PauseLayer::onRestartFull(cocos2d::CCObject* sender)
{
    CCBlockLayer::decrementForcePrio();
    PlayLayer::resumeAndRestart(GameManager::sharedState()->m_playLayer, true);
    removeMeAndCleanup();
}

void PauseLayer::onResume(cocos2d::CCObject* sender)
{
    CCBlockLayer::decrementForcePrio();
    PlayLayer::resume(GameManager::sharedState()->m_playLayer);
    removeMeAndCleanup();
}

void PauseLayer::onSettings(cocos2d::CCObject* sender)
{
    GameOptionsLayer* layer = GameOptionsLayer::create(GameManager::sharedState()->m_playLayer);
    layer->show();
}

void PauseLayer::onTime(cocos2d::CCObject* sender)
{
}

void PauseLayer::onTryEdit(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->m_playLayer->m_level->m_levelType == GJLevelType::Editor)
    {
        onEdit(sender);
    }
    else
    {
        std::string text = "Are you sure you want to <cg>open</c> the editor?\n<cr>(Read-Only)</c>";
        FLAlertLayer* alert = FLAlertLayer::create(this, "Edit Level", text);
        alert->setTag(1);
        alert->show();
    }
}

void PauseLayer::setupProgressBars()
{
    GJGameLevel* level = GameManager::sharedState()->m_playLayer->m_level;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    if (GJGameLevel::isPlatformer(level))
    {
        int timeMilli = GameManager::sharedState()->m_playLayer->getPlayTimerMilli();

        int hours = timeMilli / 1000 / 3600;
        int minutes = timeMilli / 1000 / 60 % 60;
        int seconds = timeMilli / 1000 % 60;
        int ms = timeMilli % 1000;

        std::string timeStr;
        char buffer[32];
        if (minutes | hours)
        {
            if (hours)
                snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d", hours, minutes, seconds, ms);
            else
                snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", minutes, seconds, ms);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "%i.%03d", seconds, ms);
        }
        timeStr = buffer;

        std::string timeLabel = "Time: " + timeStr;
        cocos2d::CCLabelBMFont* timeBMFont = cocos2d::CCLabelBMFont::create(timeLabel.c_str(), "bigFont.fnt");
        addChild(timeBMFont, 3);
        timeBMFont->setPosition(cocos2d::CCPoint(
            winSize.width * 0.5f,
            (winSize.height * 0.5f) + 85.0f));
        timeBMFont->limitLabelWidth(150.0f, 0.8f, 0.0f);

        if (GameManager::sharedState()->m_playLayer->m_gameState.m_unkBool32)
        {
            std::string pointsStr = "Points: " + GameToolbox::pointsToString(
                GameManager::sharedState()->m_playLayer->m_gameState.m_points);

            cocos2d::CCLabelBMFont* pointsLabel = cocos2d::CCLabelBMFont::create(pointsStr.c_str(), "bigFont.fnt");
            addChild(pointsLabel, 3);
            pointsLabel->setPosition(timeBMFont->getPosition() + cocos2d::CCPoint(0.0f, -35.0f));
            pointsLabel->limitLabelWidth(150.0f, 0.8f, 0.0f);
        }
    }
    else
    {
        cocos2d::CCSprite* bar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
        bar->setColor(cocos2d::ccc3(0, 0, 0));
        bar->setOpacity(125);
        bar->setScale(1.0f);
        addChild(bar, 3);
        bar->setPosition(cocos2d::CCPoint(
            winSize.width * 0.5f,
            (winSize.height * 0.5f) + 80.0f));

        cocos2d::CCSprite* normalBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
        normalBar->setScale(0.992f);
        normalBar->setScaleY(0.86f);
        normalBar->setColor(cocos2d::ccc3(0, 255, 255));

        float normalBarOffset = normalBar->getContentSize().width - (normalBar->getContentSize().width * 0.992f);
        bar->addChild(normalBar, 1);
        normalBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        normalBar->setPosition(cocos2d::CCPoint(
            normalBarOffset * 0.5f,
            bar->getContentSize().height * 0.5f));

        float normalBarWidth = normalBar->getContentSize().width;
        float normalPct = (float)GJGameLevel::getNormalPercent(level) / 100.0f;
        float cropWidth = (normalBarWidth > (normalBarWidth * normalPct)) ? (normalBarWidth * normalPct) : normalBarWidth;

        normalBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, cropWidth, normalBar->getContentSize().height));

        cocos2d::CCSprite* practiceBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
        practiceBar->setColor(cocos2d::ccc3(0, 0, 0));
        practiceBar->setOpacity(125);
        practiceBar->setScale(1.0f);
        addChild(practiceBar, 3);
        practiceBar->setPosition(bar->getPosition() + cocos2d::CCPoint(0.0f, -50.0f));

        cocos2d::CCSprite* practiceBarFill = cocos2d::CCSprite::create("GJ_progressBar_001.png");
        practiceBarFill->setScale(0.992f);
        practiceBarFill->setScaleY(0.86f);
        practiceBarFill->setColor(cocos2d::ccc3(255, 255, 0));

        practiceBar->addChild(practiceBarFill, 1);
        practiceBarFill->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        practiceBarFill->setPosition(normalBar->getPosition());

        float practiceBarWidth = practiceBarFill->getContentSize().width;
        float practicePct = (float)level->m_practicePercent / 100.0f;
        float practiceCropWidth = (practiceBarWidth > (practiceBarWidth * practicePct)) ? (practiceBarWidth * practicePct) : practiceBarWidth;

        practiceBarFill->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, practiceCropWidth, practiceBarFill->getContentSize().height));

        int normalPercent = GJGameLevel::getNormalPercent(level);
        std::string normalPercentStr = std::to_string(normalPercent) + "%";
        cocos2d::CCLabelBMFont* normalLabel = cocos2d::CCLabelBMFont::create(normalPercentStr.c_str(), "bigFont.fnt");
        addChild(normalLabel, 4);
        normalLabel->setPosition(bar->getPosition());
        normalLabel->setScale(0.5f);

        std::string practicePercentStr = std::to_string(level->m_practicePercent) + "%";
        cocos2d::CCLabelBMFont* practiceLabel = cocos2d::CCLabelBMFont::create(practicePercentStr.c_str(), "bigFont.fnt");
        addChild(practiceLabel, 4);
        practiceLabel->setPosition(practiceBar->getPosition());
        practiceLabel->setScale(0.5f);

        cocos2d::CCLabelBMFont* normalTitle = cocos2d::CCLabelBMFont::create("Normal", "bigFont.fnt");
        addChild(normalTitle, 4);
        normalTitle->setPosition(bar->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
        normalTitle->setScale(0.5f);

        cocos2d::CCLabelBMFont* practiceTitle = cocos2d::CCLabelBMFont::create("Practice", "bigFont.fnt");
        addChild(practiceTitle, 4);
        practiceTitle->setPosition(practiceBar->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
        practiceTitle->setScale(0.5f);
    }
}

void PauseLayer::sfxSliderChanged(cocos2d::CCObject* sender)
{
    float value = SliderThumb::getValue((SliderThumb*)sender);
    if (value < 0.03f)
        value = 0.0f;
    FMODAudioEngine::sharedEngine()->setEffectsVolume(value);
}

void PauseLayer::tryQuit(cocos2d::CCObject* sender)
{
    if (GameManager::sharedState()->getGameVariable("0167"))
    {
        std::string text = "Are you sure you want to <cr>exit</c>?";
        FLAlertLayer* alert = FLAlertLayer::create(this, "Exit Level", text);
        alert->setTag(0);
        alert->show();
    }
    else
    {
        onQuit(sender);
    }
}

void PauseLayer::tryShowBanner(float dt)
{
    AdToolbox::enableBannerNoRefresh();
    unschedule(schedule_selector(PauseLayer::tryShowBanner));
}

PauseLayer::~PauseLayer()
{
}