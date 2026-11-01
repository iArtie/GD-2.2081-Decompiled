// GJGarageLayer.cpp
#include "Layers/GJGarageLayer.h"
#include "Managers/GameManager.h"
#include "Managers/AchievementManager.h"
#include "Managers/GameStatsManager.h"
#include "Managers/GJAccountManager.h"
#include "Sprites/SimplePlayer.h"
#include "Editor/ListButtonBar.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"
#include "Layers/FLAlertLayer.h"
#include "Layers/DialogLayer.h"
#include "Pages/CharacterColorPage.h"
#include "Pages/ShardsPage.h"
#include "Layers/GJShopLayer.h"
#include "Layers/CurrencyRewardLayer.h"
#include "UI/GJItemIcon.h"
#include "Cocos/CCCircleWave.h"
#include "Popups/ItemInfoPopup.h"

void GJGarageLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2) {
        return;
    }
    
    int tag = layer->getTag();
    GameManager* manager = GameManager::sharedState();
    manager->m_gameRateDelegate1 = this;
    
    if (tag == 1) {
        manager->rateGame();
    } else if (tag == 2) {
        manager->likeFacebook();
    } else if (tag == 3) {
        manager->followTwitter();
    } else if (tag == 4) {
        manager->subYouTube();
    }
}

std::string GJGarageLayer::achievementForUnlock(int id, UnlockType type) {
    std::string result;
    AchievementManager* manager = AchievementManager::sharedState();
    std::string achievementKey = manager->achievementForUnlock(id, type);
    
    if (manager->isAchievementAvailable(achievementKey)) {
        result = achievementKey;
    } else {
        result = "";
    }
    
    return result;
}

std::string GJGarageLayer::descriptionForUnlock(int id, UnlockType type) {
    std::string result;
    AchievementManager* manager = AchievementManager::sharedState();
    std::string achievementKey = manager->achievementForUnlock(id, type);
    cocos2d::CCDictionary* dict = manager->getAchievementsWithID(achievementKey);
    
    cocos2d::CCString* desc = static_cast<cocos2d::CCString*>(dict->objectForKey("unachievedDescription"));
    result = desc->getCString();
    
    return result;
}

void GJGarageLayer::dialogClosed(DialogLayer* layer) {
    GameManager* manager = GameManager::sharedState();
    manager->reportAchievementWithID("geometry.ach.rate", 100, false);
    manager->m_ratedGame = true;
    manager->m_hasRatedGame = true;
    updateRate();
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f);
    
    CurrencyRewardLayer* rewardLayer = CurrencyRewardLayer::create(0, 0, 0, 0, IconType::Icon, 0, IconType::Icon, 0, cocos2d::CCPoint(0.0f, 1.0f), CurrencyRewardType::Center, 0.0f, 0.0f);
    addChild(rewardLayer, 100);
    
    GJItemIcon* icon = GJItemIcon::createBrowserItem(UnlockType::Item, 13);
    rewardLayer->createUnlockObject(icon, cocos2d::CCPoint(0.5f, 0.5f), 0.0f);
}

cocos2d::CCArray* GJGarageLayer::getItems(IconType type) {
    GameManager* manager = GameManager::sharedState();
    int count = manager->countForType(type);
    int active = manager->activeIconForType(type);
    return getItems(count, 0, type, active);
}

std::string GJGarageLayer::getLockFrame(int id, UnlockType type) {
    return "GJ_lock_001";
}

