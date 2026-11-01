#include "Layers/GauntletSelectLayer.h"
#include "Nodes/GauntletNode.h"
#include "Layers/BoomScrollLayer.h"
#include "UI/LoadingCircle.h"
#include "UI/TextArea.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Delegates/AppDelegate.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/FLAlertLayer.h"
#include "Layers/DialogLayer.h"
#include "Layers/RewardUnlockLayer.h"
#include "Utils/PlatformToolbox.h"
#include "Utils/GameToolbox.h"
#include "Utils/AdToolbox.h"

GauntletSelectLayer* GauntletSelectLayer::create(int unused)
{
    GauntletSelectLayer* instance = new GauntletSelectLayer();
    if (instance && instance->init(unused))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GauntletSelectLayer::init(int unused)
{
    if (!cocos2d::CCLayer::init())
        return false;

    // Load textures
    cocos2d::CCTextureCache::sharedTextureCache()->addImage("GauntletSheet.png");
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("GauntletSheet.plist");

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    // Background
    m_gauntlets = cocos2d::CCDictionary::create();
    m_gauntlets->retain();

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPointZero);
    addChild(bg, -2);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().width);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().height);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(51, 51, 51));
    m_backgroundSprite = bg;

    // Title
    cocos2d::CCSprite* title = cocos2d::CCSprite::createWithSpriteFrameName("theLostGauntletsLabel_001.png");
    addChild(title);
    title->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 28.0f));

    // Corner decorations
    cocos2d::ccColor3B cornerColor = cocos2d::ccc3(4, 4, 4);

    cocos2d::CCSprite* corner1 = cocos2d::CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
    corner1->setAnchorPoint(cocos2d::CCPointZero);
    corner1->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
    ));
    addChild(corner1, 1);
    corner1->setColor(cornerColor);

    cocos2d::CCSprite* corner2 = cocos2d::CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
    corner2->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    corner2->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f
    ));
    corner2->setFlipX(true);
    addChild(corner2, 1);
    corner2->setColor(cornerColor);

    cocos2d::CCSprite* corner3 = cocos2d::CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
    corner3->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    corner3->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() + 1.0f
    ));
    corner3->setFlipY(true);
    addChild(corner3, 1);
    corner3->setColor(cornerColor);

    cocos2d::CCSprite* corner4 = cocos2d::CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
    corner4->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    corner4->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() + 1.0f
    ));
    corner4->setFlipX(true);
    corner4->setFlipY(true);
    addChild(corner4, 1);
    corner4->setColor(cornerColor);

    float centerY = winSize.height * 0.5f;

    // Menu for navigation buttons
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    addChild(menu, 5);

    bool controllerConnected = PlatformToolbox::isControllerConnected();

    // Left arrow
    cocos2d::CCSprite* leftSprite;
    if (controllerConnected)
    {
        leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_DPad_Left_001.png");
        leftSprite->setScale(0.8f);
    }
    else
    {
        leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        leftSprite->setScale(0.8f);
        leftSprite->setFlipX(true);
    }

    m_leftButton = CCMenuItemSpriteExtra::create(leftSprite, nullptr, this, menu_selector(GauntletSelectLayer::onPrev));
    m_leftButton->setSizeMult(2.0f);
    menu->addChild(m_leftButton);

    cocos2d::CCPoint leftPos = menu->convertToNodeSpace(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        centerY
    ));
    m_leftButton->setPosition(leftPos);

    // Right arrow
    cocos2d::CCSprite* rightSprite;
    if (controllerConnected)
        rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_DPad_Right_001.png");
    else
        rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    rightSprite->setScale(0.8f);

    m_rightButton = CCMenuItemSpriteExtra::create(rightSprite, nullptr, this, menu_selector(GauntletSelectLayer::onNext));
    m_rightButton->setSizeMult(2.0f);
    menu->addChild(m_rightButton);

    cocos2d::CCPoint rightPos = menu->convertToNodeSpace(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.0f,
        centerY
    ));
    m_rightButton->setPosition(rightPos);

    // Back button
    cocos2d::CCSprite* backArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backArrow, nullptr, this, menu_selector(GauntletSelectLayer::onBack));
    backBtn->setSizeMult(1.6f);

    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 22.0f
    ));

    // Info button
    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSprite, nullptr, this, menu_selector(GauntletSelectLayer::onInfo));
    infoBtn->setSizeMult(2.0f);
    backMenu->addChild(infoBtn);

    cocos2d::CCPoint infoPos = backMenu->convertToNodeSpace(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 30.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 30.0f
    ));
    infoBtn->setPosition(infoPos);

    if (controllerConnected)
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
    m_tryAgainText = TextArea::create(errorText, "bigFont.fnt", 1.0f, 600.0f, cocos2d::CCPoint(0.5f, 1.0f), 30.0f);
    m_tryAgainText->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f));
    m_tryAgainText->setScale(0.6f);
    addChild(m_tryAgainText, 10);
    m_tryAgainText->setVisible(false);

    // Refresh button
    cocos2d::CCSprite* refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    refreshSprite->setScale(0.75f);
    m_refreshButton = CCMenuItemSpriteExtra::create(refreshSprite, nullptr, this, menu_selector(GauntletSelectLayer::onRefresh));
    backMenu->addChild(m_refreshButton);

    cocos2d::CCPoint refreshPos = backMenu->convertToNodeSpace(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.0f,
        25.0f
    ));
    m_refreshButton->setPosition(refreshPos);
    m_refreshButton->setVisible(false);

    // Load gauntlets
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->areGauntletsLoaded())
    {
        setupGauntlets();
    }
    else
    {
        m_loadingCircle->setVisible(true);
        glm->m_levelManagerDelegate = this;
        glm->getGauntlets();
    }

    updateArrows();

    return true;
}

