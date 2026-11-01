#include "Layers/GauntletLayer.h"
#include "Nodes/GauntletNode.h"
#include "Sprites/GauntletSprite.h"
#include "UI/LoadingCircle.h"
#include "UI/TextArea.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Delegates/AppDelegate.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GauntletLayer* GauntletLayer::create(GauntletType type)
{
    GauntletLayer* instance = new GauntletLayer();
    if (instance && instance->init(type))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GauntletLayer::init(GauntletType type)
{
    if (!cocos2d::CCLayer::init())
        return false;

    m_gauntletType = type;
    m_levels = nullptr;
    m_loadingCircle = nullptr;
    m_tryAgainText = nullptr;
    m_activeItemButton = nullptr;
    m_activeObjects = nullptr;
    m_backgroundSprite = nullptr;
    m_unkPtr = nullptr;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    // Setup background
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    const char* bgName = bgForGauntlet(type);
    cocos2d::CCSprite* bg = cocos2d::CCSprite::create(bgName);
    
    cocos2d::ccTexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
    bg->getTexture()->setTexParameters(params);
    
    addChild(bg, -1);
    bg->setAnchorPoint(cocos2d::CCPointZero);
    bg->setScale(AppDelegate::get()->bgScale());
    bg->setColor(cocos2d::ccc3(76, 76, 76));
    bg->setPosition(cocos2d::CCPoint(0.0f, -20.0f));
    m_backgroundSprite = bg;

    // Title
    std::string title = GauntletNode::nameForType(type) + " Gauntlet";
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    addChild(titleLabel, 2);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height - 22.0f));
    titleLabel->setScale(1.0f);
    titleLabel->setColor(cocos2d::ccc3(255, 255, 0));

    // Title shadow
    cocos2d::CCLabelBMFont* shadowLabel = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    addChild(shadowLabel, 0);
    cocos2d::CCPoint titlePos = titleLabel->getPosition();
    shadowLabel->setPosition(cocos2d::CCPoint(titlePos.x + 2.0f, titlePos.y - 4.0f));
    shadowLabel->setScale(1.0f);
    shadowLabel->setColor(cocos2d::ccc3(0, 0, 0));
    shadowLabel->setOpacity(100);

    // Back button
    cocos2d::CCSprite* arrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    cocos2d::CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(arrow, nullptr, this, menu_selector(GauntletLayer::onBack));
    backBtn->setSizeMult(1.6f);
    
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(menu, 2);
    menu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 22.0f
    ));

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    // Loading circle
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = this;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);
    m_loadingCircle->m_sprite->setColor(cocos2d::ccc3(100, 100, 100));

    // Try again text
    std::string errorText = "Something went wrong\nplease try again later";
    m_tryAgainText = TextArea::create(errorText, "bigFont.fnt", 1.0f, 600.0f, cocos2d::CCPoint(0.5f, 1.0f), 20.0f);
    m_tryAgainText->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f));
    m_tryAgainText->setScale(0.6f);
    addChild(m_tryAgainText, 10);
    m_tryAgainText->setVisible(false);

    // Load levels
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string searchKey = glm->getGauntletSearchKey(type);
    cocos2d::CCArray* storedLevels = glm->getStoredOnlineLevels(searchKey);
    
    if (storedLevels)
    {
        setupGauntlet(storedLevels);
    }
    else
    {
        m_loadingCircle->setVisible(true);
        glm->m_levelManagerDelegate = this;
        glm->getGauntletLevels(type);
    }

    return true;
}

void GauntletLayer::keyBackClicked()
{
    onBack(nullptr);
}

void GauntletLayer::loadLevelsFailed(const char* key, int type)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string searchKey = glm->getGauntletSearchKey(m_gauntletType);
    
    if (searchKey == key)
    {
        m_loadingCircle->setVisible(false);
        m_tryAgainText->setVisible(true);
    }
}