bool GJGarageLayer::init() {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }
    
    GameManager* manager = GameManager::sharedState();
    manager->m_inMenuLayer = true;
    manager->m_sceneEnum = 0;
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background gradient
    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPointZero);
    addChild(bg, -2);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().height);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().width);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(23, 27, 38));
    
    // Side art
    cocos2d::CCSprite* sideArt1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt1->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    sideArt1->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f, cocos2d::CCDirector::sharedDirector()->getScreenTop() + 1.0f));
    sideArt1->setFlipY(true);
    addChild(sideArt1, -1);
    
    cocos2d::CCSprite* sideArt2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt2->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    sideArt2->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f, cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    addChild(sideArt2, 1);
    
    cocos2d::CCSprite* sideArt3 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArt3->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    sideArt3->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f, cocos2d::CCDirector::sharedDirector()->getScreenBottom() - 1.0f));
    sideArt3->setFlipX(true);
    addChild(sideArt3, 1);
    
    // Username input
    m_usernameInput = CCTextInputNode::create(180.0f, 50.0f, "Player", "Thonburi");
    m_usernameInput->m_delegate = this;
    m_usernameInput->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 34.0f));
    m_usernameInput->m_textField->setVisible(false);
    m_usernameInput->m_maxLabelLength = 15;
    m_usernameInput->setLabelPlaceholderColor(cocos2d::ccc3(120, 120, 120));
    m_usernameInput->setLabelPlaceholderScale(0.7f);
    m_usernameInput->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    addChild(m_usernameInput, 20);
    
    // Set username text
    GJAccountManager* accountManager = GJAccountManager::sharedState();
    if (accountManager->m_accountID == accountManager->m_unkInt1) {
        // Player name from GameManager
        std::string playerName = GameManager::sharedState()->m_playerName;
        if (playerName != "") {
            m_usernameInput->setString(playerName);
        }
        if (!GameManager::sharedState()->m_clickedName) {
            cocos2d::CCSprite* nameTxt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_nameTxt_001.png");
            addChild(nameTxt);
            cocos2d::CCPoint pos = m_usernameInput->getPosition();
            nameTxt->setPosition(cocos2d::CCPoint(pos.x + 115.0f, pos.y + 10.0f));
        }
    } else {
        // Account username
        m_usernameInput->setString(accountManager->m_username);
        m_usernameInput->setTouchEnabled(false);
        cocos2d::CCSprite* lockIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
        addChild(lockIcon, 2);
        cocos2d::CCSize labelSize = m_usernameInput->m_textLabel->getContentSize();
        cocos2d::CCPoint pos = m_usernameInput->getPosition();
        lockIcon->setPosition(cocos2d::CCPoint(pos.x + labelSize.width * 0.5f + 8.0f, pos.y + 10.0f));
        lockIcon->setScale(0.6f);
    }
    
    // Floor line
    cocos2d::CCSprite* floorLine = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
    addChild(floorLine, 0);
    floorLine->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f));
    floorLine->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
    
    // Player object
    int playerFrame = manager->m_playerFrame.random - manager->m_playerFrame.seed;
    m_playerObject = SimplePlayer::create(playerFrame);
    addChild(m_playerObject, 0);
    m_playerObject->createRobotSprite(true);
    m_playerObject->createSpiderSprite(true);
    
    float scale = (manager->m_playerIconType == IconType::Jetpack) ? 1.5f : 1.2f;
    m_playerObject->setScale(scale);
    cocos2d::CCPoint floorPos = floorLine->getPosition();
    m_playerObject->setPosition(cocos2d::CCPoint(floorPos.x, floorPos.y + (scale * 30.0f) * 0.5f + 1.0f));
    
    updatePlayerColors();
    
    // Update player frame based on icon type
    switch (manager->m_playerIconType) {
        case IconType::Ship:
            m_playerObject->updatePlayerFrame(manager->m_playerShip.random - manager->m_playerShip.seed, IconType::Ship);
            break;
        case IconType::Ball:
            m_playerObject->updatePlayerFrame(manager->m_playerBall.random - manager->m_playerBall.seed, IconType::Ball);
            break;
        case IconType::Ufo:
            m_playerObject->updatePlayerFrame(manager->m_playerBird.random - manager->m_playerBird.seed, IconType::Ufo);
            break;
        case IconType::Wave:
            m_playerObject->updatePlayerFrame(manager->m_playerDart.random - manager->m_playerDart.seed, IconType::Wave);
            break;
        case IconType::Robot:
            m_playerObject->updatePlayerFrame(manager->m_playerRobot.random - manager->m_playerRobot.seed, IconType::Robot);
            break;
        case IconType::Spider:
            m_playerObject->updatePlayerFrame(manager->m_playerSpider.random - manager->m_playerSpider.seed, IconType::Spider);
            break;
        case IconType::Swing:
            m_playerObject->updatePlayerFrame(manager->m_playerSwing.random - manager->m_playerSwing.seed, IconType::Swing);
            break;
        case IconType::Jetpack:
            m_playerObject->updatePlayerFrame(manager->m_playerJetpack.random - manager->m_playerJetpack.seed, IconType::Jetpack);
            break;
        default:
            break;
    }
    
    setupIconSelect();
    
    // Back button
    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onBack));
    backBtn->setSizeMult(1.6f);
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(backBtn, nullptr);
    addChild(menu, 1);
    menu->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));
    
    // Shop button
    cocos2d::CCSprite* shopSprite = cocos2d::CCSprite::createWithSpriteFrameName("shopRope_001.png");
    CCMenuItemSpriteExtra* shopBtn = CCMenuItemSpriteExtra::create(shopSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onShop));
    shopBtn->useAnimationType(AnimationType::Move);
    shopBtn->setOffset(cocos2d::CCPoint(0.0f, -15.0f));
    shopBtn->setDuration(0.3f);
    shopBtn->setUnselectedDuration(0.3f);
    menu->addChild(shopBtn);
    shopBtn->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 135.0f, winSize.height - 25.0f));
    
    // Shards button
    cocos2d::CCSprite* shardsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_shardsBtn_001.png");
    CCMenuItemSpriteExtra* shardsBtn = CCMenuItemSpriteExtra::create(shardsSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onShards));
    menu->addChild(shardsBtn);
    shardsBtn->setPosition(cocos2d::CCPoint(30.0f, winSize.height - 80.0f));
    
    // Paint button
    cocos2d::CCSprite* paintSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png");
    CCMenuItemSpriteExtra* paintBtn = CCMenuItemSpriteExtra::create(paintSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onPaint));
    menu->addChild(paintBtn);
    paintBtn->setPosition(cocos2d::CCPoint(30.0f, winSize.height - 120.0f));
    
    // Stats display - stars
    cocos2d::CCSprite* starsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    starsIcon->setScale(0.54f);
    addChild(starsIcon);
    starsIcon->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 18.0f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 12.0f));
    
    GameStatsManager* statsManager = GameStatsManager::sharedState();
    int stars = statsManager->getStat("6");
    cocos2d::CCLabelBMFont* starsLabel = cocos2d::CCLabelBMFont::create();
    addChild(starsLabel);
    starsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    starsLabel->setPosition(cocos2d::CCPoint(starsIcon->getPosition().x - 12.0f, starsIcon->getPosition().y + 0.5f));
    starsLabel->setScale(0.34f);
    starsLabel->setString(cocos2d::CCString::createWithFormat("%i", stars)->getCString());
    
    // Moons
    cocos2d::CCSprite* moonsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
    moonsIcon->setScale(0.54f);
    addChild(moonsIcon);
    moonsIcon->setPosition(cocos2d::CCPoint(starsIcon->getPosition().x, starsIcon->getPosition().y - 15.0f));
    
    int moons = statsManager->getStat("28");
    cocos2d::CCLabelBMFont* moonsLabel = cocos2d::CCLabelBMFont::create();
    addChild(moonsLabel);
    moonsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    moonsLabel->setPosition(cocos2d::CCPoint(moonsIcon->getPosition().x - 12.0f, moonsIcon->getPosition().y + 0.5f));
    moonsLabel->setScale(0.34f);
    moonsLabel->setString(cocos2d::CCString::createWithFormat("%i", moons)->getCString());
    
    // Coins
    cocos2d::CCSprite* coinsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
    addChild(coinsIcon);
    coinsIcon->setPosition(cocos2d::CCPoint(moonsIcon->getPosition().x, moonsIcon->getPosition().y - 15.0f));
    coinsIcon->setScale(0.51f);
    
    int coins = statsManager->getStat("8");
    cocos2d::CCLabelBMFont* coinsLabel = cocos2d::CCLabelBMFont::create();
    addChild(coinsLabel);
    coinsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    coinsLabel->setPosition(cocos2d::CCPoint(coinsIcon->getPosition().x - 12.0f, coinsIcon->getPosition().y + 0.5f));
    coinsLabel->setScale(0.34f);
    coinsLabel->setString(cocos2d::CCString::createWithFormat("%i", coins)->getCString());
    
    // User coins
    cocos2d::CCSprite* userCoinsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
    addChild(userCoinsIcon);
    userCoinsIcon->setPosition(cocos2d::CCPoint(coinsIcon->getPosition().x, coinsIcon->getPosition().y - 15.0f));
    userCoinsIcon->setScale(0.51f);
    
    int userCoins = statsManager->getStat("12");
    cocos2d::CCLabelBMFont* userCoinsLabel = cocos2d::CCLabelBMFont::create();
    addChild(userCoinsLabel);
    userCoinsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    userCoinsLabel->setPosition(cocos2d::CCPoint(userCoinsIcon->getPosition().x - 12.0f, userCoinsIcon->getPosition().y + 0.5f));
    userCoinsLabel->setScale(0.34f);
    userCoinsLabel->setString(cocos2d::CCString::createWithFormat("%i", userCoins)->getCString());
    
    // Orbs
    cocos2d::CCSprite* orbsIcon = cocos2d::CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
    orbsIcon->setScale(0.54f);
    addChild(orbsIcon);
    orbsIcon->setPosition(cocos2d::CCPoint(userCoinsIcon->getPosition().x, userCoinsIcon->getPosition().y - 15.0f));
    
    int orbs = statsManager->getStat("14");
    cocos2d::CCLabelBMFont* orbsLabel = cocos2d::CCLabelBMFont::create();
    addChild(orbsLabel);
    orbsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    orbsLabel->setPosition(cocos2d::CCPoint(orbsIcon->getPosition().x - 12.0f, orbsIcon->getPosition().y + 0.5f));
    orbsLabel->setScale(0.34f);
    orbsLabel->setString(cocos2d::CCString::createWithFormat("%i", orbs)->getCString());
    
    // Diamonds
    cocos2d::CCSprite* diamondsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_diamondsIcon_001.png");
    diamondsIcon->setScale(0.6f);
    addChild(diamondsIcon);
    diamondsIcon->setPosition(cocos2d::CCPoint(orbsIcon->getPosition().x, orbsIcon->getPosition().y - 15.0f));
    
    int diamonds = statsManager->getStat("13");
    cocos2d::CCLabelBMFont* diamondsLabel = cocos2d::CCLabelBMFont::create();
    addChild(diamondsLabel);
    diamondsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    diamondsLabel->setPosition(cocos2d::CCPoint(diamondsIcon->getPosition().x - 12.0f, diamondsIcon->getPosition().y + 0.5f));
    diamondsLabel->setScale(0.34f);
    diamondsLabel->setString(cocos2d::CCString::createWithFormat("%i", diamonds)->getCString());
    
    // Diamond currency
    cocos2d::CCSprite* diamondCurrencyIcon = cocos2d::CCSprite::createWithSpriteFrameName("currencyDiamondIcon_001.png");
    diamondCurrencyIcon->setScale(0.54f);
    addChild(diamondCurrencyIcon);
    diamondCurrencyIcon->setPosition(cocos2d::CCPoint(diamondsIcon->getPosition().x, diamondsIcon->getPosition().y - 15.0f));
    
    int diamondCurrency = statsManager->getStat("29");
    cocos2d::CCLabelBMFont* diamondCurrencyLabel = cocos2d::CCLabelBMFont::create();
    addChild(diamondCurrencyLabel);
    diamondCurrencyLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    diamondCurrencyLabel->setPosition(cocos2d::CCPoint(diamondCurrencyIcon->getPosition().x - 12.0f, diamondCurrencyIcon->getPosition().y + 0.5f));
    diamondCurrencyLabel->setScale(0.34f);
    diamondCurrencyLabel->setString(cocos2d::CCString::createWithFormat("%i", diamondCurrency)->getCString());
    
    // Info button
    if (manager->m_playerUserID.random != manager->m_playerUserID.seed) {
        cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onInfo));
        infoBtn->setSizeMult(2.0f);
        menu->addChild(infoBtn);
        cocos2d::CCPoint infoPos = infoBtn->convertToNodeSpace(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 15.0f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 15.0f));
        infoBtn->setPosition(infoPos);
    }
    
    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backBtn);
    }
    
    return true;
}