void GauntletSelectLayer::keyBackClicked()
{
    onBack(nullptr);
}

void GauntletSelectLayer::onBack(cocos2d::CCObject* sender)
{
    if (m_playing || m_locked)
        return;

    m_playing = true;
    m_exiting = true;
    setKeypadEnabled(false);
    GameManager::sharedState()->m_currentGauntlet = 1;
    cocos2d::CCDirector::sharedDirector()->replaceScene(
        cocos2d::CCTransitionFade::create(0.5f, CreatorLayer::scene())
    );
}

void GauntletSelectLayer::onNext(cocos2d::CCObject* sender)
{
    if (m_scrollLayer)
        goToPage(m_scrollLayer->m_page + 1, false);
}

void GauntletSelectLayer::onPrev(cocos2d::CCObject* sender)
{
    if (m_scrollLayer)
        goToPage(m_scrollLayer->m_page - 1, false);
}

void GauntletSelectLayer::goToPage(int page, bool instant)
{
    if (!m_scrollLayer)
        return;

    m_scrollLayer->quickUpdate();
    if (instant)
        m_scrollLayer->instantMoveToPage(page);
    else
        m_scrollLayer->moveToPage(page);

    updateArrows();

    m_playBlocked = true;
    this->stopActionByTag(1001);
    this->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.3f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GauntletSelectLayer::unblockPlay)),
        nullptr
    ));
}

void GauntletSelectLayer::unblockPlay()
{
    m_playBlocked = false;
}

void GauntletSelectLayer::updateArrows()
{
    bool hasPages = (m_scrollLayer != nullptr);
    m_leftButton->setVisible(hasPages);
    m_rightButton->setVisible(hasPages);
}

void GauntletSelectLayer::onInfo(cocos2d::CCObject* sender)
{
    std::string info = "<cg>Gauntlets</c> are themed collections of 5 rated levels that give a special <cy>reward</c> once beaten. They are made by RobTop based on which levels fit the theme or through official <cl>creator contests</c>. Follow RobTop on social media to be notified about future contests!";
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Gauntlets", info);
    alert->show();
}

