#include "InfoLayer.h"
#include "GJGameLevel.h"
#include "GJUserScore.h"
#include "GJLevelList.h"
#include "GJCommentListLayer.h"
#include "LoadingCircle.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameLevelManager.h"
#include "GameManager.h"
#include "GJAccountManager.h"
#include "GameToolbox.h"
#include "PlatformToolbox.h"
#include "FLAlertLayer.h"
#include "ShareCommentLayer.h"
#include "ProfilePage.h"
#include "LevelBrowserLayer.h"
#include "GJSearchObject.h"
#include "CommentUploadDelegate.h"
#include "LevelCommentDelegate.h"
#include "TextArea.h"
#include "ButtonSprite.h"
#include "CustomListView.h"

InfoLayer* InfoLayer::create(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
    auto layer = new InfoLayer();
    if (layer && layer->init(level, score, list)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool InfoLayer::init(GJGameLevel* level, GJUserScore* score, GJLevelList* list) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }
    
    if (score) {
        m_score = score;
        m_score->retain();
        m_mode = 1;
    } else if (list) {
        m_levelList = list;
        m_levelList->retain();
        m_mode = 2;
    } else {
        m_level = level;
        m_level->retain();
        m_mode = 0;
    }
    
    bool commentsDisabled = GameManager::sharedState()->getGameVariable("0075");
    
    bool extendedMode = GameManager::sharedState()->getGameVariable("0089");
    if (!commentsDisabled) {
        if (!extendedMode && !m_score) {
            GameManager::sharedState()->setGameVariable("0089", false);
        }
    }
    if (commentsDisabled || m_score) {
        extendedMode = true;
    }
    
    GameLevelManager::sharedState()->m_commentUploadDelegate = this;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Title and creator info
    cocos2d::CCLabelBMFont* titleLabel = nullptr;
    cocos2d::CCLabelBMFont* creatorLabel = nullptr;
    int accountId = 0;
    std::string creatorName;
    
    if (m_score) {
        titleLabel = cocos2d::CCLabelBMFont::create((m_score->m_userName + "'s comments").c_str(), "bigFont.fnt");
        titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 129.0f));
        titleLabel->limitLabelWidth(180.0f, 0.55f, 0.0f);
        m_mainLayer->addChild(titleLabel);
    } else {
        std::string titleText;
        if (m_levelList) {
            titleText = m_levelList->m_listName;
            creatorName = m_levelList->m_creatorName;
            accountId = m_levelList->m_accountID;
        } else {
            titleText = m_level->m_levelName;
            creatorName = m_level->m_creatorName;
            accountId = m_level->m_accountID.random - m_level->m_accountID.seed;
            if (accountId == 0) {
                accountId = GameLevelManager::sharedState()->accountIDForUserID(
                    m_level->m_userID.random - m_level->m_userID.seed
                );
            }
        }
        
        float titleY = extendedMode ? centerY + 129.0f : centerY + 125.0f;
        titleLabel = cocos2d::CCLabelBMFont::create(titleText.c_str(), "bigFont.fnt");
        titleLabel->setPosition(cocos2d::CCPoint(centerX, titleY));
        titleLabel->limitLabelWidth(extendedMode ? 150.0f : 300.0f, extendedMode ? 0.6f : 1.0f, 0.0f);
        m_mainLayer->addChild(titleLabel);
        
        std::string creatorText = "By " + creatorName;
        creatorLabel = cocos2d::CCLabelBMFont::create(creatorText.c_str(), "bigFont.fnt");
        creatorLabel->limitLabelWidth(extendedMode ? 150.0f : 300.0f, extendedMode ? 0.5f : 0.8f, 0.0f);
        
        if (extendedMode) {
            auto titleSize = titleLabel->getScaledContentSize();
            auto creatorSize = creatorLabel->getScaledContentSize();
            float totalWidth = titleSize.width + creatorSize.width + 15.0f;
            float offset = (centerX - totalWidth * 0.5f) - (centerX - titleSize.width * 0.5f);
            titleLabel->setPosition(cocos2d::CCPoint(centerX + offset, titleLabel->getPosition().y));
            creatorLabel->setPosition(cocos2d::CCPoint(
                titleLabel->getPosition().x + titleSize.width * 0.5f + creatorSize.width * 0.5f + 15.0f,
                titleLabel->getPosition().y
            ));
        } else {
            creatorLabel->setPosition(cocos2d::CCPoint(centerX, titleLabel->getPosition().y - 26.0f));
        }
        m_mainLayer->addChild(creatorLabel);
        
        auto moreItem = CCMenuItemSpriteExtra::create(creatorLabel, this, menu_selector(InfoLayer::onMore));
        m_buttonMenu->addChild(moreItem);
        moreItem->setPosition(m_buttonMenu->convertToNodeSpace(creatorLabel->getPosition()));
        if (accountId == 0) {
            creatorLabel->setColor(cocos2d::ccc3(100, 100, 255));
            moreItem->setEnabled(false);
        }
    }
    
    // Description
    if (!extendedMode) {
        cocos2d::CCPoint descPos(centerX, centerY + 52.0f);
        auto descBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        descBg->setContentSize(cocos2d::CCSize(340.0f, 55.0f));
        descBg->setPosition(descPos);
        descBg->setColor(cocos2d::ccc3(165, 33, 33));
        m_mainLayer->addChild(descBg, -1);
        
        std::string descText = "(No description provided)";
        if (m_levelList) {
            descText = m_levelList->getUnpackedDescription();
        } else if (m_level) {
            descText = m_level->getUnpackedLevelDescription();
        }
        if (descText.empty()) {
            descText = "(No description provided)";
        }
        
        float scale = 1.0f;
        if (descText.length() > 180) {
            scale = 0.7f + (180.0f - static_cast<float>(descText.length())) / 300.0f * 0.3f;
        }
        
        auto textArea = TextArea::create(descText, "chatFont.fnt", 1.0f, 295.0f / scale, cocos2d::CCPoint(20.0f, 0.0f), 0.5f, cocos2d::CCPoint(0.5f, 0.5f));
        m_mainLayer->addChild(textArea);
        textArea->setPosition(descPos + cocos2d::CCPoint(0.0f, 1.0f));
        textArea->setScale(scale);
    }
    
    setupLevelInfo();
    
    // Original button for copied levels
    if (m_level && m_level->m_originalLevel.random != m_level->m_originalLevel.seed &&
        m_level->m_originalLevel.random - m_level->m_originalLevel.seed != m_level->m_levelID.random - m_level->m_levelID.seed &&
        !GameManager::sharedState()->getGameVariable("0077")) {
        auto origBtn = ButtonSprite::create("Original", 0, 0, 0.45f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 25.0f);
        origBtn->setScale(0.8f);
        auto origItem = CCMenuItemSpriteExtra::create(origBtn, this, menu_selector(InfoLayer::onOriginal));
        origItem->setSizeMult(1.2f);
        m_buttonMenu->addChild(origItem);
        cocos2d::CCPoint pos = moreItem->getPosition();
        origItem->setPosition(pos + cocos2d::CCPoint(creatorLabel->getContentSize().width * creatorLabel->getScale() * 0.5f + 70.0f, 0.0f));
    }
    
    // Comment button
    auto commentSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
    auto commentItem = CCMenuItemSpriteExtra::create(commentSprite, this, menu_selector(InfoLayer::onComment));
    commentItem->setSizeMult(1.5f);
    m_buttonMenu->addChild(commentItem);
    commentItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 210.0f, centerY - 135.0f)));
    
    // Report button
    auto reportSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
    m_reportBtn = CCMenuItemSpriteExtra::create(reportSprite, this, menu_selector(InfoLayer::confirmReport));
    m_reportBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_reportBtn);
    m_reportBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 210.0f, centerY - 135.0f)));
    
    bool canReport = !GameLevelManager::sharedState()->hasReportedLevel(getID());
    m_reportBtn->getNormalImage()->setColor(canReport ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(75, 75, 75));
    m_reportBtn->setEnabled(canReport);
    
    if (commentsDisabled) {
        commentItem->setVisible(false);
        commentItem->setEnabled(false);
    }
    
    // Close button
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    auto closeItem = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(InfoLayer::onClose));
    closeItem->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeItem);
    closeItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 210.0f, centerY + 135.0f)));
    
    // Loading circle
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->retain();
    m_loadingCircle->show();
    m_loadingCircle->setVisible(false);
    m_loadingCircle->m_sprite->setPosition(cocos2d::CCPoint(centerX, centerY - 53.0f));
    
    // Navigation
    auto navMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(navMenu, 10);
    
    float navY = centerY - 53.0f;
    
    auto leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftSprite->setScale(0.8f);
    m_leftArrow = CCMenuItemSpriteExtra::create(leftSprite, this, menu_selector(InfoLayer::onPrevPage));
    m_leftArrow->setSizeMult(2.0f);
    navMenu->addChild(m_leftArrow);
    m_leftArrow->setPosition(navMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 195.0f, navY)));
    
    auto rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightSprite->setFlipX(true);
    rightSprite->setScale(0.8f);
    m_rightArrow = CCMenuItemSpriteExtra::create(rightSprite, this, menu_selector(InfoLayer::onNextPage));
    m_rightArrow->setSizeMult(2.0f);
    navMenu->addChild(m_rightArrow);
    m_rightArrow->setPosition(navMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 195.0f, navY)));
    
    m_leftArrow->setVisible(false);
    m_rightArrow->setVisible(false);
    
    m_pageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_pageLabel, 10);
    m_pageLabel->setPosition(cocos2d::CCPoint(director->getScreenRight() - 7.0f, director->getScreenTop() - 2.0f));
    m_pageLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    m_pageLabel->setScale(0.4f);
    
    // Comment mode buttons
    auto modeMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(modeMenu, 10);
    modeMenu->setPosition(cocos2d::CCPoint(centerX - 195.0f, centerY + 35.0f));
    modeMenu->alignItemsVerticallyWithPadding(4.0f);
    
    m_likeBtn = getSpriteButton("GJ_likesIcon_001.png", menu_selector(InfoLayer::toggleCommentMode), modeMenu, 0.7f, cocos2d::CCPoint(0.0f, -0.5f));
    m_likeBtn->setTag(0);
    
    m_timeBtn = getSpriteButton("GJ_timeIcon_001.png", menu_selector(InfoLayer::toggleCommentMode), modeMenu, 0.7f, cocos2d::CCPoint(0.0f, -1.0f));
    m_timeBtn->setTag(1);
    
    if (!m_score && !commentsDisabled) {
        auto extItem = getSpriteButton("GJ_extendedIcon_001.png", menu_selector(InfoLayer::toggleExtendedMode), modeMenu, 0.7f, cocos2d::CCPoint(0.0f, -1.0f));
        auto extSprite = static_cast<ButtonSprite*>(extItem->getNormalImage());
        extSprite->updateBGImage(extendedMode ? "GJ_button_02.png" : "GJ_button_01.png");
    }
    
    auto smallItem = getSpriteButton("GJ_smallModeIcon_001.png", menu_selector(InfoLayer::toggleSmallCommentMode), modeMenu, 0.7f, cocos2d::CCPoint(0.0f, -1.5f));
    auto smallSprite = static_cast<ButtonSprite*>(smallItem->getNormalImage());
    smallSprite->updateBGImage(GameManager::sharedState()->getGameVariable("0088") ? "GJ_button_02.png" : "GJ_button_01.png");
    
    updateCommentModeButtons();
    
    // Load comments button
    auto loadBtn = ButtonSprite::create("Load Comments", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 25.0f);
    m_commentsBtn = CCMenuItemSpriteExtra::create(loadBtn, this, menu_selector(InfoLayer::onGetComments));
    m_commentsBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(m_commentsBtn);
    m_commentsBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX, centerY - 53.0f)));
    
    m_noComments = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_noComments, 10);
    m_noComments->setPosition(cocos2d::CCPoint(centerX, centerY - 53.0f));
    m_noComments->setScale(0.8f);
    m_noComments->setVisible(false);
    
    if (!PlatformToolbox::isNetworkAvailable()) {
        m_noComments->setVisible(true);
        m_commentsBtn->setVisible(false);
    }
    
    // Load stored comments or fetch
    bool forceUpdate = GameManager::sharedState()->getGameVariable("0090");
    if (!commentsDisabled) {
        auto key = GameLevelManager::sharedState()->getCommentKey(getID(), 0, GameManager::sharedState()->getGameVariable("0069"), m_mode);
        auto stored = GameLevelManager::sharedState()->getStoredLevelComments(key);
        if (stored || forceUpdate) {
            loadPage(0, false);
        } else {
            setupCommentsBrowser(nullptr);
        }
    } else {
        m_noComments->setVisible(false);
        m_commentsBtn->setVisible(false);
        m_likeBtn->setVisible(false);
        m_likeBtn->setEnabled(false);
        m_timeBtn->setVisible(false);
        m_timeBtn->setEnabled(false);
        m_refreshCommentsBtn->setVisible(false);
        m_refreshCommentsBtn->setEnabled(false);
    }
    
    // Info button
    auto infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    auto infoItem = CCMenuItemSpriteExtra::create(infoSprite, this, menu_selector(InfoLayer::onLevelInfo));
    infoItem->setSizeMult(2.0f);
    m_buttonMenu->addChild(infoItem);
    infoItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 203.0f, centerY + 128.0f)));
    
    // Refresh button
    auto refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    refreshSprite->setScale(0.6f);
    m_refreshCommentsBtn = CCMenuItemSpriteExtra::create(refreshSprite, this, menu_selector(InfoLayer::onRefreshComments));
    m_refreshCommentsBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_refreshCommentsBtn);
    m_refreshCommentsBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 195.0f, centerY)));
    
    if (commentsDisabled) {
        m_refreshCommentsBtn->setVisible(false);
        m_refreshCommentsBtn->setEnabled(false);
    }
    
    return true;
}