void GJGarageLayer::keyBackClicked() {
    onBack(nullptr);
}

void GJGarageLayer::listButtonBarSwitchedPage(ListButtonBar* bar, int page) {
    bar->getTag();
}

void GJGarageLayer::onArrow(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    GameManager* manager = GameManager::sharedState();
    int active = manager->activeIconForType(m_iconType);
    int count = manager->countForType(m_iconType);
    
    // Get or create page for icon type
    int currentPage = 0;
    auto it = m_iconPages.find(m_iconType);
    if (it != m_iconPages.end()) {
        currentPage = it->second;
    } else {
        m_iconPages[m_iconType] = 0;
    }
    
    int newPage = currentPage;
    if (tag == 1) {
        newPage = currentPage + 1;
    } else {
        newPage = currentPage - 1;
    }
    
    if (newPage < 0) {
        newPage = count / 36;
    }
    if (newPage > count / 36) {
        newPage = 0;
    }
    
    m_iconPages[m_iconType] = newPage;
    setupPage(newPage, m_iconType);
}

void GJGarageLayer::onBack(cocos2d::CCObject* sender) {
    if (m_hasClosed) {
        return;
    }
    
    setKeypadEnabled(false);
    
    if (GameManager::sharedState()->m_ropeGarageEnter) {
        GameManager::sharedState()->m_ropeGarageEnter = false;
        retain();
        removeFromParent();
        setKeypadEnabled(false);
        
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        director->popScene();
        
        cocos2d::CCScene* nextScene = director->getNextScene();
        if (nextScene) {
            m_hasClosed = true;
            nextScene->addChild(this, 1000);
            release();
            
            cocos2d::CCSize winSize = director->getWinSize();
            cocos2d::CCAction* moveAction = cocos2d::CCSequence::create(
                cocos2d::CCEaseIn::create(cocos2d::CCMoveTo::create(0.3f, cocos2d::CCPoint(0.0f, winSize.height)), 2.0f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(GJGarageLayer::removeFromParent)),
                nullptr
            );
            runAction(moveAction);
        }
    } else {
        m_hasClosed = true;
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCScene* scene = MenuLayer::scene(false);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        director->replaceScene(transition);
    }
}

