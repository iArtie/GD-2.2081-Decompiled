#include "Layers/MenuLayer.h"
#include "Layers/MenuGameLayer.h"
#include "Managers/GameManager.h"
#include "GameLevelManager.h"
#include "Utils/GameToolbox.h"
#include "Managers/GJAccountManager.h"
#include "Layers/GJGarageLayer.h"
#include "Layers/CreatorLayer.h"
#include "Layers/LevelSelectLayer.h"
#include "Layers/AchievementsLayer.h"
#include "Layers/StatsLayer.h"
#include "Layers/OptionsLayer.h"
#include "Layers/VideoOptionsLayer.h"
#include "Pages/RewardsPage.h"
#include "Pages/ProfilePage.h"
#include "Layers/GJMoreGamesLayer.h"
#include "Utils/PromoInterstitial.h"
#include "Popups/TOSPopup.h"
#include "Utils/PlatformToolbox.h"
#include "Managers/GooglePlayManager.h"
#include "Delegates/AppDelegate.h"
#include "Layers/FLAlertLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

void MenuLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (!btn2)
        return;

    switch (layer->getTag()) {
        case 0: {
            AppDelegate* appDelegate = AppDelegate::get();
            appDelegate->trySaveGame(true);
            appDelegate = AppDelegate::get();
            appDelegate->showLoadingCircle(false, true, true);

            cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0.5f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(MenuLayer::endGame)),
                nullptr);
            cocos2d::CCDirector::sharedDirector()->getActionManager()->addAction(seq, this, false);
            break;
        }
        case 1: {
            GameManager* gm = GameManager::sharedState();
            gm->setGameVariable("0034", true);
            PlatformToolbox::activateGameCenter();
            if (PlatformToolbox::isLocalPlayerAuthenticated())
                onGameCenter(nullptr);
            break;
        }
        case 2: {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("https://%s", "");
            std::string url = str->getCString();
            cocos2d::CCApplication::sharedApplication()->openURL(url.c_str());
            break;
        }
        case 3: {
            cocos2d::CCApplication::sharedApplication()->openURL("https://www.newgrounds.com/");
            break;
        }
        default:
            return;
    }
}

void MenuLayer::endGame()
{
    cocos2d::CCDirector::sharedDirector()->end();
}

void MenuLayer::firstNetworkTest()
{
    GameLevelManager::sharedState()->performNetworkTest();
}

void MenuLayer::googlePlaySignedIn()
{
    if (m_gpSprite) {
        if (PlatformToolbox::isSignedInGooglePlay()) {
            cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_gpgBtn_001.png");
            m_gpSprite->setDisplayFrame(frame);
            cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
                schedule_selector(MenuLayer::syncPlatformAchievements),
                this, 0.0f, false);
        } else {
            cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_gpBtn_001.png");
            m_gpSprite->setDisplayFrame(frame);
        }
    }
}