int InfoLayer::getID() {
    if (m_score) return m_score->m_userID;
    if (m_levelList) return -m_levelList->m_listID;
    if (m_level) return m_level->m_levelID.random - m_level->m_levelID.seed;
    return 0;
}

int InfoLayer::getRealID() {
    if (m_score) return m_score->m_userID;
    if (m_levelList) return m_levelList->m_listID;
    if (m_level) return m_level->m_levelID.random - m_level->m_levelID.seed;
    return 0;
}

int InfoLayer::getAccountID() {
    if (m_levelList) return m_levelList->m_accountID;
    if (m_level) return m_level->m_accountID.random - m_level->m_accountID.seed;
    return 0;
}

bool InfoLayer::isCorrect(const char* key) {
    return m_commentKey == key;
}

void InfoLayer::loadPage(int page, bool noSetup) {
    m_commentsBtn->setVisible(false);
    
    bool sortByLikes = GameManager::sharedState()->getGameVariable("0069");
    auto key = GameLevelManager::sharedState()->getCommentKey(getID(), page, sortByLikes, m_mode);
    m_commentKey = key;
    
    m_loadingCircle->setVisible(true);
    m_noComments->setVisible(false);
    
    m_leftArrow->setVisible(page > 0);
    
    auto stored = GameLevelManager::sharedState()->getStoredLevelComments(key.c_str());
    if (stored) {
        m_rightArrow->setVisible(true);
    } else {
        m_rightArrow->setVisible(false);
    }
    
    auto storedLevels = GameLevelManager::sharedState()->getStoredOnlineLevels(key.c_str());
    if (storedLevels) {
        auto info = GameLevelManager::sharedState()->getPageInfo(key.c_str());
        setupPageInfo(info, key.c_str());
        loadCommentsFinished(storedLevels, key.c_str());
    } else {
        if (!noSetup) {
            setupCommentsBrowser(nullptr);
        }
        GameLevelManager::sharedState()->m_levelCommentDelegate = this;
        
        if (GameManager::sharedState()->m_playerUserID.random == GameManager::sharedState()->m_playerUserID.seed) {
            m_canUpdateUserScore = true;
            GameLevelManager::sharedState()->updateUserScore();
        } else {
            GameLevelManager::sharedState()->getLevelComments(getID(), page, m_itemCount, sortByLikes, m_mode);
        }
        m_pageLabel->setString(" ");
    }
    
    m_page = page;
}