void GJGarageLayer::onInfo(cocos2d::CCObject* sender) {
    GameManager* manager = GameManager::sharedState();
    int userID = manager->m_playerUserID.random - manager->m_playerUserID.seed;
    
    cocos2d::CCString* text = cocos2d::CCString::createWithFormat("<cy>UserID:</c> %i", userID);
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "User Info", text->getCString());
    alert->show();
}

void GJGarageLayer::onNavigate(cocos2d::CCObject* sender) {
    int page = sender->getTag();
    setupPage(page, m_iconType);
}

void GJGarageLayer::onPaint(cocos2d::CCObject* sender) {
    CharacterColorPage* colorPage = CharacterColorPage::create();
    colorPage->m_scene = this;
    colorPage->m_delegate = this;
    colorPage->show();
    
    if (m_iconSelection) {
        m_iconSelection->setVisible(false);
    }
}

void GJGarageLayer::onRewardedVideo(cocos2d::CCObject* sender) {
    if (m_videoPlaying || !AdToolbox::hasCachedRewardedVideo()) {
        return;
    }
    
    GameManager* manager = GameManager::sharedState();
    if (manager->canShowRewardedVideo()) {
        m_videoPlaying = true;
        manager->m_rewardedVideoDelegate = this;
        AdToolbox::showRewardedVideo();
    } else {
        GameStatsManager* statsManager = GameStatsManager::sharedState();
        std::string message = statsManager->getNextVideoAdReward();
        
        if (message.empty()) {
            // Create dialog with multiple messages
            cocos2d::CCArray* dialogs = cocos2d::CCArray::create();
            
            // Dialog 1
            DialogObject* d1 = DialogObject::create("Scratch", "I don't have any <co>chests</c> left<d010>.<d010>.<d010>.", 12, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d1);
            
            // Dialog 2
            DialogObject* d2 = DialogObject::create("Scratch", "You took <cr>EVERYTHING</c> I had!", 14, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d2);
            
            // Dialog 3
            DialogObject* d3 = DialogObject::create("Scratch", "I need to find <cy>new stuff</c>...", 13, 1.0f, false, cocos2d::ccc3(255, 255, 255));
            dialogs->addObject(d3);
            
            DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
            addChild(dialogLayer, 100);
            dialogLayer->updateChatPlacement(PlacementType::Center);
            dialogLayer->animateInRandomSide();
        }
    }
}

void GJGarageLayer::onSelect(cocos2d::CCObject* sender) {
    int id = sender->getTag();
    IconType type = static_cast<IconType>(sender->getZOrder());
    
    GameManager* manager = GameManager::sharedState();
    
    if (!manager->isIconUnlocked(id, type)) {
        UnlockType unlockType = manager->iconTypeToUnlockType(type);
        showUnlockPopup(id, unlockType);
        return;
    }
    
    manager->m_scoreValid = false;
    
    // Update the appropriate icon in GameManager
    switch (type) {
        case IconType::Cube:
            manager->m_playerFrame.value = id;
            manager->m_playerFrame.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerFrame.random = id + manager->m_playerFrame.seed;
            break;
        case IconType::Ship:
            manager->m_playerShip.value = id;
            manager->m_playerShip.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerShip.random = id + manager->m_playerShip.seed;
            break;
        case IconType::Ball:
            manager->m_playerBall.value = id;
            manager->m_playerBall.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerBall.random = id + manager->m_playerBall.seed;
            break;
        case IconType::Ufo:
            manager->m_playerBird.value = id;
            manager->m_playerBird.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerBird.random = id + manager->m_playerBird.seed;
            break;
        case IconType::Wave:
            manager->m_playerDart.value = id;
            manager->m_playerDart.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerDart.random = id + manager->m_playerDart.seed;
            break;
        case IconType::Robot:
            manager->m_playerRobot.value = id;
            manager->m_playerRobot.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerRobot.random = id + manager->m_playerRobot.seed;
            break;
        case IconType::Spider:
            manager->m_playerSpider.value = id;
            manager->m_playerSpider.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerSpider.random = id + manager->m_playerSpider.seed;
            break;
        case IconType::Swing:
            manager->m_playerSwing.value = id;
            manager->m_playerSwing.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerSwing.random = id + manager->m_playerSwing.seed;
            break;
        case IconType::Jetpack:
            manager->m_playerJetpack.value = id;
            manager->m_playerJetpack.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerJetpack.random = id + manager->m_playerJetpack.seed;
            break;
        case IconType::DeathEffect:
            manager->m_playerDeathEffect.value = id;
            manager->m_playerDeathEffect.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerDeathEffect.random = id + manager->m_playerDeathEffect.seed;
            break;
        case IconType::Special:
            manager->m_playerStreak.value = id;
            manager->m_playerStreak.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerStreak.random = id + manager->m_playerStreak.seed;
            break;
        case IconType::ShipFire:
            manager->m_playerShipFire.value = id;
            manager->m_playerShipFire.seed = static_cast<int>(rand() * 4.6566e-10f * 1000.0f);
            manager->m_playerShipFire.random = id + manager->m_playerShipFire.seed;
            break;
        default:
            break;
    }
    
    m_playerObject->updateColors();
    
    if (id == 2 && type == IconType::Special) {
        playRainbowEffect();
    }
    
    // Update cursor position
    cocos2d::CCNode* parent = sender->getParent();
    cocos2d::CCPoint worldPos = parent->convertToWorldSpace(sender->getPosition());
    m_cursor1->setPosition(worldPos);
    m_cursor1->setVisible(true);
    
    if (static_cast<int>(type) <= static_cast<int>(IconType::Jetpack)) {
        m_playerObject->updatePlayerFrame(id, m_iconType);
        manager->m_playerIconType = m_iconType;
        
        float scale = (manager->m_playerIconType == IconType::Jetpack) ? 1.5f : 1.2f;
        m_playerObject->setScale(scale);
    }
    
    m_iconID = id;
    m_selectedIconType = type;
    
    if (id == m_iconID && type == m_selectedIconType) {
        UnlockType unlockType = manager->iconTypeToUnlockType(type);
        showUnlockPopup(id, unlockType);
    }
}