void GauntletSelectLayer::onExit()
{
    cocos2d::CCLayer::onExit();
    if (m_exiting)
    {
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("GauntletSheet.plist");
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("GauntletSheet.png");
    }
}

void GauntletSelectLayer::loadLevelsFailed(const char* key, int type)
{
    std::string keyStr = key;
    if (keyStr == "get_gauntlets")
    {
        m_loadingCircle->setVisible(false);
        m_tryAgainText->setVisible(true);
        m_refreshButton->setVisible(true);
    }
}

void GauntletSelectLayer::onRefresh(cocos2d::CCObject* sender)
{
    m_refreshButton->setVisible(false);

    if (m_scrollLayer)
    {
        m_scrollLayer->removeFromParentAndCleanup(true);
        m_scrollLayer = nullptr;
    }

    updateArrows();

    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->resetGauntlets();
    glm->m_levelManagerDelegate = this;
    m_loadingCircle->setVisible(true);
    glm->getGauntlets();
}

void GauntletSelectLayer::setupGauntlets()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 11.0f);

    cocos2d::CCArray* pages = cocos2d::CCArray::create();
    cocos2d::CCArray* pageMenus = cocos2d::CCArray::create();
    cocos2d::CCArray* pageContainers = cocos2d::CCArray::create();

    cocos2d::CCDictionary* savedGauntlets = GameLevelManager::sharedState()->m_savedGauntlets;
    int currentGauntlet = GameManager::sharedState()->m_currentGauntlet;
    int targetPage = 0;
    int itemIndex = 1;

    if (savedGauntlets && savedGauntlets->count() > 0)
    {
        cocos2d::CCDictElement* element = savedGauntlets->m_pElements;
        while (element)
        {
            GJMapPack* pack = (GJMapPack*)element->m_pObject;

            // Skip Demon Gauntlet if not unlocked
            if (pack->m_packID == 7 && !GameManager::getUGV("19"))
            {
                element = (cocos2d::CCDictElement*)element->hh.next;
                continue;
            }

            int page = (int)ceilf((float)itemIndex / 3.0f) - 1;

            // Create page if needed
            if (pages->count() <= page)
            {
                cocos2d::CCLayer* layer = cocos2d::CCLayer::create();
                pages->addObject(layer);
                cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
                layer->addChild(menu);
                pageMenus->addObject(menu);
                pageContainers->addObject(cocos2d::CCArray::create());
            }

            if (currentGauntlet == pack->m_packID)
                targetPage = (currentGauntlet > 1) ? page : 0;

            // Create gauntlet node
            GauntletNode* node = GauntletNode::create(pack);
            m_gauntlets->setObject(node, pack->m_packID);

            // Create container
            cocos2d::CCNode* container = cocos2d::CCNode::create();
            container->setContentSize(cocos2d::CCSize(110.0f, 234.0f));
            container->addChild(node);

            cocos2d::CCPoint containerCenter = cocos2d::CCPoint(
                container->getContentSize().width * 0.5f,
                container->getContentSize().height * 0.5f
            );
            node->setPosition(container->convertToNodeSpace(containerCenter));

            // Create menu item
            CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(container, nullptr, this, menu_selector(GauntletSelectLayer::onPlay));
            btn->m_scaleMultiplier = 1.05f;
            btn->setTag(pack->m_packID);

            cocos2d::CCArray* items = (cocos2d::CCArray*)pageContainers->objectAtIndex(page);
            items->addObject(btn);

            // Add to menu
            cocos2d::CCMenu* pageMenu = (cocos2d::CCMenu*)pageMenus->objectAtIndex(page);
            pageMenu->addChild(btn);

            itemIndex++;
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
    }

    // Position items on each page
    for (int i = 0; i < pages->count(); i++)
    {
        cocos2d::CCLayer* page = (cocos2d::CCLayer*)pages->objectAtIndex(i);
        cocos2d::CCArray* items = (cocos2d::CCArray*)pageContainers->objectAtIndex(i);
        cocos2d::CCMenu* menu = (cocos2d::CCMenu*)pageMenus->objectAtIndex(i);

        // Convert center to page space
        cocos2d::CCPoint pageCenter = page->convertToNodeSpace(center);
        GameToolbox::alignItemsHorisontally(items, 112.0f, pageCenter);
    }

    // Create scroll layer
    m_scrollLayer = BoomScrollLayer::create(pages, false, true, false, 0, nullptr);
    m_scrollLayer->m_delegate = this;
    addChild(m_scrollLayer);

    // Set pages indicator
    cocos2d::CCPoint indicatorPos = cocos2d::CCPoint(
        winSize.width * 0.5f,
        cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 15.0f
    );
    m_scrollLayer->setPagesIndicatorPosition(indicatorPos);

    // Go to target page
    if (targetPage > 0)
    {
        int totalPages = m_scrollLayer->getTotalPages();
        if (targetPage == totalPages - 1)
            goToPage(targetPage - 1, true);
        goToPage(targetPage, true);
    }

    updateArrows();
}