bool MenuLayer::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    GameManager* gm = GameManager::sharedState();
    gm->fadeInMenuMusic();
    gm->m_inMenuLayer = true;
    gm->m_sceneEnum = 0;
    gm->m_somethingInMenuLayer = 0;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_menuGameLayer = MenuGameLayer::create();
    this->addChild(m_menuGameLayer);

    cocos2d::CCSprite* logo = cocos2d::CCSprite::createWithSpriteFrameName("GJ_logo_001.png");
    this->addChild(logo, 2);
    logo->setPosition(ccp(winSize.width * 0.5f, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 50.0f));

    cocos2d::CCMenu* mainMenu = cocos2d::CCMenu::create();

    cocos2d::CCSprite* playSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn_001.png");
    CCMenuItemSpriteExtra* playBtn = CCMenuItemSpriteExtra::create(
        playSprite, nullptr, this, menu_selector(MenuLayer::onPlay));
    mainMenu->addChild(playBtn);

    this->addChild(mainMenu);
    mainMenu->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f));

    cocos2d::CCSprite* garageSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_garageBtn_001.png");
    CCMenuItemSpriteExtra* garageBtn = CCMenuItemSpriteExtra::create(
        garageSprite, nullptr, this, menu_selector(MenuLayer::onGarage));
    mainMenu->addChild(garageBtn);
    garageBtn->setPosition(playBtn->getPosition() + ccp(-110.0f, 0.0f));

    if (!GameManager::sharedState()->m_clickedGarage) {
        cocos2d::CCSprite* chrSel = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chrSel_001.png");
        this->addChild(chrSel);
        cocos2d::CCPoint worldPos = mainMenu->convertToWorldSpace(garageBtn->getPosition());
        chrSel->setPosition(worldPos + ccp(-50.0f, -50.0f));
    }

    cocos2d::CCSprite* creatorSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_creatorBtn_001.png");
    CCMenuItemSpriteExtra* creatorBtn = CCMenuItemSpriteExtra::create(
        creatorSprite, nullptr, this, menu_selector(MenuLayer::onCreator));
    mainMenu->addChild(creatorBtn);
    creatorBtn->setPosition(playBtn->getPosition() + ccp(110.0f, 0.0f));

    if (!GameManager::sharedState()->m_clickedEditor) {
        cocos2d::CCSprite* lvlEdit = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lvlEdit_001.png");
        this->addChild(lvlEdit);
        cocos2d::CCPoint worldPos = mainMenu->convertToWorldSpace(creatorBtn->getPosition());
        lvlEdit->setPosition(worldPos + ccp(50.0f, -50.0f));
    }

    float bottomY = cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 45.0f;
    cocos2d::CCMenu* bottomMenu = cocos2d::CCMenu::create();
    this->addChild(bottomMenu);

    cocos2d::CCSprite* achSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_achBtn_001.png");
    achSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* achBtn = CCMenuItemSpriteExtra::create(
        achSprite, nullptr, this, menu_selector(MenuLayer::onAchievements));
    bottomMenu->addChild(achBtn);

    cocos2d::CCSprite* optSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* optBtn = CCMenuItemSpriteExtra::create(
        optSprite, nullptr, this, menu_selector(MenuLayer::onOptions));
    bottomMenu->addChild(optBtn);

    cocos2d::CCSprite* statsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_statsBtn_001.png");
    statsSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* statsBtn = CCMenuItemSpriteExtra::create(
        statsSprite, nullptr, this, menu_selector(MenuLayer::onStats));
    bottomMenu->addChild(statsBtn);

    cocos2d::CCSprite* ngSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_ngBtn_001.png");
    ngSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* ngBtn = CCMenuItemSpriteExtra::create(
        ngSprite, nullptr, this, menu_selector(MenuLayer::onNewgrounds));
    bottomMenu->addChild(ngBtn);

    bottomMenu->setPosition(ccp(winSize.width * 0.5f, bottomY));
    bottomMenu->alignItemsHorizontallyWithPadding(5.0f);

    float socialY = cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 30.0f;

    cocos2d::CCSprite* robTopSprite = cocos2d::CCSprite::createWithSpriteFrameName("robtoplogo_small.png");
    robTopSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* robTopBtn = CCMenuItemSpriteExtra::create(
        robTopSprite, nullptr, this, menu_selector(MenuLayer::onRobTop));
    robTopBtn->setSizeMult(1.1f);
    cocos2d::CCMenu* socialMenu = cocos2d::CCMenu::create(robTopBtn, nullptr);
    this->addChild(socialMenu, 2);
    socialMenu->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 50.0f,
        socialY - 5.0f));

    cocos2d::CCSprite* fbSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_fbIcon_001.png");
    fbSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* fbBtn = CCMenuItemSpriteExtra::create(
        fbSprite, nullptr, this, menu_selector(MenuLayer::onFacebook));
    fbBtn->setSizeMult(1.5f);
    socialMenu->addChild(fbBtn);

    cocos2d::CCSprite* twSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_twIcon_001.png");
    twSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* twBtn = CCMenuItemSpriteExtra::create(
        twSprite, nullptr, this, menu_selector(MenuLayer::onTwitter));
    twBtn->setSizeMult(1.5f);
    socialMenu->addChild(twBtn);

    cocos2d::CCSprite* ytSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png");
    ytSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* ytBtn = CCMenuItemSpriteExtra::create(
        ytSprite, nullptr, this, menu_selector(MenuLayer::onYouTube));
    ytBtn->setSizeMult(1.5f);
    socialMenu->addChild(ytBtn);

    cocos2d::CCSprite* twitchSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_twitchIcon_001.png");
    twitchSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* twitchBtn = CCMenuItemSpriteExtra::create(
        twitchSprite, nullptr, this, menu_selector(MenuLayer::onTwitch));
    twitchBtn->setSizeMult(1.5f);
    socialMenu->addChild(twitchBtn);

    cocos2d::CCSprite* discordSprite = cocos2d::CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png");
    discordSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* discordBtn = CCMenuItemSpriteExtra::create(
        discordSprite, nullptr, this, menu_selector(MenuLayer::onDiscord));
    discordBtn->setSizeMult(1.5f);
    socialMenu->addChild(discordBtn);

    fbBtn->setPosition(socialMenu->convertToNodeSpace(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 22.0f,
        socialY + 25.0f)));

    twBtn->setPosition(fbBtn->getPosition() + ccp(29.0f, 0.0f));
    ytBtn->setPosition(twBtn->getPosition() + ccp(29.0f, 0.0f));
    twitchBtn->setPosition(ytBtn->getPosition() + ccp(29.0f, 0.0f));
    discordBtn->setPosition(ytBtn->getPosition() + ccp(29.0f, -29.0f));

    cocos2d::CCSprite* moreSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_moreGamesBtn_001.png");
    moreSprite->setScale(0.9f);
    CCMenuItemSpriteExtra* moreBtn = CCMenuItemSpriteExtra::create(
        moreSprite, nullptr, this, menu_selector(MenuLayer::onMoreGames));
    cocos2d::CCMenu* moreMenu = cocos2d::CCMenu::create(moreBtn, nullptr);
    this->addChild(moreMenu, 2);
    moreMenu->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 43.0f,
        bottomY));

    GameManager::sharedState()->m_firstSetup = false;

    if (PlatformToolbox::isControllerConnected()) {
        cocos2d::CCSprite* startSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        this->addChild(startSprite);
        cocos2d::CCPoint worldPos = mainMenu->convertToWorldSpace(playBtn->getPosition());
        cocos2d::CCPoint nodePos = this->convertToNodeSpace(worldPos);
        startSprite->setPosition(nodePos + ccp(0.0f, -66.0f));

        cocos2d::CCSprite* ySprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Y_001.png");
        this->addChild(ySprite);
        cocos2d::CCPoint yWorldPos = creatorBtn->getParent()->convertToWorldSpace(creatorBtn->getPosition());
        cocos2d::CCPoint yNodePos = this->convertToNodeSpace(yWorldPos);
        ySprite->setPosition(yNodePos + ccp(0.0f, -45.0f));
        ySprite->setScale(0.8f);

        cocos2d::CCSprite* xSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_X_001.png");
        this->addChild(xSprite);
        cocos2d::CCPoint xWorldPos = garageBtn->getParent()->convertToWorldSpace(garageBtn->getPosition());
        cocos2d::CCPoint xNodePos = this->convertToNodeSpace(xWorldPos);
        xSprite->setPosition(xNodePos + ccp(0.0f, -45.0f));
        xSprite->setScale(0.8f);

        cocos2d::CCSprite* dpadSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_DPad_Down_001.png");
        this->addChild(dpadSprite);
        cocos2d::CCPoint dpadWorldPos = optBtn->getParent()->convertToWorldSpace(optBtn->getPosition());
        cocos2d::CCPoint dpadNodePos = this->convertToNodeSpace(dpadWorldPos);
        dpadSprite->setPosition(dpadNodePos + ccp(0.0f, -34.0f));
        dpadSprite->setScale(0.5f);

        GameManager* gm2 = GameManager::sharedState();
        if (!gm2->getGameVariable("0028")) {
            GJAccountManager* accountMgr = GJAccountManager::sharedState();
            float yOffset = 0.0f;
            if (accountMgr->m_accountID - accountMgr->m_unkInt1 > 0)
                yOffset = 15.0f;

            cocos2d::CCSprite* lThumbSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_LThumb_001.png");
            this->addChild(lThumbSprite);
            lThumbSprite->setPosition(ccp(
                cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
                yOffset + (winSize.height * 0.5f + 15.0f) + 10.0f));

            cocos2d::CCSprite* aSprite = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_A_001.png");
            this->addChild(aSprite);
            aSprite->setPosition(lThumbSprite->getPosition() + ccp(0.0f, -30.0f));

            cocos2d::CCLabelBMFont* lThumbLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
            lThumbLabel->setScale(0.5f);
            this->addChild(lThumbLabel);
            lThumbLabel->setAnchorPoint(ccp(0.0f, 0.6f));
            lThumbLabel->setPosition(lThumbSprite->getPosition() + ccp(16.0f, 1.0f));

            cocos2d::CCLabelBMFont* aLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
            aLabel->setScale(0.5f);
            this->addChild(aLabel);
            aLabel->setAnchorPoint(ccp(0.0f, 0.6f));
            aLabel->setPosition(aSprite->getPosition() + ccp(16.0f, 1.0f));
        }
    }

    cocos2d::CCSprite* profileSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_profileButton_001.png");
    CCMenuItemSpriteExtra* profileBtn = CCMenuItemSpriteExtra::create(
        profileSprite, nullptr, this, menu_selector(MenuLayer::onMyProfile));
    m_profileButton = profileBtn;
    mainMenu->addChild(profileBtn, 2);
    profileBtn->setPosition(mainMenu->convertToNodeSpace(ccp(45.0f, 105.0f)));

    m_profileLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    this->addChild(m_profileLabel, 2);
    m_profileLabel->setPosition(ccp(45.0f, 105.0f) + ccp(2.0f, 36.0f));
    m_profileLabel->limitLabelWidth(70.0f, 0.7f, 0.0f);

    gm = GameManager::sharedState();
    if (!gm->getGameVariable("0048") && GameManager::sharedState()->m_clickedGarage) {
        m_viewProfileSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_viewProfileTxt_001.png");
        this->addChild(m_viewProfileSprite, 2);
        m_viewProfileSprite->setPosition(ccp(45.0f, 105.0f) + ccp(76.0f, -1.0f));
    }

    updateUserProfileButton();

    GameManager::sharedState()->m_menuLayer = this;

    cocos2d::CCSprite* dailySprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_dailyRewardBtn_001.png");
    CCMenuItemSpriteExtra* dailyBtn = CCMenuItemSpriteExtra::create(
        dailySprite, nullptr, this, menu_selector(MenuLayer::onDaily));
    bottomMenu->addChild(dailyBtn);
    dailyBtn->setPosition(bottomMenu->convertToNodeSpace(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 40.0f,
        winSize.height * 0.5f + 20.0f)));
    dailyBtn->setSizeMult(1.5f);

    gm = GameManager::sharedState();
    if (!gm->getUGV("30")) {
        m_showingTOS = true;
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.01f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(MenuLayer::showTOS)),
            nullptr);
        this->runAction(seq);
    }

    if (!GameLevelManager::sharedState()->m_testedNetwork) {
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.01f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(MenuLayer::firstNetworkTest)),
            nullptr);
        this->runAction(seq);
    }

    return true;
}