void GJGarageLayer::onSelectTab(cocos2d::CCObject* sender) {
    IconType type = static_cast<IconType>(sender->getTag());
    selectTab(type);
}

void GJGarageLayer::onShards(cocos2d::CCObject* sender) {
    ShardsPage* shardsPage = ShardsPage::create();
    shardsPage->m_scene = this;
    shardsPage->show();
}

void GJGarageLayer::onShop(cocos2d::CCObject* sender) {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCScene* scene = GJShopLayer::scene(ShopType::Default);
    cocos2d::CCTransitionMoveInT* transition = cocos2d::CCTransitionMoveInT::create(0.5f, scene);
    director->replaceScene(transition);
    setKeypadEnabled(false);
}

void GJGarageLayer::onSpecial(cocos2d::CCObject* sender) {
    GameManager::sharedState()->toggleGameVariable("0153");
}

void GJGarageLayer::onToggleItem(cocos2d::CCObject* sender) {
    int id = sender->getTag();
    IconType type = static_cast<IconType>(sender->getZOrder());
    
    GameManager* manager = GameManager::sharedState();
    UnlockType unlockType = manager->iconTypeToUnlockType(type);
    
    if (manager->isIconUnlocked(id, type)) {
        GameStatsManager* statsManager = GameStatsManager::sharedState();
        bool enable = !statsManager->isItemEnabled(unlockType, id);
        statsManager->toggleEnableItem(unlockType, id, enable);
        
        GJItemIcon* icon = static_cast<GJItemIcon*>(sender->getParent());
        icon->toggleEnabledState(enable);
    } else {
        showUnlockPopup(id, unlockType);
    }
}

void GJGarageLayer::playRainbowEffect() {
    CCCircleWave* wave1 = CCCircleWave::create(4.0f, 64.0f, 0.5f, false);
    wave1->m_color = cocos2d::ccc3(255, 0, 0);
    wave1->m_blendAdditive = true;
    wave1->m_opacityMod = 1.0f;
    wave1->setPosition(m_playerObject->getPosition());
    addChild(wave1, -1);
    wave1->setTag(4);
    wave1->setID(1);
    
    CCCircleWave* wave2 = CCCircleWave::create(1.0f, 60.0f, 0.5f, false);
    wave2->m_color = cocos2d::ccc3(0, 255, 0);
    wave2->m_blendAdditive = true;
    wave2->m_opacityMod = 0.8f;
    wave2->setPosition(m_playerObject->getPosition());
    addChild(wave2, -1);
    wave2->setTag(4);
    wave2->setID(1);
    
    CCCircleWave* wave3 = CCCircleWave::create(20.0f, 60.0f, 0.5f, false);
    wave3->m_color = cocos2d::ccc3(0, 0, 255);
    wave3->m_blendAdditive = true;
    wave3->m_opacityMod = 1.0f;
    wave3->setPosition(m_playerObject->getPosition());
    addChild(wave3, -1);
    wave3->setTag(4);
}

void GJGarageLayer::playShadowEffect() {
    CCCircleWave* wave = CCCircleWave::create(20.0f, 60.0f, 0.3f, false);
    wave->m_color = cocos2d::ccc3(0, 0, 0);
    wave->m_blendAdditive = false;
    wave->m_opacityMod = 0.5f;
    wave->setPosition(m_playerObject->getPosition());
    addChild(wave, -1);
}

void GJGarageLayer::playerColorChanged() {
    if (m_iconSelection) {
        m_iconSelection->setVisible(true);
    }
    updatePlayerColors();
}

void GJGarageLayer::rewardedVideoFinished() {
    m_videoPlaying = false;
}

cocos2d::CCScene* GJGarageLayer::scene() {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    GJGarageLayer* layer = GJGarageLayer::create();
    scene->addChild(layer);
    return scene;
}

void GJGarageLayer::selectTab(IconType type) {
    // Update tab buttons
    for (int i = 0; i < m_tabButtons->count(); ++i) {
        CCMenuItemToggler* toggler = static_cast<CCMenuItemToggler*>(m_tabButtons->objectAtIndex(i));
        int tag = toggler->getTag();
        toggler->toggle(tag == static_cast<int>(type));
        toggler->setEnabled(tag != static_cast<int>(type));
    }
    
    // Update or create page entry
    int page = 0;
    auto it = m_iconPages.find(type);
    if (it != m_iconPages.end()) {
        page = it->second;
    } else {
        m_iconPages[type] = 0;
    }
    
    setupPage(page, type);
}