void GauntletSelectLayer::onPlay(cocos2d::CCObject* sender)
{
    if (m_playing || m_playBlocked || m_locked)
        return;

    int packId = sender->getTag();

    // Check if Demon Gauntlet is locked
    if (packId == 8 && !GameStatsManager::sharedState()->isItemUnlocked(GJItem, 5))
    {
        // Show dialog about Demon Guardian
        cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

        auto addDialog = [&](const char* title, const char* text) {
            std::string t = title;
            std::string txt = text;
            DialogObject* dialog = DialogObject::create(t, txt, 2, 1.0f, 0, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(dialog);
        };

        addDialog("...", "It's no use, that <cg>Gauntlet</c> is <cl>locked</c>.");
        addDialog("...", "The <co>Demon Guardian</c> was the only one who could open it.");
        addDialog("...", "Not sure what happened to him. He always was a bit crazy.");

        DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
        addChild(dialogLayer, 100);
        dialogLayer->updateChatPlacement(Center);
        dialogLayer->animateInRandomSide();
        return;
    }

    GameLevelManager* glm = GameLevelManager::sharedState();
    GJMapPack* pack = glm->getSavedGauntlet(packId);

    if (!pack)
        return;

    // Check if gauntlet is unlocked
    if (!GameStatsManager::sharedState()->isGauntletUnlocked(packId))
    {
        // Try to show rewarded video
        if (AdToolbox::hasCachedRewardedVideo() && !m_videoPlaying)
        {
            m_videoPlaying = true;
            m_gauntletID = packId;
            GameManager::sharedState()->m_rewardedVideoDelegate = this;
            GameManager::sharedState()->m_unk664 = packId;
            AdToolbox::showRewardedVideo();
        }
        else
        {
            std::string msg = "The <cg>Gauntlet</c> cannot be unlocked right now.\nPlease try again later.";
            FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Unlock Gauntlet", msg);
            alert->show();
        }
        return;
    }

    // Check if gauntlet is completed and chest not claimed
    if (pack->hasCompletedMapPack() && !GameStatsManager::sharedState()->isGauntletChestUnlocked(packId))
    {
        // Show reward claim
        GauntletNode* node = (GauntletNode*)m_gauntlets->objectForKey(packId);
        node->runAction(cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.5f),
            cocos2d::CCCallFunc::create(node, callfunc_selector(GauntletNode::onClaimReward)),
            nullptr
        ));

        GJRewardItem* reward = GameStatsManager::sharedState()->unlockGauntletChest(packId);
        if (reward)
        {
            RewardUnlockLayer* unlockLayer = RewardUnlockLayer::create(reward->m_rewardType, 0);
            addChild(unlockLayer, 100);
            unlockLayer->showCollectReward(reward);
        }
        return;
    }

    // Play the gauntlet
    m_playing = true;
    GameManager::sharedState()->m_currentGauntlet = packId;
    cocos2d::CCDirector::sharedDirector()->replaceScene(
        cocos2d::CCTransitionFade::create(0.5f, GauntletLayer::scene((GauntletType)packId))
    );
}

