#include "Layers/LevelSearchLayer.h"
#include "Layers/FLAlertLayer.h"
#include "Models/GJSearchObject.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Layers/LevelBrowserLayer.h"
#include "Layers/MoreSearchLayer.h"
#include "Layers/DemonFilterSelectLayer.h"
#include "UI/SearchButton.h"
#include "Cocos/CCTextInputNode.h"
#include "Models/GJGameLevel.h"
#include "Managers/GJAccountManager.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "CCScale9Sprite.h"

void LevelSearchLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (!layer->getTag() && btn2)
        clearFilters();
}

bool LevelSearchLayer::checkDiff(int diff)
{
    const char* diffKey = getDiffKey(diff);
    std::string key(diffKey);
    cocos2d::CCString* str = (cocos2d::CCString*)m_difficultyDict->valueForKey(key);
    return str->boolValue();
}

bool LevelSearchLayer::checkTime(int time)
{
    const char* timeKey = getTimeKey(time);
    std::string key(timeKey);
    cocos2d::CCString* str = (cocos2d::CCString*)m_difficultyDict->valueForKey(key);
    return str->boolValue();
}

void LevelSearchLayer::clearFilters()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->getBoolForKey("star_filter"))
        toggleStar(nullptr);

    for (int i = 0; i != 8; ++i)
        toggleDifficultyNum(i, false);

    for (int j = 0; j != 6; ++j)
        toggleTimeNum(j, false);

    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "uncompleted_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "completed_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "featured_filter");
    glm = GameLevelManager::sharedState();
    glm->setIntForKey(0, "song_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "original_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "twoP_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "enable_songFilter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "customsong_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "nostar_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "coin_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "follow_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "friend_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "epic_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "mythic_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(false, "legendary_filter");
    glm = GameLevelManager::sharedState();
    glm->setIntForKey(0, "demon_filter");
}

void LevelSearchLayer::confirmClearFilters(cocos2d::CCObject* sender)
{
    std::string message = "Do you want to <cr>reset</c> all search <cy>filters</c>?";
    FLAlertLayer* alert = FLAlertLayer::create(this, "Clear Filters", message);
    alert->setTag(0);
    alert->show();
}

LevelSearchLayer* LevelSearchLayer::create(int type)
{
    LevelSearchLayer* ret = new LevelSearchLayer();
    if (ret->init(type)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void LevelSearchLayer::demonFilterSelectClosed(int filter)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->setIntForKey(filter, "demon_filter");

    std::string frameName;
    if (filter) {
        cocos2d::CCString* str = cocos2d::CCString::createWithFormat("difficulty_%02d_btn2_001.png", filter);
        frameName = str->getCString();
    } else {
        frameName = "difficulty_06_btn_001.png";
    }

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    cocos2d::CCSpriteFrame* frame = cache->spriteFrameByName(frameName.c_str());
    m_lastDifficultySprite->setDisplayFrame(frame);
}

const char* LevelSearchLayer::getDiffKey(int diff)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("D%i", diff);
    return str->getCString();
}

std::string LevelSearchLayer::getLevelLenKey()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool isTiny = checkTime(0);
    bool isShort = checkTime(1);
    bool isMedium = checkTime(2);
    bool isLong = checkTime(3);
    bool isXL = checkTime(4);
    bool isPlat = checkTime(5);
    return glm->getLengthStr(isTiny, isShort, isMedium, isLong, isXL, isPlat);
}

std::string LevelSearchLayer::getSearchDiffKey()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool isAuto = checkDiff(0);
    bool isEasy = checkDiff(1);
    bool isNormal = checkDiff(2);
    bool isHard = checkDiff(3);
    bool isHarder = checkDiff(4);
    bool isInsane = checkDiff(5);
    bool isDemon = checkDiff(6);
    bool isAuto2 = checkDiff(7);
    return glm->getDifficultyStr(isAuto, isEasy, isNormal, isHard, isHarder, isInsane, isDemon, isAuto2);
}

