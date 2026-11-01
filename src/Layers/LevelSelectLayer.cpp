#include "Layers/LevelSelectLayer.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Managers/AchievementManager.h"
#include "GameLevelManager.h"
#include "Models/GJGameLevel.h"
#include "Layers/GJGroundLayer.h"
#include "Layers/BoomScrollLayer.h"
#include "Pages/LevelPage.h"
#include "Layers/SongsLayer.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "Layers/MenuLayer.h"

static bool s_didClick = false;

cocos2d::ccColor3B LevelSelectLayer::colorForPage(int page)
{
    cocos2d::ccColor3B color;
    if (page) {
        if (!(page % 22)) {
            color.r = 5;
            color.g = 44;
            color.b = 52;
            return color;
        }
        switch (page) {
            case 1:
                color = GameManager::sharedState()->colorForIdx(7);
                break;
            case 2:
                color = GameManager::sharedState()->colorForIdx(8);
                break;
            case 3:
                color = GameManager::sharedState()->colorForIdx(9);
                break;
            case 4:
                color = GameManager::sharedState()->colorForIdx(10);
                break;
            case 5:
                color = GameManager::sharedState()->colorForIdx(11);
                break;
            case 6:
                color = GameManager::sharedState()->colorForIdx(1);
                break;
            case 7:
                color = GameManager::sharedState()->colorForIdx(3);
                break;
            case 8:
                color = GameManager::sharedState()->colorForIdx(4);
                break;
            default:
                color = colorForPage(page % 9);
                break;
        }
    } else {
        color = GameManager::sharedState()->colorForIdx(5);
    }
    return color;
}