void GauntletSelectLayer::rewardedVideoFinished()
{
    m_videoPlaying = false;
    m_locked = true;
    this->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.1f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GauntletSelectLayer::playUnlockAnimation)),
        nullptr
    ));
}

void GauntletSelectLayer::playUnlockAnimation()
{
    GauntletNode* node = (GauntletNode*)m_gauntlets->objectForKey(m_gauntletID);
    node->showUnlockAnimation();

    // Play sound
    FMODAudioEngine::sharedEngine()->fadeOutMusic(0.5f, 0);
    FMODAudioEngine::sharedEngine()->playEffect("unlockGauntlet.ogg", 0.0f, 0.0f, 0.5f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    // Particle effect 1
    std::string particleData = "84a1.05a1.25a0.16a50a180a180a29a0a11a0a0a0a0a0a0a0a2a1a0a0a1a0a0a0a0.360784a0a1a0a1a1a0a0a1a0a0a0a0a0a1a0a0.8a0a0a0a110a0a0a0a0a0a1a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
    cocos2d::CCParticleSystemQuad* particles = GameToolbox::particleFromString(particleData, 0, 0);
    particles->setEndRadius(10.0f);
    particles->setEndRadiusVar(5.0f);
    particles->setLife(particles->getLife() * 0.7f);
    particles->setDuration(particles->getDuration() * 0.7f);
    particles->setAutoRemoveOnFinish(true);
    particles->setScale(2.0f);
    addChild(particles, 11);
    particles->resetSystem();

    // Position particles at node
    cocos2d::CCPoint worldPos = node->getParent()->convertToWorldSpace(node->getPosition());
    cocos2d::CCPoint nodePos = this->convertToNodeSpace(worldPos);
    particles->setPosition(nodePos);

    // Set particle colors
    cocos2d::ccColor4F startColor = cocos2d::ccc4f(
        node->m_labelColor.r / 255.0f,
        node->m_labelColor.g / 255.0f,
        node->m_labelColor.b / 255.0f,
        1.0f
    );
    particles->setStartColor(startColor);

    cocos2d::ccColor4F endColor = cocos2d::ccc4f(
        node->m_labelColor.r / 255.0f,
        node->m_labelColor.g / 255.0f,
        node->m_labelColor.b / 255.0f,
        1.0f
    );
    particles->setEndColor(endColor);

    // Schedule next steps
    this->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.25f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GauntletSelectLayer::unlockAnimationStep2)),
        nullptr
    ));

    this->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.65f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GauntletSelectLayer::unlockAnimationFinished)),
        nullptr
    ));
}

void GauntletSelectLayer::unlockAnimationStep2()
{
    GauntletNode* node = (GauntletNode*)m_gauntlets->objectForKey(m_gauntletID);

    // Particle effect 2
    std::string particleData = "84a0.97a1a0.16a72a180a180a29a0a11a0a0a0a0a0a0a0a2a1a0a0a1a0a0a0a0a0a1a0a1a1a0a0a1a0a0.568627a0a0a0a1a0a0.8a0a0.2a0a150a0a0a0a0a0a1a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
    cocos2d::CCParticleSystemQuad* particles = GameToolbox::particleFromString(particleData, 0, 0);
    particles->setEndRadius(10.0f);
    particles->setEndRadiusVar(10.0f);
    particles->setLife(particles->getLife() * 0.8f);
    particles->setDuration(particles->getDuration() * 0.8f);
    particles->setAutoRemoveOnFinish(true);
    particles->setScale(2.0f);
    addChild(particles, 11);
    particles->resetSystem();

    // Position at node
    cocos2d::CCPoint worldPos = node->getParent()->convertToWorldSpace(node->getPosition());
    cocos2d::CCPoint nodePos = this->convertToNodeSpace(worldPos);
    particles->setPosition(nodePos);

    // Set colors
    cocos2d::ccColor4F startColor = cocos2d::ccc4f(
        node->m_labelColor.r / 255.0f,
        node->m_labelColor.g / 255.0f,
        node->m_labelColor.b / 255.0f,
        1.0f
    );
    particles->setStartColor(startColor);
    particles->setEndColor(startColor);
}

