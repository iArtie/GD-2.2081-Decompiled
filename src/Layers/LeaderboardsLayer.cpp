#include "Layers/LeaderboardsLayer.h"
#include "Managers/GameManager.h"
#include "GameLevelManager.h"
#include "UI/LoadingCircle.h"
#include "UI/TextArea.h"
#include "Layers/GJListLayer.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Sprites/ButtonSprite.h"
#include "Layers/FLAlertLayer.h"
#include "UI/CustomListView.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "Platform/cocos2d.h"

LeaderboardsLayer* LeaderboardsLayer::create(LeaderboardType type, LeaderboardStat stat) {
    auto layer = new LeaderboardsLayer();
    if (layer && layer->init(type, stat)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool LeaderboardsLayer::init(LeaderboardType type, LeaderboardStat stat) {
    if (!CCLayer::init()) {
        return false;
    }
    
    if (type == LeaderboardType::Default) {
        type = LeaderboardType::Top100;
    }
    
    GameManager::sharedState()->m_searchObjectBool = false;
    GameManager::sharedState()->m_sceneEnum = 6;
    
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    
    if (!GameManager::sharedState()->m_scoreValid) {
        auto manager = GameLevelManager::sharedState();
        
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Top100, LeaderboardStat::Stars));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Top100, LeaderboardStat::Moons));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Top100, LeaderboardStat::Demons));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Top100, LeaderboardStat::UserCoins));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Friends, LeaderboardStat::Stars));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Friends, LeaderboardStat::Moons));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Friends, LeaderboardStat::Demons));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Friends, LeaderboardStat::UserCoins));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Global, LeaderboardStat::Stars));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Global, LeaderboardStat::Moons));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Global, LeaderboardStat::Demons));
        manager->resetTimerForKey(manager->getLeaderboardKey(LeaderboardType::Global, LeaderboardStat::UserCoins));
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bg, -2);
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().height);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().width);
    bg->setPosition(cocos2d::CCPoint(-5.0f, -5.0f));
    bg->setColor(cocos2d::ccc3(102, 102, 255));
    
    auto sideArtLeft = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtLeft->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    sideArtLeft->setPosition(cocos2d::CCPoint(director->getScreenLeft() - 1.0f, director->getScreenBottom() - 1.0f));
    addChild(sideArtLeft, 1);
    
    auto sideArtRight = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    sideArtRight->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    sideArtRight->setPosition(cocos2d::CCPoint(director->getScreenRight() + 1.0f, director->getScreenBottom() - 1.0f));
    sideArtRight->setFlipX(true);
    addChild(sideArtRight, 1);
    
    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = this;
    m_circle->m_fade = false;
    m_circle->retain();
    m_circle->show();
    m_circle->setVisible(false);
    
    m_noInternet = TextArea::create("No <cg>Internet</c> connection!", "bigFont.fnt", 1.0f, 600.0f, cocos2d::CCPoint(20.0f, 0.0f), 0.5f, cocos2d::CCPoint(0.5f, 1.0f));
    m_noInternet->m_height = 30.0f;
    addChild(m_noInternet, 2);
    m_noInternet->setPosition(cocos2d::CCPoint(centerX, centerY + 10.0f));
    m_noInternet->setScale(0.6f);
    m_noInternet->setVisible(false);
    
    auto infoMenu = cocos2d::CCMenu::create();
    addChild(infoMenu, 10);
    
    auto infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    auto infoItem = CCMenuItemSpriteExtra::create(infoSprite, this, menu_selector(LeaderboardsLayer::onInfo));
    infoItem->setSizeMult(2.0f);
    infoMenu->addChild(infoItem);
    infoMenu->setPosition(cocos2d::CCPoint(director->getScreenLeft() + 30.0f, director->getScreenBottom() + 30.0f));
    
    setupTabs();
    selectLeaderboard(type, stat);
    
    auto backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto backItem = CCMenuItemSpriteExtra::create(backSprite, this, menu_selector(LeaderboardsLayer::onBack));
    backItem->setSizeMult(1.6f);
    auto backMenu = cocos2d::CCMenu::create(backItem, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(director->getScreenLeft() + 24.0f, director->getScreenTop() - 23.0f));
    
    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backItem);
    }
    
    return true;
}