LevelSelectLayer* LevelSelectLayer::create(int page)
{
    LevelSelectLayer* ret = new LevelSelectLayer();
    if (ret->init(page)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

cocos2d::ccColor3B LevelSelectLayer::getColorValue(int page1, int page2, float progress)
{
    float value = (progress / 0.66667f) - 0.2f;
    if (value < 1.0f) {
        if (value <= 0.0f)
            value = 0.0f;
    } else {
        value = 1.0f;
    }
    cocos2d::ccColor3B color1 = colorForPage(page1);
    cocos2d::ccColor3B color2 = colorForPage(page2);
    float modified = modifyValue(value);
    cocos2d::ccColor3B ret;
    ret.r = (unsigned char)((float)color2.r * modified + (float)color1.r * (1.0f - modified));
    ret.g = (unsigned char)((float)color2.g * modified + (float)color1.g * (1.0f - modified));
    ret.b = (unsigned char)((float)color2.b * modified + (float)color1.b * (1.0f - modified));
    return ret;
}

bool LevelSelectLayer::init(int page)
{
    if (!cocos2d::CCLayer::init())
        return false;

    GameStatsManager* gsm = GameStatsManager::sharedState();
    if (gsm->isItemUnlocked(GJItemType::GJItem, 4)) {
        GameManager* gm = GameManager::sharedState();
        gm->setUGV("6", 1);
    }
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    s_didClick = false;
    AchievementManager* am = AchievementManager::sharedState();
    m_masterDetectiveUnlocked = am->isAchievementEarned("geometry.ach.secret04");

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_backgroundSprite = cocos2d::CCSprite::create("GJ_gradientBG.png");
    m_backgroundSprite->setAnchorPoint(ccp(0.0f, 0.0f));
    this->addChild(m_backgroundSprite, -2);
    m_backgroundSprite->setScaleX((winSize.width + 10.0f) / m_backgroundSprite->getContentSize().width);
    m_backgroundSprite->setScaleY((winSize.height + 10.0f) / m_backgroundSprite->getContentSize().height);
    m_backgroundSprite->setPosition(ccp(-5.0f, -5.0f));
    m_backgroundSprite->setColor(ccc3(0, 102, 255));

    GameManager* gm = GameManager::sharedState();
    m_groundLayer = GJGroundLayer::create(gm->m_loadedGroundID, 1);
    this->addChild(m_groundLayer);

    float groundY = (winSize.height * 0.5f) + -110.0f;
    if (groundY > 128.0f)
        groundY = 128.0f;
    m_groundLayer->positionGround(groundY);

    cocos2d::CCSprite* topBar = cocos2d::CCSprite::createWithSpriteFrameName("GJ_topBar_001.png");
    topBar->setAnchorPoint(ccp(0.5f, 1.0f));
    topBar->setPosition(ccp(winSize.width * 0.5f, cocos2d::CCDirector::sharedDirector()->getScreenTop() + 1.0f));
    this->addChild(topBar, 1);

    cocos2d::CCSprite* sideArtLeft = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtLeft->setAnchorPoint(ccp(0.0f, 0.0f));
    sideArtLeft->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    this->addChild(sideArtLeft, 1);

    cocos2d::CCSprite* sideArtRight = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtRight->setAnchorPoint(ccp(1.0f, 0.0f));
    sideArtRight->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    sideArtRight->setFlipX(true);
    this->addChild(sideArtRight, 1);

    cocos2d::CCArray* levels = cocos2d::CCArray::create();
    for (int i = 1; i < 23; ++i) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        GJGameLevel* mainLevel = glm->getMainLevel(i, true);
        levels->addObject(mainLevel);
    }

    cocos2d::CCArray* pages = cocos2d::CCArray::create();
    for (int i = 0; i < 3; ++i) {
        LevelPage* levelPage = LevelPage::create(0);
        pages->addObject(levelPage);
    }

    GJGameLevel* secretLevel1 = GJGameLevel::create();
    secretLevel1->m_levelType = -2;
    int randVal1 = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    secretLevel1->m_levelID = randVal1;
    secretLevel1->m_unkInt = randVal1 - 2;
    levels->addObject(secretLevel1);

    GJGameLevel* secretLevel2 = GJGameLevel::create();
    secretLevel2->m_levelType = -1;
    int randVal2 = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    secretLevel2->m_levelID = randVal2;
    secretLevel2->m_unkInt = randVal2 - 1;
    levels->addObject(secretLevel2);

    m_scrollLayer = BoomScrollLayer::create(pages, 0, 1, false, levels, this);
    this->addChild(m_scrollLayer);

    float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
    m_scrollLayer->setPagesIndicatorPosition(ccp(winSize.width * 0.5f, screenBottom + 15.0f));

    m_width = winSize.width;
    m_scrollLayer->m_extendedLayer->m_delegate = this;

    int pageCount = getPageCount();
    if (page) {
        bool isLastPage = pageCount - 1 == page;
        if (isLastPage) {
            m_scrollLayer->instantMoveToPage(pageCount - 2);
            m_scrollLayer->instantMoveToPage(page);
        } else {
            m_scrollLayer->instantMoveToPage(page);
        }
    } else {
        scrollLayerMoved(ccp(0.0f, 0.0f));
    }

    cocos2d::CCLabelBMFont* downloadLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    downloadLabel->setScale(0.5f);
    CCMenuItemSpriteExtra* downloadBtn = CCMenuItemSpriteExtra::create(
        downloadLabel, nullptr, this, menu_selector(LevelSelectLayer::onDownload));
    downloadBtn->setSizeMult(1.1f);
    cocos2d::CCMenu* downloadMenu = cocos2d::CCMenu::create(downloadBtn, nullptr);
    this->addChild(downloadMenu);
    downloadMenu->setPosition(ccp(
        winSize.width * 0.5f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 35.0f));

    float middleY = winSize.height * 0.5f;
    cocos2d::CCMenu* navMenu = cocos2d::CCMenu::create();
    this->addChild(navMenu, 5);

    bool isController = PlatformToolbox::isControllerConnected();
    cocos2d::CCSprite* prevSprite;
    if (isController) {
        prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_DPad_Left_001.png");
    } else {
        prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        prevSprite->setFlipX(true);
    }
    CCMenuItemSpriteExtra* prevBtn = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, menu_selector(LevelSelectLayer::onPrev));
    navMenu->addChild(prevBtn);
    prevBtn->setSizeMult(2.0f);
    prevBtn->setPosition(navMenu->convertToNodeSpace(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        middleY)));

    const char* nextFrame = isController ? "controllerBtn_DPad_Right_001.png" : "navArrowBtn_001.png";
    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName(nextFrame);
    CCMenuItemSpriteExtra* nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(LevelSelectLayer::onNext));
    navMenu->addChild(nextBtn);
    nextBtn->setSizeMult(2.0f);
    nextBtn->setPosition(navMenu->convertToNodeSpace(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.0f,
        middleY)));

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        backSprite, nullptr, this, menu_selector(LevelSelectLayer::onBack));
    backBtn->setSizeMult(1.6f);
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    this->addChild(backMenu, 1);
    backMenu->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 22.0f));

    GameManager::sharedState()->m_somethingInMenuLayer = 0;

    cocos2d::CCMenu* infoMenu = cocos2d::CCMenu::create();
    this->addChild(infoMenu);
    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite, nullptr, this, menu_selector(LevelSelectLayer::onInfo));
    infoBtn->setSizeMult(2.0f);
    infoMenu->addChild(infoBtn);
    infoMenu->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 20.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 20.0f));

    if (isController)
        GameToolbox::addBackButton(this, backBtn);

    return true;
}