void GJGarageLayer::setupIconSelect() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background panel
    cocos2d::extension::CCScale9Sprite* panel = cocos2d::extension::CCScale9Sprite::create("square02_001.png");
    panel->setColor(cocos2d::ccc3(75, 75, 75));
    panel->setContentSize(cocos2d::CCSize(385.0f, 100.0f));
    addChild(panel, -1);
    panel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 60.0f));
    
    // "Unlock" label
    cocos2d::CCSprite* unlockLabel = cocos2d::CCSprite::createWithSpriteFrameName("GJ_unlockTxt_001.png");
    unlockLabel->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    addChild(unlockLabel);
    cocos2d::CCPoint panelPos = panel->getPosition();
    unlockLabel->setPosition(cocos2d::CCPoint(panelPos.x, panelPos.y + panel->getContentSize().height * 0.5f + 12.0f));
    
    // Tab buttons
    m_tabButtons = cocos2d::CCArray::create();
    m_tabButtons->retain();
    
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    addChild(menu, 100);
    
    // Create all tab buttons
    struct TabInfo {
        const char* offSprite;
        const char* onSprite;
        int tag;
    };
    
    TabInfo tabs[] = {
        {"gj_iconBtn_off_001.png", "gj_iconBtn_on_001.png", 0},
        {"gj_shipBtn_off_001.png", "gj_shipBtn_on_001.png", 1},
        {"gj_ballBtn_off_001.png", "gj_ballBtn_on_001.png", 2},
        {"gj_birdBtn_off_001.png", "gj_birdBtn_on_001.png", 3},
        {"gj_dartBtn_off_001.png", "gj_dartBtn_on_001.png", 4},
        {"gj_robotBtn_off_001.png", "gj_robotBtn_on_001.png", 5},
        {"gj_spiderBtn_off_001.png", "gj_spiderBtn_on_001.png", 6},
        {"gj_swingBtn_off_001.png", "gj_swingBtn_on_001.png", 7},
        {"gj_jetpackBtn_off_001.png", "gj_jetpackBtn_on_001.png", 8},
        {"gj_streakBtn_off_001.png", "gj_streakBtn_on_001.png", 99},
        {"gj_explosionBtn_off_001.png", "gj_explosionBtn_on_001.png", 98}
    };
    
    for (int i = 0; i < 11; ++i) {
        cocos2d::CCSprite* off = cocos2d::CCSprite::createWithSpriteFrameName(tabs[i].offSprite);
        cocos2d::CCSprite* on = cocos2d::CCSprite::createWithSpriteFrameName(tabs[i].onSprite);
        
        CCMenuItemToggler* toggler = CCMenuItemToggler::create(off, on, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onSelectTab));
        toggler->setSizeMult(1.2f);
        menu->addChild(toggler);
        toggler->setTag(tabs[i].tag);
        m_tabButtons->addObject(toggler);
    }
    
    menu->alignItemsHorizontallyWithPadding(2.0f);
    menu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 30.0f));
    
    // Cursors
    m_cursor1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
    m_cursor1->setScale(0.85f);
    addChild(m_cursor1, 1);
    
    m_cursor2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
    m_cursor2->setScale(0.85f);
    addChild(m_cursor2, 1);
    
    // Arrow buttons
    std::string arrowFrame = cocos2d::CCString::createWithFormat("GJ_arrow_%02d_001.png", 1)->getCString();
    
    cocos2d::CCSprite* leftArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName(arrowFrame);
    leftArrowSprite->setScale(0.8f);
    m_leftArrow = CCMenuItemSpriteExtra::create(leftArrowSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onArrow));
    m_leftArrow->setSizeMult(2.2f);
    menu->addChild(m_leftArrow);
    m_leftArrow->setPosition(cocos2d::CCPoint(panelPos.x - 220.0f, panelPos.y));
    
    cocos2d::CCSprite* rightArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName(arrowFrame);
    rightArrowSprite->setScale(0.8f);
    rightArrowSprite->setFlipX(true);
    m_rightArrow = CCMenuItemSpriteExtra::create(rightArrowSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onArrow));
    m_rightArrow->setTag(1);
    m_rightArrow->setSizeMult(2.2f);
    menu->addChild(m_rightArrow);
    m_rightArrow->setPosition(cocos2d::CCPoint(panelPos.x + 220.0f, panelPos.y));
    
    // Navigation dots
    m_pageButtons = cocos2d::CCArray::create();
    m_pageButtons->retain();
    
    m_navDotMenu = cocos2d::CCMenu::create();
    addChild(m_navDotMenu);
    m_navDotMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, 25.0f));
    
    selectTab(IconType::Cube);
}