GJSearchObject* LevelSearchLayer::getSearchObject(SearchType type, std::string query)
{
    std::string searchDiffKey = getSearchDiffKey();
    std::string levelLenKey = getLevelLenKey();

    GameLevelManager* glm = GameLevelManager::sharedState();
    bool starFilter = glm->getBoolForKey("star_filter");
    glm = GameLevelManager::sharedState();
    bool uncompletedFilter = glm->getBoolForKey("uncompleted_filter");
    glm = GameLevelManager::sharedState();
    bool featuredFilter = glm->getBoolForKey("featured_filter");
    glm = GameLevelManager::sharedState();
    int songFilter = glm->getIntForKey("song_filter");
    glm = GameLevelManager::sharedState();
    bool originalFilter = glm->getIntForKey("original_filter") != 0;
    glm = GameLevelManager::sharedState();
    bool twoPFilter = glm->getBoolForKey("twoP_filter");
    glm = GameLevelManager::sharedState();
    bool customsongFilter = glm->getBoolForKey("customsong_filter");
    glm = GameLevelManager::sharedState();
    bool enableSongFilter = glm->getBoolForKey("enable_songFilter");
    glm = GameLevelManager::sharedState();
    bool nostarFilter = glm->getBoolForKey("nostar_filter");
    glm = GameLevelManager::sharedState();
    bool coinFilter = glm->getBoolForKey("coin_filter");
    glm = GameLevelManager::sharedState();
    bool epicFilter = glm->getBoolForKey("epic_filter");
    glm = GameLevelManager::sharedState();
    bool mythicFilter = glm->getBoolForKey("mythic_filter");
    glm = GameLevelManager::sharedState();
    bool legendaryFilter = glm->getBoolForKey("legendary_filter");
    glm = GameLevelManager::sharedState();
    bool completedFilter = glm->getBoolForKey("completed_filter");
    glm = GameLevelManager::sharedState();
    int demonFilter = glm->getIntForKey("demon_filter");

    return GJSearchObject::create(
        type,
        query,
        searchDiffKey,
        levelLenKey,
        "",
        starFilter,
        uncompletedFilter,
        featuredFilter,
        songFilter,
        originalFilter,
        twoPFilter,
        customsongFilter,
        enableSongFilter,
        nostarFilter,
        coinFilter,
        epicFilter,
        mythicFilter,
        legendaryFilter,
        completedFilter,
        demonFilter,
        0,
        m_type);
}

const char* LevelSearchLayer::getTimeKey(int time)
{
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("T%i", time);
    return str->getCString();
}