void LeaderboardsLayer::setupTabs() {
    m_tabs = cocos2d::CCArray::create();
    m_tabs->retain();
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto topMenu = cocos2d::CCMenu::create();
    addChild(topMenu, 2);
    m_tabs->addObject(topMenu);
    
    auto topOn = cocos2d::CCSprite::createWithSpriteFrameName("top_tab_on_001.png");
    auto topOff = cocos2d::CCSprite::createWithSpriteFrameName("top_tab_off_001.png");
    m_topBtn = CCMenuItemToggler::create(topOff, topOn, this, menu_selector(LeaderboardsLayer::onTop));
    m_topBtn->toggle(true);
    m_topBtn->m_offButton->m_colorEnabled = true;
    m_topBtn->m_offButton->m_animationEnabled = false;
    m_topBtn->m_onButton->m_animationEnabled = false;
    m_topBtn->setSizeMult(1.2f);
    topMenu->addChild(m_topBtn);
    
    auto friendsMenu = cocos2d::CCMenu::create();
    addChild(friendsMenu, 2);
    m_tabs->addObject(friendsMenu);
    
    auto friendsOn = cocos2d::CCSprite::createWithSpriteFrameName("friends_tab_on_001.png");
    auto friendsOff = cocos2d::CCSprite::createWithSpriteFrameName("friends_tab_off_001.png");
    m_friendsBtn = CCMenuItemToggler::create(friendsOff, friendsOn, this, menu_selector(LeaderboardsLayer::onWeek));
    m_friendsBtn->toggle(true);
    m_friendsBtn->m_offButton->m_colorEnabled = true;
    m_friendsBtn->m_offButton->m_animationEnabled = false;
    m_friendsBtn->m_onButton->m_animationEnabled = false;
    m_friendsBtn->setSizeMult(1.2f);
    friendsMenu->addChild(m_friendsBtn);
    
    auto globalMenu = cocos2d::CCMenu::create();
    addChild(globalMenu, -1);
    m_tabs->addObject(globalMenu);
    
    auto globalOn = cocos2d::CCSprite::createWithSpriteFrameName("global_tab_on_001.png");
    auto globalOff = cocos2d::CCSprite::createWithSpriteFrameName("global_tab_off_001.png");
    m_globalBtn = CCMenuItemToggler::create(globalOff, globalOn, this, menu_selector(LeaderboardsLayer::onGlobal));
    m_globalBtn->toggle(true);
    m_globalBtn->m_offButton->m_colorEnabled = true;
    m_globalBtn->m_offButton->m_animationEnabled = false;
    m_globalBtn->m_onButton->m_animationEnabled = false;
    m_globalBtn->setSizeMult(1.2f);
    globalMenu->addChild(m_globalBtn);
    
    auto creatorsMenu = cocos2d::CCMenu::create();
    addChild(creatorsMenu, -1);
    m_tabs->addObject(creatorsMenu);
    
    auto creatorsOn = cocos2d::CCSprite::createWithSpriteFrameName("creators_tab_on_001.png");
    auto creatorsOff = cocos2d::CCSprite::createWithSpriteFrameName("creators_tab_off_001.png");
    m_creatorsBtn = CCMenuItemToggler::create(creatorsOff, creatorsOn, this, menu_selector(LeaderboardsLayer::onCreators));
    m_creatorsBtn->toggle(true);
    m_creatorsBtn->m_offButton->m_colorEnabled = true;
    m_creatorsBtn->m_offButton->m_animationEnabled = false;
    m_creatorsBtn->m_onButton->m_animationEnabled = false;
    m_creatorsBtn->setSizeMult(1.2f);
    creatorsMenu->addChild(m_creatorsBtn);
    
    float tabY = centerY + 110.0f + 13.0f + 10.0f;
    topMenu->setPosition(cocos2d::CCPoint(centerX - 142.5f, tabY));
    friendsMenu->setPosition(cocos2d::CCPoint(centerX - 47.5f, tabY));
    globalMenu->setPosition(cocos2d::CCPoint(centerX + 47.5f, tabY));
    creatorsMenu->setPosition(cocos2d::CCPoint(centerX + 142.5f, tabY));
    
    auto modeMenu = cocos2d::CCMenu::create();
    addChild(modeMenu, 10);
    m_modeButtons = cocos2d::CCArray::create();
    m_modeButtons->retain();
    
    cocos2d::CCPoint modePos(centerX + 220.0f, centerY + 52.5f);
    modePos = modeMenu->convertToNodeSpace(modePos);
    
    const char* statFrames[] = {"GJ_starsIcon_001.png", "GJ_moonsIcon_001.png", "GJ_demonIcon_001.png", "GJ_coinsIcon2_001.png"};
    
    for (int i = 0; i < 4; ++i) {
        auto icon = cocos2d::CCSprite::createWithSpriteFrameName(statFrames[i]);
        auto btn = ButtonSprite::create(icon, 0, 1, 32.0f, 1.0f, true, "GJ_button_01.png", nullptr, false);
        btn->m_subSprite->setScale(1.2f);
        btn->updateSpriteOffset(cocos2d::CCPoint(0.0f, -1.5f));
        
        auto item = CCMenuItemSpriteExtra::create(btn, this, menu_selector(LeaderboardsLayer::onStat));
        item->setTag(i);
        item->setPosition(cocos2d::CCPoint(modePos.x, modePos.y - i * 35.0f));
        item->setScale(0.6f);
        item->m_baseScale = 0.6f;
        modeMenu->addChild(item);
        m_modeButtons->addObject(item);
    }
}