void LevelSelectLayer::keyBackClicked()
{
    onBack(nullptr);
}

void LevelSelectLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    switch (key) {
        case KEY_Right:
        case CONTROLLER_Right:
            onNext(nullptr);
            return;
        case KEY_Left:
        case CONTROLLER_Left:
            onPrev(nullptr);
            return;
        case KEY_Space:
        case KEY_Enter:
        case CONTROLLER_Start:
        case CONTROLLER_A:
            onPlay(nullptr);
            return;
        default:
            break;
    }
    cocos2d::CCLayer::keyDown(key, timestamp);
}

void LevelSelectLayer::onBack(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    cocos2d::CCScene* scene = MenuLayer::scene(false);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void LevelSelectLayer::onDownload(cocos2d::CCObject* sender)
{
    SongsLayer* songsLayer = SongsLayer::create();
    cocos2d::CCNode* parent = getParent();
    parent->addChild(songsLayer, 100);
    songsLayer->showLayer(false);
}

void LevelSelectLayer::onInfo(cocos2d::CCObject* sender)
{
    LevelPage* page = (LevelPage*)m_scrollLayer->getPage(m_scrollLayer->m_page);
    page->onInfo(nullptr);
}

void LevelSelectLayer::onNext(cocos2d::CCObject* sender)
{
    m_scrollLayer->quickUpdate();
    m_scrollLayer->moveToPage(m_scrollLayer->m_page + 1);
}

void LevelSelectLayer::onPlay(cocos2d::CCObject* sender)
{
    if (!s_didClick) {
        LevelPage* page = (LevelPage*)m_scrollLayer->getPage(m_scrollLayer->m_page);
        page->onPlay(nullptr);
    }
}

void LevelSelectLayer::onPrev(cocos2d::CCObject* sender)
{
    m_scrollLayer->quickUpdate();
    m_scrollLayer->moveToPage(m_scrollLayer->m_page - 1);
}

cocos2d::CCScene* LevelSelectLayer::scene(int page)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    LevelSelectLayer* layer = LevelSelectLayer::create(page);
    scene->addChild(layer);
    return scene;
}

void LevelSelectLayer::scrollLayerMoved(cocos2d::CCPoint position)
{
    float pageFloat = -position.x / m_width;
    int pageCount = getPageCount();
    while (pageFloat < 0.0f)
        pageFloat += (float)pageCount;
    int pageIndex = (int)pageFloat;
    m_scrolls = pageIndex;
    float progress = pageFloat - (float)pageIndex;
    int page1 = pageIndex % pageCount;
    int page2 = pageIndex % pageCount + 1;
    if (pageCount == page2)
        page2 = 0;

    cocos2d::ccColor3B color = getColorValue(page1, page2, progress);
    m_backgroundSprite->setColor(color);

    cocos2d::ccColor3B ground1;
    ground1.r = (unsigned char)((float)color.r * 0.8f);
    ground1.g = (unsigned char)((float)color.g * 0.8f);
    ground1.b = (unsigned char)((float)color.b * 0.8f);
    m_groundLayer->updateGround01Color(ground1);

    cocos2d::ccColor3B ground2;
    ground2.r = (unsigned char)((float)color.r * 0.9f);
    ground2.g = (unsigned char)((float)color.g * 0.9f);
    ground2.b = (unsigned char)((float)color.b * 0.9f);
    m_groundLayer->updateGround02Color(ground2);
}

void LevelSelectLayer::tryShowAd()
{
    GameManager* gm = GameManager::sharedState();
    gm->tryShowInterstitial(120, 140, false);
}

int LevelSelectLayer::getPageCount()
{
    return m_scrollLayer->m_pages->count();
}

float LevelSelectLayer::modifyValue(float value)
{
    return value;
}

LevelSelectLayer::~LevelSelectLayer()
{
}