void GauntletSelectLayer::unlockAnimationStep3()
{
    GauntletNode* node = (GauntletNode*)m_gauntlets->objectForKey(m_gauntletID);

    // Particle effect 3
    std::string particleData = "100a0a0.88a0.21a-1a90a180a5a5a48a82a0a0a0a0a0a0a7a2a0a360a0.411765a1a1a1a0a1a1a0a3a2a0a60a0a0a0.737255a0a0.160784a0a1a0a0.16a0.1a0.51a0.12a0a0a0a0a0a0a0a2a1a1a0a0a166a0a0a0a0a0a0a0a0a1a1a0a0a0a0";
    cocos2d::CCParticleSystemQuad* particles = GameToolbox::particleFromString(particleData, 0, 0);
    particles->setAutoRemoveOnFinish(true);
    particles->setScale(1.6f);
    particles->setPosVar(cocos2d::CCPoint(30.0f, 70.0f));
    addChild(particles, 15);
    particles->resetSystem();

    // Position at node
    cocos2d::CCPoint worldPos = node->getParent()->convertToWorldSpace(node->getPosition());
    cocos2d::CCPoint nodePos = this->convertToNodeSpace(worldPos);
    particles->setPosition(nodePos);

    // Set colors
    cocos2d::ccColor4F color = cocos2d::ccc4f(
        node->m_labelColor.r / 255.0f,
        node->m_labelColor.g / 255.0f,
        node->m_labelColor.b / 255.0f,
        1.0f
    );
    particles->setStartColor(color);
    particles->setEndColor(color);

    // Particle effect 4
    std::string particleData2 = "30a0a1.15a0.3a-1a110a97a0a8a44a71a0a0a0a0a0a0a75a12a0a0a1a0.4a0.890196a0.4a0a0.4a0.51a0.22a-5a1a0a0a1a0a0a0a0.0784314a0a0a0a0.16a0a0.25a0a0a0a0a0a0a0a0a2a0a0a0a0a159a0a0.35a0a0a0a0a1a0a1a0a0a0a0a0";
    cocos2d::CCParticleSystemQuad* particles2 = GameToolbox::particleFromString(particleData2, 0, 0);
    particles2->setAutoRemoveOnFinish(true);
    particles2->setScale(2.2f);
    particles2->setPosVar(cocos2d::CCPoint(10.0f, 40.0f));
    addChild(particles2, 16);
    particles2->resetSystem();
    particles2->setPosition(nodePos);

    cocos2d::ccColor4F color2 = cocos2d::ccc4f(
        node->m_labelColor.r / 255.0f,
        node->m_labelColor.g / 255.0f,
        node->m_labelColor.b / 255.0f,
        0.0f
    );
    particles2->setStartColor(color);
    particles2->setEndColor(color);

    // Fade in music
    FMODAudioEngine::sharedEngine()->fadeInMusic(4.0f, 0);
}

void GauntletSelectLayer::unlockAnimationFinished()
{
    m_locked = false;
    showUnlockGauntlet();
    unlockAnimationStep3();
}

void GauntletSelectLayer::showUnlockGauntlet()
{
    GauntletNode* node = (GauntletNode*)m_gauntlets->objectForKey(m_gauntletID);
    node->onUnlock();
}

GauntletSelectLayer::~GauntletSelectLayer()
{
    if (m_gauntlets)
        m_gauntlets->release();
    if (m_loadingCircle)
        m_loadingCircle->release();
}

cocos2d::CCScene* GauntletSelectLayer::scene(int unused)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    GauntletSelectLayer* layer = GauntletSelectLayer::create(unused);
    scene->addChild(layer);
    return scene;
}