void InfoLayer::setupCommentsBrowser(cocos2d::CCArray* comments) {
    if (m_list) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto dispatcher = director->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_list);
        m_list->removeMeAndCleanup();
        m_list = nullptr;
    }
    
    bool extendedMode = GameManager::sharedState()->getGameVariable("0089");
    bool isUserComments = m_score != nullptr;
    
    float listHeight = 140.0f;
    int listStyle = 0;
    
    if (!m_levelList) {
        if (extendedMode || isUserComments) {
            listHeight = 234.0f;
            listStyle = 1;
        }
    }
    
    CustomListView* listView = nullptr;
    if (comments) {
        if (m_levelList) {
            auto account = GJAccountManager::sharedState();
            int currentUser = account->m_accountID - account->m_unkInt1;
            for (int i = 0; i < comments->count(); ++i) {
                auto comment = comments->objectAtIndex(i);
                comment->m_isOwn = (m_levelList->m_accountID == currentUser && currentUser > 0);
            }
        }
        
        listView = CustomListView::create(comments, BoomListType::Comment, listHeight, 340.0f);
    }
    
    auto listLayer = GJCommentListLayer::create(listView, "Comments", cocos2d::ccc4(0, 0, 0, 255), 340.0f, listHeight, 0);
    m_list = listLayer;
    m_mainLayer->addChild(listLayer);
    
    float yOffset = (extendedMode || isUserComments) ? -4.0f : -53.0f;
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    listLayer->setPosition(cocos2d::CCPoint((winSize.width - 340.0f) * 0.5f, yOffset + (winSize.height - listHeight) * 0.5f));
}

