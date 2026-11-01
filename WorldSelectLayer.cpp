#include "WorldSelectLayer.h"
#include "GJWorldNode.h"
#include "BoomScrollLayer.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "GameToolbox.h"
#include "GJGarageLayer.h"
#include "GJMoreGamesLayer.h"
#include "MenuLayer.h"
#include "DialogLayer.h"
#include "DialogObject.h"
#include "PlatformToolbox.h"
#include "CCMenuItemSpriteExtra.h"

void WorldSelectLayer::animateInActiveIsland()
{
    if (m_worldNode)
    {
        int tag = m_worldNode->getTag();
        goToPage(tag - 1, false);

        cocos2d::CCSequence* action1 = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.0f),
            cocos2d::CCCallFunc::create(m_worldNode, callfunc_selector(GJWorldNode::unlockActiveItem)),
            nullptr);
        runAction(action1);

        cocos2d::CCSequence* action2 = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(WorldSelectLayer::unblockButtons)),
            nullptr);
        runAction(action2);
    }
}

cocos2d::ccColor3B WorldSelectLayer::colorForPage(int page)
{
    unsigned char r;
    unsigned char g;
    unsigned char b;

    if (page)
    {
        r = 150;
        if (page == 1)
        {
            g = 25;
            b = 255;
        }
        else
        {
            g = 150;
            b = 150;
        }
    }
    else
    {
        r = 25;
        g = 125;
        b = 255;
    }

    return cocos2d::ccc3(r, g, b);
}

WorldSelectLayer* WorldSelectLayer::create(int page)
{
    WorldSelectLayer* layer = new WorldSelectLayer();

    if (layer->init(page))
    {
        layer->autorelease();
        return layer;
    }

    delete layer;
    return nullptr;
}

cocos2d::ccColor3B WorldSelectLayer::getColorValue(int page1, int page2, float progress)
{
    float t = (progress / 0.66667f) - 0.2f;

    if (t < 1.0f)
    {
        if (t <= 0.0f)
            t = 0.0f;
    }
    else
    {
        t = 1.0f;
    }

    cocos2d::ccColor3B color1 = colorForPage(page1);
    cocos2d::ccColor3B color2 = colorForPage(page2);

    unsigned char r = (unsigned char)((float)color2.r * t + (float)color1.r * (1.0f - t));
    unsigned char g = (unsigned char)((float)color2.g * t + (float)color1.g * (1.0f - t));
    unsigned char b = (unsigned char)((float)color2.b * t + (float)color1.b * (1.0f - t));

    return cocos2d::ccc3(r, g, b);
}

void WorldSelectLayer::goToPage(int page, bool instant)
{
    if (m_scrollLayer->getTotalPages() > page && page >= 0)
    {
        m_scrollLayer->quickUpdate();

        if (instant)
            m_scrollLayer->instantMoveToPage(page);
        else
            m_scrollLayer->moveToPage(page);

        updateArrows();
    }
}