void LeaderboardsLayer::selectLeaderboard(LeaderboardType type, LeaderboardStat stat) {
    if (m_type == type && m_stat == stat) {
        return;
    }
    
    m_stat = stat;
    m_type = type;
    
    GameLevelManager::sharedState()->m_leaderboardType = m_type;
    GameLevelManager::sharedState()->m_leaderboardStat = m_stat;
    
    toggleTabButtons();
    m_circle->setVisible(true);
    m_noInternet->setVisible(false);
    
    LeaderboardStat actualStat = (m_type == LeaderboardType::Creator) ? LeaderboardStat::Stars : m_stat;
    
    if (GameManager::sharedState()->m_scoreValid) {
        auto manager = GameLevelManager::sharedState();
        std::string key = manager->getLeaderboardKey(m_type, actualStat);
        auto scores = manager->getStoredOnlineLevels(key.c_str());
        
        if (scores) {
            loadLeaderboardFinished(scores, key.c_str());
        } else {
            setupLevelBrowser(nullptr);
            manager->m_leaderboardManagerDelegate = this;
            manager->getLeaderboardScores(m_type, actualStat);
        }
    } else {
        setupLevelBrowser(nullptr);
        GameLevelManager::sharedState()->m_leaderboardManagerDelegate = this;
        GameLevelManager::sharedState()->updateUserScore();
    }
}

void LeaderboardsLayer::setupLevelBrowser(cocos2d::CCArray* scores) {
    if (m_list) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto dispatcher = director->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_list);
        m_list->removeMeAndCleanup();
        m_list = nullptr;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    CustomListView* listView = scores ? CustomListView::create(scores, BoomListType::Score, 220.0f, 356.0f) : nullptr;
    
    m_list = GJListLayer::create(listView, nullptr, cocos2d::ccc4(0, 0, 0, 255), 356.0f, 220.0f, 0);
    addChild(m_list);
    m_list->setPosition(cocos2d::CCPoint((winSize.width - 356.0f) * 0.5f, (winSize.height - 220.0f) * 0.5f - 10.0f + 5.0f));
    
    refreshTabs();
}

void LeaderboardsLayer::toggleTabButtons() {
    m_topBtn->toggle(m_type == LeaderboardType::Top100);
    m_friendsBtn->toggle(m_type == LeaderboardType::Friends);
    m_globalBtn->toggle(m_type == LeaderboardType::Global);
    m_creatorsBtn->toggle(m_type == LeaderboardType::Creator);
    
    m_topBtn->setEnabled(m_type != LeaderboardType::Top100);
    m_friendsBtn->setEnabled(m_type != LeaderboardType::Friends);
    m_globalBtn->setEnabled(m_type != LeaderboardType::Global);
    m_creatorsBtn->setEnabled(m_type != LeaderboardType::Creator);
    
    int topZ = (m_type == LeaderboardType::Top100) ? 2 : -1;
    int friendsZ = (m_type == LeaderboardType::Friends) ? 2 : -1;
    int globalZ = (m_type == LeaderboardType::Global) ? 2 : -1;
    int creatorsZ = (m_type == LeaderboardType::Creator) ? 2 : -1;
    
    m_topBtn->getParent()->getParent()->setZOrder(topZ);
    m_friendsBtn->getParent()->getParent()->setZOrder(friendsZ);
    m_globalBtn->getParent()->getParent()->setZOrder(globalZ);
    m_creatorsBtn->getParent()->getParent()->setZOrder(creatorsZ);
    
    refreshTabs();
    
    if (m_modeButtons) {
        for (int i = 0; i < m_modeButtons->count(); ++i) {
            auto item = m_modeButtons->objectAtIndex(i);
            auto sprite = static_cast<ButtonSprite*>(item->getNormalImage());
            const char* frame = (item->getTag() == static_cast<int>(m_stat)) ? "GJ_button_02.png" : "GJ_button_01.png";
            sprite->updateBGImage(frame);
            item->setEnabled(m_type != LeaderboardType::Creator);
        }
    }
}

void LeaderboardsLayer::refreshTabs() {
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto tab = m_tabs->objectAtIndex(i);
        tab->setEnabled(false);
        tab->setVisible(true);
    }
}

void LeaderboardsLayer::loadLeaderboardFinished(cocos2d::CCArray* scores, const char* key) {
    if (isCorrect(key)) {
        if (m_userScores) {
            m_userScores->release();
        }
        m_userScores = scores;
        m_userScores->retain();
        setupLevelBrowser(m_userScores);
        m_circle->setVisible(false);
        m_noInternet->setVisible(false);
    }
}