void InfoLayer::loadCommentsFinished(cocos2d::CCArray* comments, const char* key) {
    if (isCorrect(key)) {
        setupCommentsBrowser(comments);
        m_loadingCircle->setVisible(false);
        m_noComments->setVisible(false);
    }
}

void InfoLayer::loadCommentsFailed(const char* key) {
    if (isCorrect(key)) {
        m_loadingCircle->setVisible(false);
        m_noComments->setVisible(true);
    }
}

void InfoLayer::setupPageInfo(const std::string& info, const char* key) {
    if (!isCorrect(key)) return;
    
    auto parts = splitString(info, ":");
    if (parts.size() > 2) {
        m_itemCount = std::stoi(parts[0]);
        m_pageStartIdx = std::stoi(parts[1]);
        m_pageEndIdx = std::stoi(parts[2]);
        
        m_leftArrow->setVisible(m_pageStartIdx != 0);
        m_rightArrow->setVisible(m_pageStartIdx + m_pageEndIdx < m_itemCount);
        
        updateLevelsLabel();
    }
}

void InfoLayer::updateLevelsLabel() {
    int end = m_pageStartIdx + m_pageEndIdx;
    if (end > m_itemCount) end = m_itemCount;
    
    auto str = cocos2d::CCString::createWithFormat("%i to %i of %i", m_pageStartIdx + 1, end, m_itemCount);
    m_pageLabel->setString(str->getCString());
}