bool WorldSelectLayer::init(int page)
{
    if (!cocos2d::CCLayer::init())
        return false;

    cocos2d::CCTextureCache::sharedTextureCache()->addImage("WorldSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("WorldSheet.plist");

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_background = cocos2d::CCSprite::create("GJ_gradientBG.png");
    m_background->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(m_background, -2);
    m_background->setScaleX((winSize.width + 10.0f) / m_background->m_obRect.size.width);
    m_background->setScaleY((winSize.height + 10.0f) / m_background->m_obRect.size.height);
    m_background->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    m_background->setColor(cocos2d::ccc3(51, 51, 51));

    cocos2d::CCMenu* garageMenu = cocos2d::CCMenu::create();
    addChild(garageMenu, 2);
    garageMenu->setPosition(cocos2d::CCPoint(winSize.width - 40.0f, winSize.height - 25.0f));

    cocos2d::CCSprite* garageSprite = cocos2d::CCSprite::createWithSpriteFrameName("garageRope_001.png");
    CCMenuItemSpriteExtra* garageButton = CCMenuItemSpriteExtra::create(garageSprite, nullptr, menu_selector(WorldSelectLayer::onGarage));
    garageButton->setSizeMult(1.2f);
    garageButton->useAnimationType(Move);
    garageButton->m_offset = cocos2d::CCPoint(0.0f, -8.0f);
    garageButton->m_duration = 0.2f;
    garageButton->m_unselectedDuration = 0.2f;
    garageMenu->addChild(garageButton);

    cocos2d::ccColor3B sideArtColor = cocos2d::ccc3(255, 255, 255);

    cocos2d::CCSprite* leftArt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    leftArt->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    leftArt->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f, cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    addChild(leftArt, 1);
    leftArt->setColor(sideArtColor);

    cocos2d::CCSprite* rightArt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    rightArt->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    rightArt->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f, cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    rightArt->setFlipX(true);
    addChild(rightArt, 1);
    rightArt->setColor(sideArtColor);

    float screenMidY = winSize.height * 0.5f;

    cocos2d::CCMenu* arrowMenu = cocos2d::CCMenu::create();
    addChild(arrowMenu, 5);

    bool controllerConnected = PlatformToolbox::isControllerConnected();

    cocos2d::CCSprite* leftArrowSprite;
    if (controllerConnected)
    {
        leftArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_DPad_Left_001.png");
        leftArrowSprite->setScale(0.6f);
    }
    else
    {
        leftArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        leftArrowSprite->setScale(0.6f);
        leftArrowSprite->setFlipX(true);
    }

    m_arrowLeft = CCMenuItemSpriteExtra::create(leftArrowSprite, nullptr, menu_selector(WorldSelectLayer::onPrev));
    arrowMenu->addChild(m_arrowLeft);
    m_arrowLeft->setSizeMult(2.0f);
    cocos2d::CCPoint leftPos(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f, screenMidY);
    m_arrowLeft->setPosition(arrowMenu->convertToNodeSpace(leftPos));

    const char* rightArrowFrame = controllerConnected ? "controllerBtn_DPad_Right_001.png" : "navArrowBtn_001.png";
    cocos2d::CCSprite* rightArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName(rightArrowFrame);
    rightArrowSprite->setScale(0.6f);

    m_arrowRight = CCMenuItemSpriteExtra::create(rightArrowSprite, nullptr, menu_selector(WorldSelectLayer::onNext));
    arrowMenu->addChild(m_arrowRight);
    m_arrowRight->setSizeMult(2.0f);
    cocos2d::CCPoint rightPos(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.0f, screenMidY);
    m_arrowRight->setPosition(arrowMenu->convertToNodeSpace(rightPos));

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backButton = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(WorldSelectLayer::onBack));
    backButton->setSizeMult(1.6f);
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backButton, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 22.0f));

    if (controllerConnected)
        GameToolbox::addBackButton(this, backButton);

    setupWorlds();
    updateArrows();

    scrollLayerMoved(cocos2d::CCPoint(0, 0));

    GameManager::sharedState()->m_somethingInMenuLayer = false;

    if (m_worldNode)
    {
        if (m_worldNode->m_islandUnlocked)
        {
            cocos2d::CCSequence* action = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(1.0f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(WorldSelectLayer::animateInActiveIsland)),
                nullptr);
            runAction(action);
            m_buttonsLocked = true;
            m_scrollLayer->setTouchEnabled(false);
        }
        else
        {
            int tag = m_worldNode->getTag();
            goToPage(tag - 1, true);
        }
    }
    else if (page > 0)
    {
        goToPage(page, true);
    }

    if (GameStatsManager::sharedState()->hasCompletedMainLevel(2010))
    {
        if (!GameManager::sharedState()->getUGV("18"))
        {
            m_buttonsLocked = true;
            cocos2d::CCSequence* action = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(1.0f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(WorldSelectLayer::showCompleteDialog)),
                nullptr);
            runAction(action);
        }
    }

    return true;
}