void LeaderboardsLayer::loadLeaderboardFailed(const char* key) {
    if (isCorrect(key)) {
        m_circle->setVisible(false);
        
        if (PlatformToolbox::isNetworkAvailable()) {
            if (m_type == LeaderboardType::Friends) {
                m_noInternet->setString("Create an <cg>account</c> to\ncompare scores\nwith <cl>friends</c>!");
            }
        } else {
            m_noInternet->setString("No <cg>Internet</c> connection!");
        }
        m_noInternet->setVisible(true);
    }
}

bool LeaderboardsLayer::isCorrect(const char* key) {
    auto manager = GameLevelManager::sharedState();
    LeaderboardStat stat = (m_type == LeaderboardType::Creator) ? LeaderboardStat::Stars : m_stat;
    std::string correctKey = manager->getLeaderboardKey(m_type, stat);
    return key == correctKey;
}

void LeaderboardsLayer::updateUserScoreFinished() {
    if (m_type != LeaderboardType::Creator) {
        selectLeaderboard(LeaderboardType::Default, m_stat);
    }
}

void LeaderboardsLayer::updateUserScoreFailed() {
    m_circle->setVisible(false);
    if (!PlatformToolbox::isNetworkAvailable()) {
        m_noInternet->setVisible(true);
    }
}

void LeaderboardsLayer::onTop(cocos2d::CCObject* sender) {
    if (m_type != LeaderboardType::Top100) {
        selectLeaderboard(LeaderboardType::Top100, m_stat);
    }
}

void LeaderboardsLayer::onWeek(cocos2d::CCObject* sender) {
    if (m_type != LeaderboardType::Friends) {
        selectLeaderboard(LeaderboardType::Friends, m_stat);
    }
}

void LeaderboardsLayer::onGlobal(cocos2d::CCObject* sender) {
    if (m_type != LeaderboardType::Global) {
        selectLeaderboard(LeaderboardType::Global, m_stat);
    }
}

void LeaderboardsLayer::onCreators(cocos2d::CCObject* sender) {
    if (m_type != LeaderboardType::Creator) {
        selectLeaderboard(LeaderboardType::Creator, m_stat);
    }
}

void LeaderboardsLayer::onStat(cocos2d::CCObject* sender) {
    LeaderboardStat stat = static_cast<LeaderboardStat>(sender->getTag());
    selectLeaderboard(m_type, stat);
}

void LeaderboardsLayer::onInfo(cocos2d::CCObject* sender) {
    const char* title = "";
    const char* desc = "";
    
    switch (m_type) {
        case LeaderboardType::Top100:
            title = "Top 100";
            desc = "The <cg>Top 100</c> leaderboard shows the players with the most <cy>Stars</c> in the game. Players must be <cl>manually whitelisted</c> by a Leaderboard Moderator to appear here, ensuring legitimate rankings.";
            break;
        case LeaderboardType::Global:
            title = "Global";
            desc = "The <cg>Global</c> leaderboard shows players in order of <cy>Stars</c>.\nPlayers banned by Leaderboard Moderators are excluded.";
            break;
        case LeaderboardType::Creator:
            title = "Creators";
            desc = "The <cg>Creators</c> leaderboard is sorted by amount of Creator Points. Creating a rated level rewards 1, <cy>Featured</c> rewards 2, <co>Epic</c> rewards 3, <cp>Legendary</c> rewards 4, and <cj>Mythic</c> rewards 5.";
            break;
        case LeaderboardType::Friends:
            title = "Friends";
            desc = "The <cg>Friends</c> leaderboard shows your friends in order of <cy>Stars</c>.";
            break;
        default:
            break;
    }
    
    auto alert = FLAlertLayer::create(this, title, desc, "OK", nullptr, 380.0f);
    alert->show();
}

void LeaderboardsLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2) {
        cocos2d::CCApplication::sharedApplication()->openURL("https://www.geometrydash.com/support");
    }
}

void LeaderboardsLayer::onBack(cocos2d::CCObject* sender) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    setKeypadEnabled(false);
    GameManager::sharedState()->safePopScene();
}

void LeaderboardsLayer::keyBackClicked() {
    onBack(nullptr);
}

cocos2d::CCScene* LeaderboardsLayer::scene(LeaderboardType type, LeaderboardStat stat) {
    auto scene = cocos2d::CCScene::create();
    auto layer = LeaderboardsLayer::create(type, stat);
    scene->addChild(layer);
    return scene;
}

LeaderboardsLayer::~LeaderboardsLayer() {
    if (m_circle) {
        m_circle->release();
    }
    if (m_userScores) {
        m_userScores->release();
    }
    if (m_modeButtons) {
        m_modeButtons->release();
    }
    if (m_tabs) {
        m_tabs->release();
    }
}