void InfoLayer::updateCommentModeButtons() {
    bool sortByLikes = GameManager::sharedState()->getGameVariable("0069");
    
    auto likeSprite = static_cast<ButtonSprite*>(m_likeBtn->getNormalImage());
    auto timeSprite = static_cast<ButtonSprite*>(m_timeBtn->getNormalImage());
    
    if (sortByLikes) {
        likeSprite->updateBGImage("GJ_button_02.png");
        timeSprite->updateBGImage("GJ_button_01.png");
        m_timeBtn->setEnabled(false);
    } else {
        likeSprite->updateBGImage("GJ_button_01.png");
        timeSprite->updateBGImage("GJ_button_02.png");
        m_likeBtn->setEnabled(false);
    }
}

void InfoLayer::toggleCommentMode(cocos2d::CCObject* sender) {
    bool sortByLikes = sender->getTag() == 0;
    GameManager::sharedState()->setGameVariable("0069", sortByLikes);
    updateCommentModeButtons();
    loadPage(0, false);
}

void InfoLayer::toggleExtendedMode(cocos2d::CCObject* sender) {
    GameManager::sharedState()->toggleGameVariable("0089");
    reloadWindow();
}

void InfoLayer::toggleSmallCommentMode(cocos2d::CCObject* sender) {
    GameManager::sharedState()->toggleGameVariable("0088");
    GameLevelManager::sharedState()->resetCommentTimersForLevelID(getID(), m_mode);
    reloadWindow();
}