void GJGarageLayer::setupPage(int page, IconType type) {
    m_cursor2->setVisible(false);
    m_iconType = type;
    
    if (m_iconType == IconType::Special) {
        setupSpecialPage();
        return;
    }
    
    GameManager* manager = GameManager::sharedState();
    int active = manager->activeIconForType(type);
    int count = manager->countForType(type);
    
    if (page == -1) {
        page = (active - 1) / 36;
        if (page < 0) {
            page = 0;
        }
    }
    
    // Update or create page entry
    auto it = m_iconPages.find(type);
    if (it != m_iconPages.end()) {
        it->second = page;
    } else {
        m_iconPages[type] = page;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Get items for this page
    cocos2d::CCArray* items = getItems(count, page, type, active);
    
    if (m_iconSelection) {
        m_iconSelection->removeMeAndCleanup();
        m_iconSelection = nullptr;
    }
    
    // Create ListButtonBar
    cocos2d::CCPoint centerPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 60.0f);
    m_iconSelection = ListButtonBar::create(items, cocos2d::CCPoint(5.0f, 5.0f), 12, 25.0f, 220.0f, 0.0f, 0.0f, 3);
    m_iconSelection->m_scrollLayer->togglePageIndicators(false);
    addChild(m_iconSelection, 100);
    
    // Update cursor position
    if (m_currentIcon) {
        cocos2d::CCNode* parent = m_currentIcon->getParent();
        cocos2d::CCPoint worldPos = parent->convertToWorldSpace(m_currentIcon->getPosition());
        m_cursor1->setPosition(worldPos);
        m_cursor1->setVisible(true);
    } else {
        m_cursor1->setVisible(false);
    }
    
    // Update arrows
    bool showArrows = count > 36;
    m_leftArrow->setEnabled(showArrows);
    m_leftArrow->setVisible(showArrows);
    m_rightArrow->setEnabled(showArrows);
    m_rightArrow->setVisible(showArrows);
    
    if (!showArrows) {
        m_navDotMenu->setEnabled(false);
        m_navDotMenu->setVisible(false);
        if (type == IconType::DeathEffect) {
            // Add special toggle for DeathEffect
            cocos2d::CCMenu* toggleMenu = cocos2d::CCMenu::create();
            m_iconSelection->addChild(toggleMenu, 1);
            
            GameManager* manager3 = GameManager::sharedState();
            bool enabled = !manager3->getGameVariable("0153");
            
            cocos2d::CCPoint togglePos = cocos2d::CCPoint(winSize.width * 0.5f - 165.0f, winSize.height * 0.5f - 60.0f - 30.0f);
            GameToolbox::createToggleButton(
                "Explode",
                this,
                cocos2d::SEL_MenuHandler(&GJGarageLayer::onSpecial),
                0,
                enabled,
                toggleMenu,
                togglePos,
                m_iconSelection,
                m_iconSelection,
                0.6f,
                0.5f,
                80.0f,
                cocos2d::CCPoint(8.0f, 0.0f),
                "goldFont.fnt",
                nullptr,
                false,
                false
            );
        }
        return;
    }
    
    // Setup navigation dots
    m_navDotMenu->setEnabled(true);
    m_navDotMenu->setVisible(true);
    m_navDotMenu->removeAllChildren();
    
    int totalPages = (count + 35) / 36;
    
    while (m_pageButtons->count() < totalPages) {
        cocos2d::CCSprite* dotSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png");
        dotSprite->setScale(0.9f);
        CCMenuItemSpriteExtra* dotBtn = CCMenuItemSpriteExtra::create(dotSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onNavigate));
        dotBtn->setTag(m_pageButtons->count());
        m_pageButtons->addObject(dotBtn);
        dotBtn->setSizeMult(1.1f);
    }
    
    for (int i = 0; i < totalPages; ++i) {
        CCMenuItemSpriteExtra* dotBtn = static_cast<CCMenuItemSpriteExtra*>(m_pageButtons->objectAtIndex(i));
        m_navDotMenu->addChild(dotBtn);
        
        cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
            (i == page) ? "gj_navDotBtn_on_001.png" : "gj_navDotBtn_off_001.png"
        );
        dotBtn->setDisplayFrame(frame);
    }
    
    m_navDotMenu->alignItemsHorizontallyWithPadding(6.0f);
    
    if (type == IconType::DeathEffect) {
        // Add special toggle for DeathEffect
        cocos2d::CCMenu* toggleMenu = cocos2d::CCMenu::create();
        m_iconSelection->addChild(toggleMenu, 1);
        
        GameManager* manager3 = GameManager::sharedState();
        bool enabled = !manager3->getGameVariable("0153");
        
        cocos2d::CCPoint togglePos = cocos2d::CCPoint(winSize.width * 0.5f - 165.0f, winSize.height * 0.5f - 60.0f - 30.0f);
        GameToolbox::createToggleButton(
            "Explode",
            this,
            cocos2d::SEL_MenuHandler(&GJGarageLayer::onSpecial),
            0,
            enabled,
            toggleMenu,
            togglePos,
            m_iconSelection,
            m_iconSelection,
            0.6f,
            0.5f,
            80.0f,
            cocos2d::CCPoint(8.0f, 0.0f),
            "goldFont.fnt",
            nullptr,
            false,
            false
        );
    }
}

void GJGarageLayer::setupSpecialPage() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    if (m_iconSelection) {
        m_iconSelection->removeMeAndCleanup();
        m_iconSelection = nullptr;
    }
    
    // Get streak items
    cocos2d::CCArray* items = getItems(IconType::Special);
    cocos2d::CCPoint centerPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 60.0f);
    m_iconSelection = ListButtonBar::create(items, cocos2d::CCPoint(5.0f, 5.0f), 12, 25.0f, 220.0f, 0.0f, 0.0f, 3);
    m_iconSelection->m_scrollLayer->togglePageIndicators(false);
    addChild(m_iconSelection, 100);
    
    // Update cursor
    if (m_currentIcon) {
        cocos2d::CCNode* parent = m_currentIcon->getParent();
        cocos2d::CCPoint worldPos = parent->convertToWorldSpace(m_currentIcon->getPosition());
        m_cursor1->setPosition(worldPos);
        m_cursor1->setVisible(true);
    } else {
        m_cursor1->setVisible(false);
    }
    
    // Ship fire items
    cocos2d::CCArray* fireItems = getItems(IconType::ShipFire);
    cocos2d::CCPoint firePos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 60.0f - 30.0f);
    ListButtonBar* fireBar = ListButtonBar::create(fireItems, cocos2d::CCPoint(5.0f, 5.0f), 12, 25.0f, 220.0f, 0.0f, 0.0f, 3);
    fireBar->m_scrollLayer->togglePageIndicators(false);
    m_iconSelection->addChild(fireBar, 100);
    
    // Update second cursor
    if (m_currentIcon) {
        cocos2d::CCNode* parent = m_currentIcon->getParent();
        cocos2d::CCPoint worldPos = parent->convertToWorldSpace(m_currentIcon->getPosition());
        m_cursor2->setPosition(worldPos);
        m_cursor2->setVisible(true);
    } else {
        m_cursor2->setVisible(false);
    }
    
    // Special items (icons 18-20)
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    m_iconSelection->addChild(menu, 1000);
    
    cocos2d::CCPoint startPos = cocos2d::CCPoint(winSize.width * 0.5f - 165.0f, winSize.height * 0.5f - 60.0f - 30.0f);
    
    for (int id = 18; id <= 20; ++id) {
        GameManager* manager = GameManager::sharedState();
        bool unlocked = manager->isIconUnlocked(id, IconType::Item);
        
        GJItemIcon* icon = GJItemIcon::createBrowserItem(UnlockType::Item, id);
        
        if (unlocked) {
            GameStatsManager* statsManager = GameStatsManager::sharedState();
            bool enabled = statsManager->isItemEnabled(UnlockType::Item, id);
            icon->toggleEnabledState(enabled);
        }
        
        if (icon) {
            float scale = GJItemIcon::scaleForType(UnlockType::Item);
            icon->setScale(scale);
            
            CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(icon, nullptr, this, cocos2d::SEL_MenuHandler(&GJGarageLayer::onToggleItem));
            item->setTag(id);
            item->setZOrder(static_cast<int>(IconType::Item));
            menu->addChild(item);
            
            int xOffset = (id - 18) * 30;
            item->setPosition(cocos2d::CCPoint(xOffset, 0.0f));
            
            if (!unlocked) {
                int unlockState = GameStatsManager::sharedState()->getItemUnlockState(id, UnlockType::Item);
                if (unlockState) {
                    icon->changeToLockedState(1.0f / scale);
                } else {
                    icon->darkenStoreItem(ShopType::Default);
                }
            }
        }
    }
}