void GauntletLayer::loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int type)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string searchKey = glm->getGauntletSearchKey(m_gauntletType);
    
    if (searchKey == key)
    {
        m_loadingCircle->setVisible(false);
        setupGauntlet(levels);
    }
}

void GauntletLayer::onBack(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    cocos2d::CCDirector::sharedDirector()->replaceScene(
        cocos2d::CCTransitionFade::create(0.5f, GauntletSelectLayer::scene(0))
    );
}

void GauntletLayer::onLevel(cocos2d::CCObject* sender)
{
    int index = sender->getTag();
    GJGameLevel* level = (GJGameLevel*)m_levels->objectAtIndex(index);
    GameLevelManager::sharedState()->gotoLevelPage(level);
}

cocos2d::CCScene* GauntletLayer::scene(GauntletType type)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    GauntletLayer* layer = GauntletLayer::create(type);
    scene->addChild(layer);
    return scene;
}

void GauntletLayer::setupGauntlet(cocos2d::CCArray *levels)
{
    // Validate levels
    if (!levels || levels->count() != 5 || m_levels)
        return;

    m_levels = levels;
    m_levels->retain();

    // Get window size
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 12.0f);

    // Create menu
    cocos2d::CCMenu *menu = cocos2d::CCMenu::create();
    addChild(menu, 2);

    // Get frame name for this gauntlet type
    std::string frame = GauntletNode::frameForType(m_gauntletType);
    std::string lockedFrame = frame + "_locked_001";

    // Find first incomplete level
    int firstIncomplete = 0;
    GameStatsManager *gsm = GameStatsManager::sharedState();
    for (int i = 0; i < levels->count(); i++)
    {
        GJGameLevel *level = (GJGameLevel *)levels->objectAtIndex(i);
        if (!gsm->hasCompletedGauntletLevel(level))
        {
            firstIncomplete = i;
            break;
        }
    }

    float yOffset = yOffsetForGauntlet(m_gauntletType);
    cocos2d::CCPoint lastPos = cocos2d::CCPointZero;

    for (int i = 0; i < levels->count(); i++)
    {
        GJGameLevel *level = (GJGameLevel *)levels->objectAtIndex(i);
        bool isLocked = (i > 0 && i == firstIncomplete && !level->m_gauntletLevel2);
        bool isCompleted = false;
        int lockState = isLocked ? 1 : 0;

        // Position
        float xOffset = (i - 2) * 90.0f;
        float yOffset2 = (i & 1) ? 50.0f : -50.0f;
        cocos2d::CCPoint pos = center + cocos2d::CCPoint(xOffset, yOffset2);

        // Create gauntlet sprite
        GauntletSprite *sprite = GauntletSprite::create(m_gauntletType, lockState | (i > firstIncomplete));
        cocos2d::CCMenuItemSpriteExtra *btn = CCMenuItemSpriteExtra::create(sprite, nullptr, this, menu_selector(GauntletLayer::onLevel));
        btn->m_scaleMultiplier = 1.2f;
        btn->setSizeMult(1.2f);
        btn->setTag(i);

        // Position button
        float yOffset3 = (i & 1) ? -6.0f : 0.0f;
        cocos2d::CCPoint menuPos = menu->convertToNodeSpace(pos + cocos2d::CCPoint(0.0f, yOffset3));
        btn->setPosition(menuPos);
        menu->addChild(btn);

        // Content node
        cocos2d::CCNode *contentNode = cocos2d::CCNode::create();
        contentNode->setContentSize(cocos2d::CCSize(50.0f, 50.0f));
        sprite->addChild(contentNode, 1);

        cocos2d::CCSize spriteSize = sprite->getScaledContentSize();
        contentNode->setPosition(cocos2d::CCPoint(spriteSize.width * 0.5f, spriteSize.height * 0.5f));

        // Lock overlay node
        cocos2d::CCNode *overlayNode = cocos2d::CCNode::create();
        overlayNode->setContentSize(cocos2d::CCSize(50.0f, 50.0f));
        sprite->addChild(overlayNode, -1);
        overlayNode->setPosition(contentNode->getPosition());

        // Background sprite
        cocos2d::CCSprite *bgSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
        overlayNode->addChild(bgSprite, -1);
        bgSprite->setPosition(cocos2d::CCPoint(0.0f, -10.0f));
        bgSprite->setScaleY(1.2f);
        bgSprite->setColor(cocos2d::ccc3(0, 0, 0));
        bgSprite->setOpacity(75);

        // Level number and star (only for unlocked levels)
        if (i <= firstIncomplete)
        {
            // Level number label
            cocos2d::CCLabelBMFont *numLabel = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("%i", level->m_stars.random - level->m_stars.seed)->getCString(),
                "bigFont.fnt"
            );
            numLabel->limitLabelWidth((i & 1) ? 120.0f : 80.0f, 0.5f, 0.0f);
            contentNode->addChild(numLabel, 3);
            numLabel->setPosition(cocos2d::CCPoint(0.0f, yOffset + 40.0f));

            // Star sprite
            cocos2d::CCSprite *star = cocos2d::CCSprite::createWithSpriteFrameName("GJ_bigStar_001.png");
            star->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
            star->setScale(0.4f);
            contentNode->addChild(star, 2);
            star->setPosition(cocos2d::CCPoint(-4.0f, -38.0f));

            // Completion check
            if (gsm->hasCompletedLevel(level))
            {
                cocos2d::CCSprite *check = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
                check->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
                check->setScale(0.8f);
                contentNode->addChild(check, 5);
                check->setPosition(cocos2d::CCPoint(8.0f, -5.0f));
            }

            // Skull for last level
            if (i == 4)
            {
                cocos2d::CCSprite *skull = cocos2d::CCSprite::createWithSpriteFrameName("miniSkull_001.png");
                skull->setScale(1.2f);
                contentNode->addChild(skull, 4);
                skull->setPosition(cocos2d::CCPoint(0.0f, 5.0f));
            }
        }

        // Disable button for locked levels
        if (i > firstIncomplete)
        {
            btn->setEnabled(false);
        }

        // Handle locked state
        if (isLocked)
        {
            m_activeObjects = cocos2d::CCArray::create();
            m_activeObjects->retain();

            // Store all visible objects to hide them
            m_activeObjects->addObject(numLabel);
            m_activeObjects->addObject(starLabel);
            m_activeObjects->addObject(star);
            if (skull)
                m_activeObjects->addObject(skull);

            // Hide all objects
            for (int j = 0; j < m_activeObjects->count(); j++)
            {
                cocos2d::CCObject *obj = m_activeObjects->objectAtIndex(j);
                obj->setVisible(false);
            }

            m_activeItemButton = btn;
            btn->setEnabled(false);

            // Schedule unlock
            this->runAction(cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(2.0f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(GauntletLayer::unlockActiveItem)),
                nullptr
            ));
        }

        // Animated dot trail for unlocked levels
        if (i > 0 && i <= firstIncomplete)
        {
            cocos2d::CCPoint startPos = lastPos;
            startPos.y -= 2.0f;
            if (i & 1)
                startPos.x += 27.0f;
            else
                startPos.x -= 27.0f;

            // Define dot positions
            cocos2d::CCPoint dotPositions[] = {
                cocos2d::CCPoint(-2.0f, -1.0f),
                cocos2d::CCPoint(10.0f, 2.5f),
                cocos2d::CCPoint(21.0f, 11.5f),
                cocos2d::CCPoint(25.0f, 26.0f),
                cocos2d::CCPoint(20.5f, 41.0f),
                cocos2d::CCPoint(15.0f, 56.0f),
                cocos2d::CCPoint(10.5f, 71.0f),
                cocos2d::CCPoint(14.5f, 85.5f),
                cocos2d::CCPoint(25.0f, 96.0f)
            };

            for (int k = 1; k < 9; k++)
            {
                cocos2d::CCPoint dotPos = dotPositions[k];
                if ((i & 1) == 0)
                    dotPos.x = -dotPos.x;

                cocos2d::CCPoint finalPos = startPos + dotPos;

                cocos2d::CCSprite *dot = cocos2d::CCSprite::createWithSpriteFrameName("uiDot_001.png");
                addChild(dot, 1);

                int color = (k & 1) ? 153 : 178;
                dot->setColor(cocos2d::ccc3(color, color, color));
                dot->setPosition(finalPos);

                if (isLocked)
                {
                    // Animate the dot
                    dot->setOpacity(0);
                    float currentScale = dot->getScale();
                    dot->setScale(currentScale * 2.5f);

                    cocos2d::CCAction *scaleAction = cocos2d::CCEaseBounceOut::create(
                        cocos2d::CCScaleTo::create(0.3f, currentScale)
                    );
                    cocos2d::CCAction *fadeAction = cocos2d::CCFadeIn::create(0.1f);

                    cocos2d::ccColor3B color3B = dot->getColor();
                    cocos2d::CCAction *tintAction = cocos2d::CCTintTo::create(0.5f, color3B.r, color3B.g, color3B.b);

                    dot->setColor(cocos2d::ccc3(255, 255, 255));
                    cocos2d::CCAction *spawnAction = cocos2d::CCSpawn::create(scaleAction, fadeAction, tintAction, nullptr);

                    float delay = (i & 1) ? (k * 0.15f + 0.65f) : ((9 - k) * 0.15f + 0.65f);
                    dot->runAction(cocos2d::CCSequence::create(
                        cocos2d::CCDelayTime::create(delay),
                        spawnAction,
                        nullptr
                    ));
                }
            }
        }

        lastPos = pos;
    }
}