void InfoLayer::reloadWindow() {
    if (m_level) m_level->retain();
    if (m_score) m_score->retain();
    if (m_levelList) m_levelList->retain();
    
    auto parent = getParent();
    int zOrder = getZOrder();
    
    bool gameVar = GameManager::sharedState()->getGameVariable("0090");
    GameManager::sharedState()->setGameVariable("0090", true);
    
    if (m_list) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto dispatcher = director->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_list);
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    auto newLayer = InfoLayer::create(m_level, m_score, m_levelList);
    GameManager::sharedState()->setGameVariable("0090", gameVar);
    
    parent->addChild(newLayer, zOrder);
    
    if (m_level) m_level->release();
    if (m_score) m_score->release();
    if (m_levelList) m_levelList->release();
}

void InfoLayer::setupLevelInfo() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    int version = m_levelList ? m_levelList->m_listVersion : m_level->m_levelVersion;
    int id = m_levelList ? m_levelList->m_listID : (m_level->m_levelID.random - m_level->m_levelID.seed);
    
    auto versionLabel = cocos2d::CCLabelBMFont::create(cocos2d::CCString::createWithFormat("Version: %i", version)->getCString(), "bigFont.fnt");
    versionLabel->setScale(0.6f);
    m_mainLayer->addChild(versionLabel, 2);
    
    auto idMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(idMenu, 2);
    
    auto idLabel = cocos2d::CCLabelBMFont::create(
        id ? cocos2d::CCString::createWithFormat("ID: %i", id)->getCString() : "",
        "bigFont.fnt"
    );
    idLabel->setScale(0.6f);
    
    auto idItem = CCMenuItemSpriteExtra::create(idLabel, this, menu_selector(InfoLayer::onCopyLevelID));
    idMenu->addChild(idItem);
    
    cocos2d::CCPoint basePos(winSize.width * 0.5f, winSize.height * 0.5f - 131.0f);
    versionLabel->setPosition(basePos + cocos2d::CCPoint(-50.0f, 0.0f));
    idMenu->setPosition(basePos + cocos2d::CCPoint(50.0f, 0.0f));
}