bool LevelSearchLayer::init(int type)
{
    if (!cocos2d::CCLayer::init())
        return false;

    GameManager::sharedState()->m_currentLevelID = -1;
    GameManager::sharedState()->m_sceneEnum = 4;
    m_type = type;
    GameManager::sharedState()->m_levelSearchType = type;

    this->setKeypadEnabled(true);
    this->setKeyboardEnabled(true);

    m_difficultyDict = cocos2d::CCDictionary::create();
    m_difficultyDict->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCSprite* gradientBG = cocos2d::CCSprite::create("GJ_gradientBG.png");
    gradientBG->setAnchorPoint(ccp(0.0f, 0.0f));
    gradientBG->setScaleX((winSize.width + 10.0f) / gradientBG->getContentSize().width);
    gradientBG->setScaleY((winSize.height + 10.0f) / gradientBG->getContentSize().height);
    gradientBG->setPosition(ccp(-5.0f, -5.0f));
    gradientBG->setColor(ccc3(0, 102, 255));
    this->addChild(gradientBG, -3);

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

    cocos2d::CCRect rect = cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f);

    cocos2d::CCMenu* topMenu = cocos2d::CCMenu::create();
    this->addChild(topMenu, 2);

    cocos2d::CCPoint closePos = ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 25.0f);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(LevelSearchLayer::onClose));
    topMenu->addChild(closeBtn);
    closeBtn->setPosition(topMenu->convertToNodeSpace(closePos));

    cocos2d::CCSprite* plusSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    plusSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* plusBtn = CCMenuItemSpriteExtra::create(
        plusSprite, nullptr, this, menu_selector(LevelSearchLayer::onMoreOptions));
    topMenu->addChild(plusBtn);
    plusBtn->setSizeMult(1.5f);
    if (m_type == 1) {
        plusSprite->setColor(ccc3(125, 125, 125));
        plusBtn->setEnabled(false);
    }
    plusBtn->setPosition(closeBtn->getPosition() + ccp(0.0f, -50.0f));

    const char* searchModeFrame = m_type ? "GJ_viewLevelsBtn_001.png" : "GJ_viewListsBtn_001.png";
    cocos2d::CCSprite* searchModeSprite = cocos2d::CCSprite::createWithSpriteFrameName(searchModeFrame);
    searchModeSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* searchModeBtn = CCMenuItemSpriteExtra::create(
        searchModeSprite, nullptr, this, menu_selector(LevelSearchLayer::onSearchMode));
    topMenu->addChild(searchModeBtn);
    searchModeBtn->setPosition(plusBtn->getPosition() + ccp(0.0f, -50.0f));

    cocos2d::CCPoint searchBarPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f + 130.0f);

    cocos2d::extension::CCScale9Sprite* searchBG = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", rect);
    searchBG->setColor(ccc3(0, 0, 0));
    searchBG->setOpacity(90);
    searchBG->setContentSize(cocos2d::CCSize(365.0f, 40.0f));
    this->addChild(searchBG, -2);
    searchBG->setPosition(searchBarPos);

    cocos2d::extension::CCScale9Sprite* inputBG = cocos2d::extension::CCScale9Sprite::create(
        "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    inputBG->setColor(ccc3(0, 0, 0));
    inputBG->setOpacity(255);
    inputBG->setContentSize(cocos2d::CCSize(210.0f, 30.0f));
    this->addChild(inputBG, -1);
    inputBG->setPosition(searchBarPos + ccp(-73.0f, 0.0f));

    const char* placeholder = m_type == 1 ? "Enter a list or id" : "Enter a level, user or id";
    m_searchInput = CCTextInputNode::create(194.0f, 50.0f, placeholder, "Thonburi");
    this->addChild(m_searchInput, 20);
    m_searchInput->m_delegate = this;
    m_searchInput->setPosition(searchBarPos + ccp(-174.0f, 0.0f));
    m_searchInput->m_textField->setAnchorPoint(ccp(0.0f, 0.5f));
    m_searchInput->m_textLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    m_searchInput->setMaxLabelScale(0.7f);
    m_searchInput->setMaxLabelWidth(194.0f);
    m_searchInput->m_textField->setVisible(false);
    m_searchInput->m_maxLabelLength = 20;
    m_searchInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

    updateSearchLabel(placeholder);

    cocos2d::CCMenu* searchBtnMenu = cocos2d::CCMenu::create();
    this->addChild(searchBtnMenu);

    cocos2d::CCSprite* searchBtnSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_longBtn06_001.png");
    CCMenuItemSpriteExtra* searchBtn = CCMenuItemSpriteExtra::create(
        searchBtnSprite, nullptr, this, menu_selector(LevelSearchLayer::onSearch));
    searchBtnMenu->addChild(searchBtn);
    searchBtn->setPosition(searchBtnMenu->convertToNodeSpace(searchBarPos + ccp(63.0f, 0.0f)));

    cocos2d::CCSprite* userBtnSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_longBtn05_001.png");
    CCMenuItemSpriteExtra* userBtn = CCMenuItemSpriteExtra::create(
        userBtnSprite, nullptr, this, menu_selector(LevelSearchLayer::onSearchUser));
    searchBtnMenu->addChild(userBtn);
    userBtn->setPosition(searchBtnMenu->convertToNodeSpace(searchBarPos + ccp(110.0f, 0.0f)));

    cocos2d::CCSprite* clearBtnSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_longBtn07_001.png");
    CCMenuItemSpriteExtra* clearBtn = CCMenuItemSpriteExtra::create(
        clearBtnSprite, nullptr, this, menu_selector(LevelSearchLayer::onClearFreeSearch));
    searchBtnMenu->addChild(clearBtn);
    clearBtn->setPosition(searchBtnMenu->convertToNodeSpace(searchBarPos + ccp(157.0f, 0.0f)));

    cocos2d::CCPoint categoriesPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f + 28.0f);

    cocos2d::CCLabelBMFont* categoriesLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    categoriesLabel->setPosition(categoriesPos + ccp(0.0f, 69.5f));
    this->addChild(categoriesLabel);
    categoriesLabel->setScale(0.5f);

    cocos2d::extension::CCScale9Sprite* categoriesBG = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", rect);
    categoriesBG->setColor(ccc3(0, 0, 0));
    categoriesBG->setOpacity(75);
    categoriesBG->setContentSize(cocos2d::CCSize(365.0f, 115.0f));
    this->addChild(categoriesBG, -1);
    categoriesBG->setPosition(categoriesPos);

    cocos2d::CCMenu* categoriesMenu = cocos2d::CCMenu::create();
    this->addChild(categoriesMenu);
    cocos2d::CCPoint categoriesMenuPos = categoriesMenu->convertToNodeSpace(categoriesPos);

    SearchButton* downloadsBtn = SearchButton::create("GJ_longBtn04_001.png", "Downloads", 0.5f, "GJ_sDownloadIcon_001.png");
    CCMenuItemSpriteExtra* downloadsItem = CCMenuItemSpriteExtra::create(
        downloadsBtn, nullptr, this, menu_selector(LevelSearchLayer::onMostDownloaded));
    categoriesMenu->addChild(downloadsItem);

    SearchButton* likesBtn = SearchButton::create("GJ_longBtn04_001.png", "Likes", 0.5f, "GJ_sLikeIcon_001.png");
    CCMenuItemSpriteExtra* likesItem = CCMenuItemSpriteExtra::create(
        likesBtn, nullptr, this, menu_selector(LevelSearchLayer::onMostLikes));
    categoriesMenu->addChild(likesItem);

    SearchButton* sentBtn = SearchButton::create("GJ_longBtn04_001.png", "Sent", 0.5f, "GJ_sModIcon_001.png");
    sentBtn->m_icon->setPosition(sentBtn->m_icon->getPosition() + ccp(0.0f, 0.5f));
    CCMenuItemSpriteExtra* sentItem = CCMenuItemSpriteExtra::create(
        sentBtn, nullptr, this, menu_selector(LevelSearchLayer::onSuggested));
    categoriesMenu->addChild(sentItem);

    SearchButton* trendingBtn = SearchButton::create("GJ_longBtn04_001.png", "Trending", 0.5f, "GJ_sTrendingIcon_001.png");
    CCMenuItemSpriteExtra* trendingItem = CCMenuItemSpriteExtra::create(
        trendingBtn, nullptr, this, menu_selector(LevelSearchLayer::onTrending));
    categoriesMenu->addChild(trendingItem);

    SearchButton* recentBtn = SearchButton::create("GJ_longBtn04_001.png", "Recent", 0.5f, "GJ_sRecentIcon_001.png");
    CCMenuItemSpriteExtra* recentItem = CCMenuItemSpriteExtra::create(
        recentBtn, nullptr, this, menu_selector(LevelSearchLayer::onMostRecent));
    categoriesMenu->addChild(recentItem);

    SearchButton* magicBtn = SearchButton::create("GJ_longBtn04_001.png", "Magic", 0.5f, "GJ_sMagicIcon_001.png");
    CCMenuItemSpriteExtra* magicItem = CCMenuItemSpriteExtra::create(
        magicBtn, nullptr, this, menu_selector(LevelSearchLayer::onMagic));
    categoriesMenu->addChild(magicItem);

    SearchButton* awardedBtn = SearchButton::create("GJ_longBtn04_001.png", "Awarded", 0.5f, "GJ_sStarsIcon_001.png");
    CCMenuItemSpriteExtra* awardedItem = CCMenuItemSpriteExtra::create(
        awardedBtn, nullptr, this, menu_selector(LevelSearchLayer::onLatestStars));
    categoriesMenu->addChild(awardedItem);

    SearchButton* followedBtn = SearchButton::create("GJ_longBtn04_001.png", "Followed", 0.5f, "GJ_sFollowedIcon_001.png");
    CCMenuItemSpriteExtra* followedItem = CCMenuItemSpriteExtra::create(
        followedBtn, nullptr, this, menu_selector(LevelSearchLayer::onFollowed));
    categoriesMenu->addChild(followedItem);

    SearchButton* friendsBtn = SearchButton::create("GJ_longBtn04_001.png", "Friends", 0.5f, "GJ_sFriendsIcon_001.png");
    CCMenuItemSpriteExtra* friendsItem = CCMenuItemSpriteExtra::create(
        friendsBtn, nullptr, this, menu_selector(LevelSearchLayer::onFriends));
    categoriesMenu->addChild(friendsItem);

    downloadsItem->setPosition(categoriesMenuPos + ccp(-119.0f, 36.0f));
    likesItem->setPosition(categoriesMenuPos + ccp(0.0f, 36.0f));
    sentItem->setPosition(categoriesMenuPos + ccp(119.0f, 36.0f));
    trendingItem->setPosition(categoriesMenuPos + ccp(-119.0f, 0.0f));
    recentItem->setPosition(categoriesMenuPos + ccp(0.0f, 0.0f));
    magicItem->setPosition(categoriesMenuPos + ccp(119.0f, 0.0f));
    awardedItem->setPosition(categoriesMenuPos + ccp(-119.0f, -36.0f));
    followedItem->setPosition(categoriesMenuPos + ccp(0.0f, -36.0f));
    friendsItem->setPosition(categoriesMenuPos + ccp(119.0f, -36.0f));

    GJAccountManager* accountManager = GJAccountManager::sharedState();
    if (accountManager->m_accountID - accountManager->m_unkInt1 == 2795) {
        SearchButton* starAwardBtn = SearchButton::create("GJ_longBtn04_001.png", "Star Award", 0.5f, "GJ_sMagicIcon_001.png");
        CCMenuItemSpriteExtra* starAwardItem = CCMenuItemSpriteExtra::create(
            starAwardBtn, nullptr, this, menu_selector(LevelSearchLayer::onStarAward));
        categoriesMenu->addChild(starAwardItem);
        starAwardItem->setPosition(categoriesMenuPos + ccp(0.0f, -76.0f));
    }

    cocos2d::CCPoint difficultyPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f - 77.0f);

    cocos2d::CCLabelBMFont* difficultyLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    difficultyLabel->setPosition(difficultyPos + ccp(0.0f, 37.0f));
    this->addChild(difficultyLabel);
    difficultyLabel->setScale(0.5f);

    cocos2d::extension::CCScale9Sprite* difficultyBG = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", rect);
    difficultyBG->setColor(ccc3(0, 0, 0));
    difficultyBG->setOpacity(75);
    difficultyBG->setContentSize(cocos2d::CCSize(365.0f, 50.0f));
    this->addChild(difficultyBG, -1);
    difficultyBG->setPosition(difficultyPos);

    m_difficultySprites = cocos2d::CCArray::create();
    m_difficultySprites->retain();

    cocos2d::CCArray* difficultyItems = cocos2d::CCArray::create();

    cocos2d::CCMenu* difficultyMenu = cocos2d::CCMenu::create();
    this->addChild(difficultyMenu, 10);

    for (int i = 0; i < 8; ++i) {
        cocos2d::CCString* frameStr = cocos2d::CCString::createWithFormat("difficulty_%02d_btn_001.png", i);
        const char* frameName = frameStr->getCString();
        if (i == 7)
            frameName = "difficulty_auto_btn_001.png";

        cocos2d::CCSprite* diffSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        diffSprite->setColor(ccc3(125, 125, 125));
        m_difficultySprites->addObject(diffSprite);
        diffSprite->setScale(0.8f);

        CCMenuItemSpriteExtra* diffItem = CCMenuItemSpriteExtra::create(
            diffSprite, nullptr, this, menu_selector(LevelSearchLayer::toggleDifficulty));
        diffItem->setTag(i);
        difficultyMenu->addChild(diffItem);
        difficultyItems->addObject(diffItem);

        if (i == 6)
            m_lastDifficultySprite = diffSprite;
    }

    cocos2d::CCPoint diffMenuPos = difficultyMenu->convertToNodeSpace(difficultyPos);
    GameToolbox::alignItemsHorisontally(difficultyItems, 18.0f, diffMenuPos, false);

    for (unsigned int j = 0; j < difficultyMenu->getChildrenCount(); ++j) {
        CCMenuItemSpriteExtra* item = (CCMenuItemSpriteExtra*)difficultyMenu->getChildren()->objectAtIndex(j);
        item->setSizeMult(1.5f);
    }

    cocos2d::CCSprite* demonFilterSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    m_demonTypeButton = CCMenuItemSpriteExtra::create(
        demonFilterSprite, nullptr, this, menu_selector(LevelSearchLayer::onSpecialDemon));
    difficultyMenu->addChild(m_demonTypeButton);
    m_demonTypeButton->setPosition(diffMenuPos + ccp(202.5f, 0.0f));

    cocos2d::CCPoint lengthPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f - 128.0f);

    cocos2d::extension::CCScale9Sprite* lengthBG = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", rect);
    lengthBG->setColor(ccc3(0, 0, 0));
    lengthBG->setOpacity(75);
    lengthBG->setContentSize(cocos2d::CCSize(365.0f, 35.0f));
    this->addChild(lengthBG, -1);
    lengthBG->setPosition(lengthPos);

    m_lengthSprites = cocos2d::CCArray::create();
    m_lengthSprites->retain();

    difficultyItems->removeAllObjects();

    cocos2d::CCMenu* lengthMenu = cocos2d::CCMenu::create();
    this->addChild(lengthMenu, 10);

    if (!m_type) {
        cocos2d::CCSprite* timeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
        CCMenuItemSpriteExtra* timeIconItem = CCMenuItemSpriteExtra::create(timeIcon, nullptr, nullptr, nullptr);
        lengthMenu->addChild(timeIconItem);
        timeIconItem->setEnabled(false);
        difficultyItems->addObject(timeIconItem);

        for (int k = 0; k < 6; ++k) {
            std::string lengthStr = GJGameLevel::lengthKeyToString(k);
            cocos2d::CCLabelBMFont* lengthLabel = cocos2d::CCLabelBMFont::create(lengthStr.c_str(), "bigFont.fnt");
            lengthLabel->setColor(ccc3(125, 125, 125));
            m_lengthSprites->addObject(lengthLabel);
            lengthLabel->setScale(0.42f);

            CCMenuItemSpriteExtra* lengthItem = CCMenuItemSpriteExtra::create(
                lengthLabel, nullptr, this, menu_selector(LevelSearchLayer::toggleTime));
            lengthItem->setTag(k);
            lengthMenu->addChild(lengthItem);
            difficultyItems->addObject(lengthItem);
        }
    }

    m_starsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    CCMenuItemSpriteExtra* starsItem = CCMenuItemSpriteExtra::create(
        m_starsSprite, nullptr, this, menu_selector(LevelSearchLayer::toggleStar));
    lengthMenu->addChild(starsItem);
    difficultyItems->addObject(starsItem);

    cocos2d::CCPoint lengthMenuPos = lengthMenu->convertToNodeSpace(lengthPos);
    GameToolbox::alignItemsHorisontally(difficultyItems, 12.0f, lengthMenuPos, false);

    for (unsigned int m = 0; m < lengthMenu->getChildrenCount(); ++m) {
        CCMenuItemSpriteExtra* item = (CCMenuItemSpriteExtra*)lengthMenu->getChildren()->objectAtIndex(m);
        item->setSizeMult(1.3f);
    }

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        backSprite, nullptr, this, menu_selector(LevelSearchLayer::onBack));
    backBtn->setSizeMult(1.5f);
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    this->addChild(backMenu, 1);
    backMenu->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected())
        GameToolbox::addBackButton(this, backBtn);

    for (int n = 0; n < 8; ++n) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        bool diffVal = glm->getDiffVal(n);
        toggleDifficultyNum(n, diffVal);
    }

    for (int ii = 0; ii < 6; ++ii) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        bool lenVal = glm->getLenVal(ii);
        toggleTimeNum(ii, lenVal);
    }

    GameLevelManager* glm = GameLevelManager::sharedState();
    bool starFilter = glm->getBoolForKey("star_filter");
    ccColor3B starColor = starFilter ? ccc3(255, 255, 255) : ccc3(125, 125, 125);
    m_starsSprite->setColor(starColor);

    const char* demonDiffKey = getDiffKey(6);
    std::string demonKey(demonDiffKey);
    cocos2d::CCString* demonStr = (cocos2d::CCString*)m_difficultyDict->valueForKey(demonKey);
    bool demonEnabled = demonStr->boolValue();
    m_demonTypeButton->setEnabled(demonEnabled);
    m_demonTypeButton->setVisible(demonEnabled);

    glm = GameLevelManager::sharedState();
    int demonFilter = glm->getIntForKey("demon_filter");
    demonFilterSelectClosed(demonFilter);

    return true;
}