void WorldSelectLayer::keyBackClicked()
{
    onBack(nullptr);
}

void WorldSelectLayer::onBack(cocos2d::CCObject* sender)
{
    if (!m_enteringLevel && !m_buttonsLocked)
    {
        m_enteringLevel = true;
        m_cleanupSprites = true;
        setKeypadEnabled(false);

        cocos2d::CCScene* scene = MenuLayer::scene(false);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    }
}

void WorldSelectLayer::onExit()
{
    cocos2d::CCLayer::onExit();

    if (m_cleanupSprites)
    {
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("WorldSheet.plist");
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("WorldSheet.png");
    }
}

void WorldSelectLayer::onFreeLevels(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->reportAchievementWithID("geometry.ach.moreGames", 100, false);

    GJMoreGamesLayer* moreGamesLayer = GJMoreGamesLayer::create();
    cocos2d::CCDirector::sharedDirector()->m_pRunningScene->addChild(moreGamesLayer, 100);
    moreGamesLayer->showLayer(false);
}

void WorldSelectLayer::onGarage(cocos2d::CCObject* sender)
{
    if (!m_buttonsLocked)
    {
        cocos2d::CCScene* scene = GJGarageLayer::scene();
        cocos2d::CCTransitionMoveInT* transition = cocos2d::CCTransitionMoveInT::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(transition);
        GameManager::sharedState()->m_ropeGarageEnter = true;
    }
}

void WorldSelectLayer::onNext(cocos2d::CCObject* sender)
{
    if (!m_buttonsLocked)
        goToPage(m_scrollLayer->m_page + 1, false);
}

void WorldSelectLayer::onPrev(cocos2d::CCObject* sender)
{
    if (!m_buttonsLocked)
        goToPage(m_scrollLayer->m_page - 1, false);
}

cocos2d::CCScene* WorldSelectLayer::scene(int page)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    WorldSelectLayer* layer = WorldSelectLayer::create(page);
    scene->addChild(layer);
    return scene;
}

void WorldSelectLayer::scrollLayerMoved(cocos2d::CCPoint position)
{
    float offset = -position.x;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    float i = offset / winSize.width;
    while (i < 0.0f)
        i += 23.0f;

    int pageInt = (int)floorf(i);
    int nextPage = pageInt % 23 + 1;
    float progress = i - (float)pageInt;

    if (pageInt % 23 == 22)
        nextPage = 0;

    cocos2d::ccColor3B color = getColorValue(pageInt % 23, nextPage, progress);
    m_background->setColor(color);
}