void InfoLayer::onClose(cocos2d::CCObject* sender) {
    if (m_list) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto dispatcher = director->getTouchDispatcher();
        dispatcher->unregisterForcePrio(m_list);
        m_list->removeMeAndCleanup();
        m_list = nullptr;
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void InfoLayer::keyBackClicked() {
    onClose(nullptr);
}

void InfoLayer::onComment(cocos2d::CCObject* sender) {
    auto account = GJAccountManager::sharedState();
    if (account->m_accountID - account->m_unkInt1 <= 0) {
        auto alert = FLAlertLayer::create(
            nullptr,
            "Account Needed",
            "You need an <cl>account</c> to post <cg>comments</c>. You can create one for free from settings in the main menu.",
            "OK",
            nullptr,
            380.0f
        );
        alert->show();
        return;
    }
    
    if (!GameManager::sharedState()->getGameVariable("0012")) {
        GameManager::sharedState()->setGameVariable("0012", true);
        auto alert = FLAlertLayer::create(
            this,
            "Comment Rules",
            "1. Do <cr>not</c> spam.\n"
            "2. Do <cr>not</c> harass other players.\n"
            "3. Do <cr>not</c> post inappropriate or controversial content.\n"
            "4. Do <cr>not</c> try to bypass these rules.\n"
            "<cy>These rules and their enforcement are entirely at Elder Moderators' discretion.</c>",
            "OK",
            nullptr,
            380.0f
        );
        alert->setTag(3);
        alert->show();
        return;
    }
    
    int timeLeft = GameLevelManager::sharedState()->getTimeLeft(GameLevelManager::sharedState()->getPostCommentKey(getID()), 15.0f);
    if (timeLeft > 0 && timeLeft <= 15) {
        auto alert = FLAlertLayer::create(
            nullptr,
            "Too fast",
            cocos2d::CCString::createWithFormat("Please wait %i seconds before posting a new comment!", timeLeft)->getCString(),
            "OK",
            nullptr,
            380.0f
        );
        alert->show();
        return;
    }
    
    auto commentLayer = ShareCommentLayer::create("Add Comment", 100, CommentType::Level, getID(), "");
    if (m_level) {
        commentLayer->m_percent = m_level->m_orbCompletion.random - m_level->m_orbCompletion.seed;
    }
    commentLayer->show();
}

void InfoLayer::confirmReport(cocos2d::CCObject* sender) {
    if (GameLevelManager::sharedState()->hasReportedLevel(getID())) {
        return;
    }
    
    const char* title = (m_mode == 1) ? "Report List" : "Report Level";
    const char* desc = (m_mode == 1)
        ? "Do you want to <cr>report</c> this list for breaking the <cg>upload guidelines</c> found in the editor? Valid reasons include hateful, abusive, or otherwise inappropriate content, as well as hacks, cheats, or exploits."
        : "Do you want to <cr>report</c> this level for breaking the <cg>upload guidelines</c> found in the editor? Valid reasons include hateful, abusive, or otherwise inappropriate content, as well as hacks, cheats, or exploits.";
    
    auto alert = FLAlertLayer::create(this, title, desc, "Back", "Report", 380.0f);
    alert->setTag(2);
    alert->show();
}

void InfoLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2) {
        int tag = layer->getTag();
        if (tag == 2) {
            GameLevelManager::sharedState()->reportLevel(getID());
            m_reportBtn->getNormalImage()->setColor(cocos2d::ccc3(75, 75, 75));
            m_reportBtn->setEnabled(false);
        } else if (tag == 3) {
            onComment(nullptr);
        }
    }
}

void InfoLayer::commentUploadFailed(int parentID, CommentError errorType) {
    if (!m_score && getID() == parentID && errorType == CommentError::None) {
        auto alert = FLAlertLayer::create(nullptr, "Failed", "Comment upload failed. Please try again later.", "OK", nullptr, 380.0f);
        alert->show();
    }
}

void InfoLayer::commentUploadFinished(int parentID) {
    if (!m_score && getID() == parentID) {
        GameManager::sharedState()->setGameVariable("0069", false);
        updateCommentModeButtons();
        loadPage(0, false);
    }
}

void InfoLayer::onCopyLevelID(cocos2d::CCObject* sender) {
    int id = getRealID();
    PlatformToolbox::copyToClipboard(std::to_string(abs(id)));
}

void InfoLayer::onGetComments(cocos2d::CCObject* sender) {
    loadPage(0, false);
}