void MenuLayer::keyBackClicked()
{
    onQuit(nullptr);
}

void MenuLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    switch (key) {
        case KEY_Enter:
        case KEY_Space:
        case CONTROLLER_Start:
            onPlay(nullptr);
            return;
        case CONTROLLER_Y:
            onCreator(nullptr);
            return;
        case CONTROLLER_Down:
            onOptions(nullptr);
            return;
        case CONTROLLER_X:
            onGarage(nullptr);
            return;
        default:
            cocos2d::CCLayer::keyDown(key, timestamp);
            return;
    }
}

void MenuLayer::onAchievements(cocos2d::CCObject* sender)
{
    AchievementsLayer* layer = AchievementsLayer::create();
    this->addChild(layer, 100);
    layer->showLayer(false);
}

void MenuLayer::onCreator(cocos2d::CCObject* sender)
{
    if (m_showingTOS)
        return;
    willClose();
    GameManager::sharedState()->m_clickedEditor = true;
    cocos2d::CCScene* scene = CreatorLayer::scene();
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void MenuLayer::onDaily(cocos2d::CCObject* sender)
{
    RewardsPage* page = RewardsPage::create();
    page->show();
}

void MenuLayer::onDiscord(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://discord.gg/geometrydash");
}

void MenuLayer::onEveryplay(cocos2d::CCObject* sender)
{
}

void MenuLayer::onFacebook(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.facebook.com/geometrydash");
}

void MenuLayer::onFreeLevels(cocos2d::CCObject* sender)
{
    GameManager* gm = GameManager::sharedState();
    gm->setGameVariable("0053", true);
    PromoInterstitial* promo = PromoInterstitial::create(false);
    promo->show();
    gm = GameManager::sharedState();
    gm->reportAchievementWithID("geometry.ach.moreGames", 100, false);
}

void MenuLayer::onFullVersion(cocos2d::CCObject* sender)
{
    PromoInterstitial* promo = PromoInterstitial::create(true);
    promo->show();
}

void MenuLayer::onGameCenter(cocos2d::CCObject* sender)
{
    GameManager* gm = GameManager::sharedState();
    if (gm->getGameVariable("0034")) {
        gm = GameManager::sharedState();
        gm->syncPlatformAchievements();
        PlatformToolbox::showAchievements();
    } else {
        showGCQuestion();
    }
}

void MenuLayer::onGarage(cocos2d::CCObject* sender)
{
    if (m_showingTOS)
        return;
    willClose();
    GameManager::sharedState()->m_clickedGarage = true;
    cocos2d::CCScene* scene = GJGarageLayer::scene();
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void MenuLayer::onGooglePlayGames(cocos2d::CCObject* sender)
{
    if (PlatformToolbox::isSignedInGooglePlay()) {
        GameManager::sharedState()->syncPlatformAchievements();
        PlatformToolbox::showAchievements();
    } else {
        PlatformToolbox::signInGooglePlay();
    }
}

void MenuLayer::onMoreGames(cocos2d::CCObject* sender)
{
    GameManager* gm = GameManager::sharedState();
    gm->setGameVariable("0053", true);
    PlatformToolbox::logEvent("MoreGames");
    gm = GameManager::sharedState();
    gm->reportAchievementWithID("geometry.ach.moreGames", 100, false);
    GJMoreGamesLayer* layer = GJMoreGamesLayer::create();
    this->addChild(layer, 100);
    layer->showLayer(false);
}

void MenuLayer::onMyProfile(cocos2d::CCObject* sender)
{
    GJAccountManager* accountMgr = GJAccountManager::sharedState();
    if (accountMgr->m_accountID - accountMgr->m_unkInt1 > 0) {
        GameManager::sharedState()->setGameVariable("0048", true);
        accountMgr = GJAccountManager::sharedState();
        ProfilePage* page = ProfilePage::create(accountMgr->m_accountID - accountMgr->m_unkInt1, true);
        page->show();
    }
}

void MenuLayer::onNewgrounds(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this, "Newgrounds", "Visit Newgrounds to find awesome music?");
    alert->setTag(3);
    alert->show();
}

void MenuLayer::onOptions(cocos2d::CCObject* sender)
{
    openOptions(false);
}

void MenuLayer::onOptionsInstant()
{
    openOptions(true);
}

void MenuLayer::onPlay(cocos2d::CCObject* sender)
{
    if (m_showingTOS)
        return;
    willClose();
    GameManager::sharedState()->m_sceneEnum = 8;
    cocos2d::CCScene* scene = LevelSelectLayer::scene(0);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void MenuLayer::onQuit(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this, "Quit Game", "Are you sure you want to <cr>quit</c>?");
    alert->setTag(0);
    alert->show();
}

void MenuLayer::onRobTop(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.robtopgames.com/");
}

void MenuLayer::onStats(cocos2d::CCObject* sender)
{
    StatsLayer* layer = StatsLayer::create();
    this->addChild(layer, 100);
    layer->showLayer(false);
}

void MenuLayer::onTrailer(cocos2d::CCObject* sender)
{
    PlatformToolbox::logEvent("gjl_trailer");
    if (GameToolbox::doWeHaveInternet()) {
        cocos2d::CCApplication::sharedApplication()->openURL("https://www.youtube.com/watch?v=...");
    }
}

void MenuLayer::onTwitch(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.twitch.tv/robtopgames");
}

void MenuLayer::onTwitter(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://twitter.com/robtopgames");
}

void MenuLayer::onYouTube(cocos2d::CCObject* sender)
{
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.youtube.com/user/robtopgames");
}

void MenuLayer::openOptions(bool videoOptions)
{
    OptionsLayer* optionsLayer = OptionsLayer::create();
    this->addChild(optionsLayer, 100);
    optionsLayer->showLayer(videoOptions);
    if (videoOptions) {
        VideoOptionsLayer* videoLayer = VideoOptionsLayer::create();
        optionsLayer->addChild(videoLayer, 100);
    }
}

cocos2d::CCScene* MenuLayer::scene(bool isVideoOptionsOpen)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    MenuLayer* layer = new MenuLayer();
    if (!layer->init()) {
        delete layer;
        layer = nullptr;
    } else {
        layer->autorelease();
    }
    scene->addChild(layer);
    if (isVideoOptionsOpen && layer) {
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.0f),
            cocos2d::CCCallFunc::create(layer, callfunc_selector(MenuLayer::onOptionsInstant)),
            nullptr);
        layer->runAction(seq);
    }
    return scene;
}