void GauntletLayer::unlockActiveItem()
{
    if (!m_activeItemButton)
        return;

    // Unlock the gauntlet sprite
    GauntletSprite* sprite = (GauntletSprite*)m_activeItemButton->getNormalImage();
    sprite->toggleLockedSprite(false);

    // Get center position
    cocos2d::CCSize spriteSize = sprite->getScaledContentSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(spriteSize.width * 0.5f, spriteSize.height * 0.5f);

    // Unhide all active objects
    for (int i = 0; i < m_activeObjects->count(); i++)
    {
        cocos2d::CCObject* obj = m_activeObjects->objectAtIndex(i);
        obj->setVisible(true);
    }

    // Show unlock particles
    for (int i = 0; i < 3; i++)
    {
        cocos2d::CCSprite* unlockSprite = cocos2d::CCSprite::createWithSpriteFrameName("gauntletLock_001.png");
        sprite->addChild(unlockSprite, 2);
        unlockSprite->setPosition(center);
        unlockSprite->setColor(cocos2d::ccc3(255, 255, 255));
        unlockSprite->setBlendFunc(cocos2d::ccBlendFunc{ GL_SRC_ALPHA, GL_ONE });
        unlockSprite->runAction(cocos2d::CCFadeOut::create(0.5f));
    }

    // Bounce animation
    float currentScale = sprite->getScale();
    sprite->setScale(1.6f);
    sprite->runAction(cocos2d::CCEaseBounceOut::create(
        cocos2d::CCScaleTo::create(0.5f, currentScale)
    ));

    // Enable button
    m_activeItemButton->setEnabled(true);

    // Mark level as unlocked
    int index = m_activeItemButton->getTag();
    GJGameLevel* level = (GJGameLevel*)m_levels->objectAtIndex(index);
    level->m_gauntletLevel2 = true;

    // Save to saved levels
    GJGameLevel* savedLevel = GameLevelManager::sharedState()->getSavedLevel(level);
    if (savedLevel)
        savedLevel->m_gauntletLevel2 = true;
}

GauntletLayer::~GauntletLayer()
{
    if (m_levels)
        m_levels->release();
    if (m_loadingCircle)
        m_loadingCircle->release();
    if (m_activeObjects)
        m_activeObjects->release();
}