void InfoLayer::onLevelInfo(cocos2d::CCObject* sender) {
    if (m_levelList) {
        m_levelList->showListInfo();
        return;
    }
    
    auto level = GameLevelManager::sharedState()->getSavedLevel(m_level);
    if (!level) level = m_level;
    
    std::string infoText;
    
    if (GameManager::sharedState()->getGameVariable("0089") || m_score) {
        std::string desc = level->getUnpackedLevelDescription();
        if (desc.empty()) desc = "(No description provided)";
        infoText += desc + "\n\n";
    }
    
    std::string uploadDate = level->m_uploadDate.empty() ? "NA" : level->m_uploadDate;
    std::string updateDate = level->m_updateDate.empty() ? "NA" : level->m_updateDate;
    infoText += cocos2d::CCString::createWithFormat("Uploaded: <cy>%s ago</c>\nUpdated: <cy>%s ago</c>", uploadDate.c_str(), updateDate.c_str())->getCString();
    
    if (level->m_starsRequested > 0) {
        const char* starType = level->isPlatformer() ? "Moons" : "Stars";
        infoText += std::string("\n") + starType + "\n" + starType + " Requested: <cy>" + std::to_string(level->m_starsRequested) + "</c>";
    }
    
    int originalId = level->m_originalLevel.random - level->m_originalLevel.seed;
    if (originalId != 0) {
        infoText += std::string("\nOriginal: <cy>") + std::to_string(originalId) + "</c>";
    }
    
    auto alert = FLAlertLayer::create(nullptr, "Level Info", infoText.c_str(), "OK", nullptr, 380.0f);
    alert->show();
}

void InfoLayer::onMore(cocos2d::CCObject* sender) {
    int accountId = getAccountID();
    if (accountId > 0) {
        auto profile = ProfilePage::create(accountId, 0);
        profile->show();
    }
}

void InfoLayer::onNextPage(cocos2d::CCObject* sender) {
    loadPage(m_page + 1, false);
}

void InfoLayer::onPrevPage(cocos2d::CCObject* sender) {
    loadPage(m_page - 1, false);
}

void InfoLayer::onOriginal(cocos2d::CCObject* sender) {
    int originalId = m_level->m_originalLevel.random - m_level->m_originalLevel.seed;
    auto search = GJSearchObject::create(SearchType::Search);
    auto scene = LevelBrowserLayer::scene(search);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    m_buttonMenu->setEnabled(false);
}

void InfoLayer::onRefreshComments(cocos2d::CCObject* sender) {
    if (GameLevelManager::sharedState()->getTimeLeft("upd_comments", 5.0f) > 5) {
        GameLevelManager::sharedState()->makeTimeStamp("upd_comments");
        GameLevelManager::sharedState()->resetCommentTimersForLevelID(getID(), m_mode);
        loadPage(0, true);
    }
}

void InfoLayer::onSimilar(cocos2d::CCObject* sender) {
    int levelId = m_level->m_levelID.random - m_level->m_levelID.seed;
    auto search = GJSearchObject::create(SearchType::Similar);
    auto scene = LevelBrowserLayer::scene(search);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    m_buttonMenu->setEnabled(false);
}

void InfoLayer::registerWithTouchDispatcher() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void InfoLayer::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        m_mainLayer->setScale(0.1f);
        auto scaleTo = cocos2d::CCScaleTo::create(0.3f, 1.0f);
        auto ease = cocos2d::CCEaseElasticOut::create(scaleTo, 1.6f);
        m_mainLayer->runAction(ease);
        
        auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
        if (!m_scene) m_scene = scene;
        if (m_ZOrder == 0) m_ZOrder = 105;
        scene->addChild(this, m_ZOrder);
        
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void InfoLayer::updateUserScoreFinished() {
    if (m_canUpdateUserScore) {
        m_canUpdateUserScore = false;
        GameLevelManager::sharedState()->getLevelComments(
            getID(), m_page, m_itemCount,
            GameManager::sharedState()->getGameVariable("0069"),
            m_mode
        );
    }
}

InfoLayer::~InfoLayer() {
    if (m_level) m_level->release();
    if (m_score) m_score->release();
    if (m_levelList) m_levelList->release();
    if (m_loadingCircle) m_loadingCircle->release();
}