void MenuLayer::showGCQuestion()
{
    FLAlertLayer* alert = FLAlertLayer::create(
        this, "Game Center",
        "Do you want to <cg>enable</c>\n<cy>Game Center</c>? You can change this anytime in the options menu.");
    alert->setTag(1);
    alert->m_scene = AppDelegate::get();
    alert->show();
}

void MenuLayer::showMeltdownPromo()
{
}

void MenuLayer::showTOS()
{
    m_showingTOS = false;
    TOSPopup* popup = TOSPopup::create();
    popup->m_scene = this->getParent();
    popup->show();
}

void MenuLayer::syncPlatformAchievements(float dt)
{
    this->unschedule(schedule_selector(MenuLayer::syncPlatformAchievements));
    GameManager::sharedState()->syncPlatformAchievements();
}

void MenuLayer::tryShowAd(float dt)
{
    GameManager* gm = GameManager::sharedState();
    if (gm->shouldShowInterstitial(180, 140, false)) {
        gm = GameManager::sharedState();
        gm->showInterstitial();
        this->unschedule(schedule_selector(MenuLayer::tryShowAd));
    }
}

void MenuLayer::updateUserProfileButton()
{
    GJAccountManager* accountMgr = GJAccountManager::sharedState();
    int accountID = accountMgr->m_accountID;
    int unkInt1 = accountMgr->m_unkInt1;
    bool loggedIn = (accountID - unkInt1) > 0;

    m_profileButton->setVisible(loggedIn);
    m_profileLabel->setVisible(loggedIn);
    if (m_viewProfileSprite)
        m_viewProfileSprite->setVisible(loggedIn);

    if (loggedIn) {
        accountMgr = GJAccountManager::sharedState();
        std::string username = accountMgr->m_username;
        m_profileLabel->setString(username.c_str());
        m_profileLabel->limitLabelWidth(70.0f, 0.7f, 0.0f);
    }
}

void MenuLayer::videoOptionsClosed()
{
    m_menuGameLayer->m_videoOptionsOpen = false;
}

void MenuLayer::videoOptionsOpened()
{
    m_menuGameLayer->m_videoOptionsOpen = true;
}

void MenuLayer::willClose()
{
    if (GooglePlayManager::sharedState()->m_delegate1 == this)
        GooglePlayManager::sharedState()->m_delegate1 = nullptr;
    GameManager::sharedState()->m_inMenuLayer = false;
}

MenuLayer::~MenuLayer()
{
}