void GJGarageLayer::showUnlockPopup(int id, UnlockType type) {
    // Special case for item 13 (Scratch)
    if (type == UnlockType::Item && id == 13) {
        cocos2d::CCArray* dialogs = cocos2d::CCArray::create();
        
        // Dialog 1
        DialogObject* d1 = DialogObject::create("Scratch", "Hey, over here...", 10, 0.8f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d1);
        
        // Dialog 2
        DialogObject* d2 = DialogObject::create("Scratch", "I've been watching you, and you got some skills.", 13, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d2);
        
        // Dialog 3
        DialogObject* d3 = DialogObject::create("Scratch", "Whatever you need, <co>Scratch</c>'s got it.", 10, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d3);
        
        // Dialog 4
        DialogObject* d4 = DialogObject::create("Scratch", "Here's a free sample. Come find me when you can.", 8, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d4);
        
        // Dialog 5
        DialogObject* d5 = DialogObject::create("Scratch", "Did you hear something?", 9, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d5);
        
        // Dialog 6
        DialogObject* d6 = DialogObject::create("Scratch", "<d010>.<d010>.<d010>.", 8, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d6);
        
        DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
        addChild(dialogLayer, 100);
        dialogLayer->m_delegate = this;
        dialogLayer->updateChatPlacement(PlacementType::Center);
        dialogLayer->animateIn(AnimationType::FromLeft);
    } else {
        showUnlockPopupNew(id, type);
    }
}

void GJGarageLayer::showUnlockPopupNew(int id, UnlockType type) {
    ItemInfoPopup* popup = ItemInfoPopup::create(id, type);
    popup->show();
}

void GJGarageLayer::textChanged(CCTextInputNode* node) {
    std::string name = node->getString();
    updatePlayerName(name.c_str());
}

void GJGarageLayer::textInputClosed(CCTextInputNode* node) {
    std::string name = node->getString();
    
    if (name == "Player" || name.empty()) {
        name = "Player";
    }
    
    GameManager* manager = GameManager::sharedState();
    manager->m_scoreValid = false;
    manager->m_playerName = name;
    updatePlayerName(name.c_str());
    
    node->setString(name);
}

void GJGarageLayer::textInputOpened(CCTextInputNode* node) {
    GameManager::sharedState()->m_clickedName = true;
    
    std::string name = node->getString();
    if (name == "Player") {
        updatePlayerName("");
        node->setString("");
        node->updateLabel(" ");
    }
}

std::string GJGarageLayer::titleForUnlock(int id, UnlockType type) {
    std::string result;
    AchievementManager* manager = AchievementManager::sharedState();
    std::string achievementKey = manager->achievementForUnlock(id, type);
    cocos2d::CCDictionary* dict = manager->getAchievementsWithID(achievementKey);
    
    cocos2d::CCString* title = static_cast<cocos2d::CCString*>(dict->objectForKey("title"));
    if (title) {
        result = title->getCString();
    }
    
    return result;
}

void GJGarageLayer::toggleGlow() {
    GameManager* manager = GameManager::sharedState();
    manager->m_playerGlow = !manager->m_playerGlow;
    
    m_playerObject->m_hasGlowOutline = manager->m_playerGlow;
    m_playerObject->updateColors();
    
    if (manager->m_playerGlow) {
        playRainbowEffect();
    }
}

void GJGarageLayer::updatePlayerColors() {
    m_playerObject->m_hasGlowOutline = GameManager::sharedState()->m_playerGlow;
    
    GameManager* manager = GameManager::sharedState();
    
    // Primary color
    int colorIdx = manager->m_playerColor.random - manager->m_playerColor.seed;
    cocos2d::ccColor3B color1 = manager->colorForIdx(colorIdx);
    
    // Secondary color
    int colorIdx2 = manager->m_playerColor2.random - manager->m_playerColor2.seed;
    cocos2d::ccColor3B color2 = manager->colorForIdx(colorIdx2);
    
    m_playerObject->setColors(color1, color2);
    
    // Glow color
    int glowColorIdx = manager->m_playerGlowColor.random - manager->m_playerGlowColor.seed;
    if (glowColorIdx < 0) {
        m_playerObject->disableCustomGlowColor();
    } else {
        cocos2d::ccColor3B glowColor = manager->colorForIdx(glowColorIdx);
        m_playerObject->enableCustomGlowColor(glowColor);
    }
    
    m_playerObject->updateColors();
}

void GJGarageLayer::updatePlayerName(const char* name) {
    // Empty implementation
}

void GJGarageLayer::updateRate() {
    GameManager* manager = GameManager::sharedState();
    
    if (manager->m_ratedGame && m_supporter) {
        std::string frame = cocos2d::CCString::createWithFormat("player_%02d_001.png", 4)->getCString();
        std::string frame2 = cocos2d::CCString::createWithFormat("player_%02d_2_001.png", 4)->getCString();
        
        m_supporter->removeAllChildrenWithCleanup(true);
        m_supporter->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame));
        
        cocos2d::CCSprite* child = cocos2d::CCSprite::createWithSpriteFrameName(frame2);
        m_supporter->addChild(child, -1);
        child->setPosition(cocos2d::CCPoint(m_supporter->getContentSize().width * 0.5f, m_supporter->getContentSize().height * 0.5f));
        m_supporter->setColor(cocos2d::ccc3(175, 206, 215));
        child->setColor(cocos2d::ccc3(255, 255, 255));
        
        manager->m_gameRateDelegate1 = nullptr;
    }
    
    if (manager->m_clickedFacebook && m_unknown) {
        m_unknown->removeAllChildrenWithCleanup(true);
        manager->m_gameRateDelegate1 = nullptr;
    }
}

GJGarageLayer::~GJGarageLayer() {
    if (m_tabButtons) {
        m_tabButtons->release();
    }
    if (m_pageButtons) {
        m_pageButtons->release();
    }
}