void LevelSearchLayer::keyBackClicked()
{
    onBack(nullptr);
}

void LevelSearchLayer::onBack(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    GameManager* gm = GameManager::sharedState();
    if (gm->safePopScene())
        this->setKeypadEnabled(false);
}

void LevelSearchLayer::onClearFreeSearch(cocos2d::CCObject* sender)
{
    std::string empty = "";
    m_searchInput->setString(empty);
}

void LevelSearchLayer::onClose(cocos2d::CCObject* sender)
{
    m_searchInput->onClickTrackNode(false);
}

void LevelSearchLayer::onFollowed(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::Followed, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onFriends(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject((SearchType)((int)SearchType::Followed | (int)SearchType::Downloaded), query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onLatestStars(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::Awarded, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onMagic(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject((SearchType)((int)SearchType::UsersLevels | (int)SearchType::MostLiked), query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onMoreOptions(cocos2d::CCObject* sender)
{
    MoreSearchLayer* layer = MoreSearchLayer::create();
    layer->show();
}

void LevelSearchLayer::onMostDownloaded(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::Downloaded, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onMostLikes(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::MostLiked, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onMostRecent(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::Recent, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onPasteClipboard(cocos2d::CCObject* sender)
{
    std::string clipboard = PlatformToolbox::getClipboardString();
    std::string current = m_searchInput->getString();
    std::string combined = current + clipboard;
    m_searchInput->setString(combined);
}

void LevelSearchLayer::onSearch(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = m_searchInput->getString();
    std::string empty = "";
    if (query != empty) {
        GJSearchObject* searchObj = getSearchObject((SearchType)0, query);
        cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(transition);
    } else {
        onMostLikes(nullptr);
    }
}

void LevelSearchLayer::onSearchMode(cocos2d::CCObject* sender)
{
    bool newType = m_type == 0;
    cocos2d::CCScene* scene = LevelSearchLayer::scene(newType);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

void LevelSearchLayer::onSearchUser(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = m_searchInput->getString();
    std::string empty = "";
    if (query != empty) {
        GJSearchObject* searchObj = getSearchObject(SearchType::Users, query);
        searchObj->m_searchMode = 0;
        cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(transition);
    }
}

void LevelSearchLayer::onSpecialDemon(cocos2d::CCObject* sender)
{
    DemonFilterSelectLayer* layer = DemonFilterSelectLayer::create();
    layer->m_scene = this;
    layer->m_delegate = this;
    layer->m_noElasticity = true;
    layer->show();
}

void LevelSearchLayer::onStarAward(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::StarAward, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onSuggested(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::Sent, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void LevelSearchLayer::onTrending(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    std::string query = "";
    GJSearchObject* searchObj = getSearchObject(SearchType::Trending, query);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

cocos2d::CCScene* LevelSearchLayer::scene(int type)
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    LevelSearchLayer* layer = LevelSearchLayer::create(type);
    scene->addChild(layer);
    return scene;
}

void LevelSearchLayer::textChanged(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void LevelSearchLayer::textInputClosed(CCTextInputNode* node)
{
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void LevelSearchLayer::textInputOpened(CCTextInputNode* node)
{
    std::string text = node->getString();
    std::string empty = "";
    if (text == empty) {
        m_searchInput->updateLabel(" ");
    }
}

void LevelSearchLayer::toggleDifficulty(cocos2d::CCObject* sender)
{
    int diff = sender->getTag();
    bool enabled = checkDiff(diff);
    if (m_type == 1) {
        for (int i = 0; i != 8; ++i) {
            if (i != diff)
                toggleDifficultyNum(i, false);
        }
    }
    int newVal = enabled ^ 1;
    cocos2d::CCString* valStr = cocos2d::CCString::createWithFormat("%i", enabled ^ 1);
    const char* diffKey = getDiffKey(diff);
    std::string key(diffKey);
    m_difficultyDict->setObject(valStr, key);
    cocos2d::CCObject* sprite = m_difficultySprites->objectAtIndex(diff);
    ccColor3B color = newVal ? ccc3(255, 255, 255) : ccc3(125, 125, 125);
    ((cocos2d::CCSprite*)sprite)->setColor(color);
    if ((diff == 0) > enabled) {
        for (int j = 1; j != 8; ++j)
            toggleDifficultyNum(j, false);
    } else if ((diff == 6) > enabled) {
        for (int k = 0; k != 6; ++k)
            toggleDifficultyNum(k, false);
    } else if ((diff == 7) <= enabled) {
        toggleDifficultyNum(0, false);
        toggleDifficultyNum(6, false);
        toggleDifficultyNum(7, false);
    } else {
        for (int m = 0; m != 7; ++m)
            toggleDifficultyNum(m, false);
    }
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->setDiffVal(diff, newVal);
    const char* demonKey = getDiffKey(6);
    std::string demonKeyStr(demonKey);
    cocos2d::CCString* demonStr = (cocos2d::CCString*)m_difficultyDict->valueForKey(demonKeyStr);
    bool demonEnabled = demonStr->boolValue();
    m_demonTypeButton->setEnabled(demonEnabled);
    m_demonTypeButton->setVisible(demonEnabled);
}

void LevelSearchLayer::toggleDifficultyNum(int diff, bool enabled)
{
    cocos2d::CCString* valStr = cocos2d::CCString::createWithFormat("%i", enabled);
    const char* diffKey = getDiffKey(diff);
    std::string key(diffKey);
    m_difficultyDict->setObject(valStr, key);
    cocos2d::CCObject* sprite = m_difficultySprites->objectAtIndex(diff);
    ccColor3B color = enabled ? ccc3(255, 255, 255) : ccc3(125, 125, 125);
    ((cocos2d::CCSprite*)sprite)->setColor(color);
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->setDiffVal(diff, enabled);
}

void LevelSearchLayer::toggleStar(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool newVal = !glm->getBoolForKey("star_filter");
    ccColor3B color = newVal ? ccc3(255, 255, 255) : ccc3(125, 125, 125);
    m_starsSprite->setColor(color);
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(newVal, "star_filter");
}

void LevelSearchLayer::toggleTime(cocos2d::CCObject* sender)
{
    int time = sender->getTag();
    bool newVal = !checkTime(time);
    cocos2d::CCString* valStr = cocos2d::CCString::createWithFormat("%i", newVal);
    const char* timeKey = getTimeKey(time);
    std::string key(timeKey);
    m_difficultyDict->setObject(valStr, key);
    cocos2d::CCObject* sprite = m_lengthSprites->objectAtIndex(time);
    ccColor3B color = newVal ? ccc3(255, 255, 255) : ccc3(125, 125, 125);
    ((cocos2d::CCSprite*)sprite)->setColor(color);
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->setLenVal(time, newVal);
}

void LevelSearchLayer::toggleTimeNum(int time, bool enabled)
{
    if (m_type != 1) {
        cocos2d::CCString* valStr = cocos2d::CCString::createWithFormat("%i", enabled);
        const char* timeKey = getTimeKey(time);
        std::string key(timeKey);
        m_difficultyDict->setObject(valStr, key);
        cocos2d::CCObject* sprite = m_lengthSprites->objectAtIndex(time);
        ccColor3B color = enabled ? ccc3(255, 255, 255) : ccc3(125, 125, 125);
        ((cocos2d::CCSprite*)sprite)->setColor(color);
        GameLevelManager* glm = GameLevelManager::sharedState();
        glm->setLenVal(time, enabled);
    }
}

void LevelSearchLayer::updateSearchLabel(const char* label)
{
    cocos2d::CCLabelBMFont* textLabel = m_searchInput->m_textLabel;
    const char* placeholder = m_type == 1 ? "Enter a list or id" : "Enter a level, user or id";
    std::string labelStr(label);
    std::string empty = "";
    bool isPlaceholder = labelStr == empty;
    bool isDefault = labelStr == placeholder;
    if (isPlaceholder || isDefault) {
        m_searchInput->updateLabel(placeholder);
        textLabel->setColor(ccc3(255, 255, 255));
    } else {
        textLabel->setColor(ccc3(255, 255, 216));
    }
}

LevelSearchLayer::~LevelSearchLayer()
{
    CC_SAFE_RELEASE(m_difficultyDict);
    CC_SAFE_RELEASE(m_difficultySprites);
    CC_SAFE_RELEASE(m_lengthSprites);
}