void WorldSelectLayer::setupWorlds()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCArray* pages = cocos2d::CCArray::create();
    cocos2d::CCPoint origin(winSize.width * 0.5f, winSize.height * 0.5f - 15.0f + 2.0f - 5.0f);

    for (int worldIndex = 1; worldIndex != 4; worldIndex++)
    {
        cocos2d::CCLayer* pageLayer = cocos2d::CCLayer::create();
        pages->addObject(pageLayer);

        GJWorldNode* worldNode = GJWorldNode::create(worldIndex, this);
        pageLayer->addChild(worldNode);
        worldNode->setTag(worldIndex);

        if (worldIndex == 2)
        {
            worldNode->setPosition(origin + cocos2d::CCPoint(0.0f, -10.0f));
        }
        else
        {
            worldNode->setPosition(origin + cocos2d::CCPoint(0.0f, 0.0f));
        }

        if (worldIndex != 3)
        {
            cocos2d::CCMoveBy* moveDown = cocos2d::CCMoveBy::create(2.0f, cocos2d::CCPoint(0.0f, -4.0f));
            cocos2d::CCEaseInOut* easeDown = cocos2d::CCEaseInOut::create(moveDown, 1.8f);
            cocos2d::CCMoveBy* moveUp = cocos2d::CCMoveBy::create(2.0f, cocos2d::CCPoint(0.0f, 4.0f));
            cocos2d::CCEaseInOut* easeUp = cocos2d::CCEaseInOut::create(moveUp, 1.8f);
            cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(easeDown, easeUp, nullptr);
            cocos2d::CCRepeatForever* repeat = cocos2d::CCRepeatForever::create(sequence);
            worldNode->runAction(repeat);
            repeat->step(0.5f);
        }

        cocos2d::CCString* labelFrame = cocos2d::CCString::createWithFormat("worldLabel_%02d_001.png", worldIndex);
        cocos2d::CCSprite* labelSprite = cocos2d::CCSprite::createWithSpriteFrameName(labelFrame->getCString());
        pageLayer->addChild(labelSprite);
        labelSprite->setPosition(origin + cocos2d::CCPoint(0.0f, 142.0f));

        if (worldNode->m_activeButton)
            m_worldNode = worldNode;

        if (worldIndex == 3)
        {
            cocos2d::CCLabelBMFont* freeLevelsLabel = cocos2d::CCLabelBMFont::create("Free Levels", "bigFont.fnt");
            pageLayer->addChild(freeLevelsLabel);
            freeLevelsLabel->setScale(0.8f);
            freeLevelsLabel->setPosition(origin + cocos2d::CCPoint(0.0f, 142.0f));

            cocos2d::CCSprite* freeLevelsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_freeLevelsBtn_001.png");
            CCMenuItemSpriteExtra* freeLevelsButton = CCMenuItemSpriteExtra::create(freeLevelsSprite, nullptr, menu_selector(WorldSelectLayer::onFreeLevels));
            cocos2d::CCMenu* freeLevelsMenu = cocos2d::CCMenu::createWithItem(freeLevelsButton);
            pageLayer->addChild(freeLevelsMenu);
            freeLevelsMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f));
        }
    }

    m_scrollLayer = BoomScrollLayer::create(pages, 0, 0, nullptr, nullptr, nullptr);
    m_scrollLayer->m_extendedLayer->m_delegate = this;
    m_scrollLayer->m_delegate = this;
    addChild(m_scrollLayer);

    float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
    m_scrollLayer->setPagesIndicatorPosition(cocos2d::CCPoint(winSize.width * 0.5f, screenBottom + 15.0f));
}

void WorldSelectLayer::showCompleteDialog()
{
    m_buttonsLocked = false;
    GameManager::sharedState()->setUGV("18", 1);

    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    dialogs->addObject(DialogObject::create(
        "The Keymaster",
        "Well, well, well...<d020> I see you have <cg>completed</c> the <cl>first two</c> islands.",
        2, 1.0f, false, cocos2d::ccc3(255, 255, 255)));

    dialogs->addObject(DialogObject::create(
        "The Keymaster",
        "Not bad,<d030> not bad at all.\n<d030>Why not try some <cg>online levels</c>?",
        2, 0.85f, false, cocos2d::ccc3(255, 255, 255)));

    dialogs->addObject(DialogObject::create(
        "The Keymaster",
        "<cl>Let's see what more you can do<d010>.<d010>.<d010>.</c>",
        2, 1.0f, false, cocos2d::ccc3(255, 255, 255)));

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(Center);
    dialogLayer->animateInRandomSide();
}

void WorldSelectLayer::tryShowAd()
{
    GameManager::sharedState()->tryShowInterstitial(120, 140, false);
}

void WorldSelectLayer::unblockButtons()
{
    m_buttonsLocked = false;
    m_scrollLayer->setTouchEnabled(true);
}

void WorldSelectLayer::updateArrows()
{
    bool showLeft = m_scrollLayer ? m_scrollLayer->m_page > 0 : false;
    m_arrowLeft->setVisible(showLeft);

    if (m_scrollLayer)
    {
        bool showRight = m_scrollLayer->m_page < m_scrollLayer->getTotalPages() - 1;
        m_arrowRight->setVisible(showRight);
    }
    else
    {
        m_arrowRight->setVisible(false);
    }
}

WorldSelectLayer::~WorldSelectLayer()
{
}