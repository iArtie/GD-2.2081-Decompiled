#include "Pages/FriendsProfilePage.h"
#include "GameLevelManager.h"
#include "Managers/GJAccountManager.h"
#include "Models/GJUserScore.h"
#include "Layers/GJCommentListLayer.h"
#include "UI/LoadingCircle.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/CustomListView.h"
#include "Pages/ProfilePage.h"
#include "Managers/GameManager.h"

FriendsProfilePage* FriendsProfilePage::create(UserListType type) {
    FriendsProfilePage* ret = new FriendsProfilePage();
    if (ret && ret->init(type)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void FriendsProfilePage::forceReloadList(UserListType type) {
    if (m_type == type) {
        m_circle->setVisible(true);
        m_refreshBtn->setVisible(false);
        m_noInternet->setVisible(false);
        setupUsersBrowser(nullptr, m_type);
        GameLevelManager::sharedState()->getUserList(m_type);
    }
}

void FriendsProfilePage::getUserListFailed(UserListType type, GJErrorCode errorType) {
    if (m_type == type) {
        m_circle->setVisible(false);
        m_refreshBtn->setVisible(true);
        m_noInternet->setString((errorType == NotFound) ? "Nothing found" : "Something went wrong");
        m_noInternet->setVisible(true);
    }
}

void FriendsProfilePage::getUserListFinished(cocos2d::CCArray* scores, UserListType type) {
    if (m_type == type) {
        if (m_users) {
            m_users->release();
        }
        m_users = scores;
        m_users->retain();
        
        setupUsersBrowser(m_users, m_type);
        m_circle->setVisible(false);
        m_refreshBtn->setVisible(true);
        m_noInternet->setVisible(false);
        
        // Reset new friend count if this is the friends list
        if (type == Friends) {
            GameLevelManager* glm = GameLevelManager::sharedState();
            GJAccountManager* acctMgr = GJAccountManager::sharedState();
            GJUserScore* userInfo = glm->userInfoForAccountID(acctMgr->m_accountID - acctMgr->m_unkInt1);
            if (userInfo) {
                userInfo->m_newFriendCount = 0;
            }
        }
    }
}

bool FriendsProfilePage::init(UserListType type) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }
    
    m_type = type;
    m_noElasticity = true;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440, 290));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Title
    float yOffset = (m_type == Blocked) ? -20.0f : -17.0f;
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f + yOffset));
    titleLabel->limitLabelWidth(185.0f, 0.8f, 0.0f);
    
    // Subtitle (only for friends list)
    if (m_type != Blocked) {
        cocos2d::CCLabelBMFont* subtitleLabel = cocos2d::CCLabelBMFont::create();
        m_mainLayer->addChild(subtitleLabel, 10);
        cocos2d::CCPoint titlePos = titleLabel->getPosition();
        subtitleLabel->setPosition(cocos2d::CCPoint(titlePos.x, titlePos.y - 21.0f));
        subtitleLabel->setScale(0.6f);
    }
    
    // Total friends label
    m_totalFriends = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(m_totalFriends, 10);
    m_totalFriends->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 145.0f + 25.0f));
    m_totalFriends->setScale(0.7f);
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, this, menu_selector(FriendsProfilePage::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Blocked button (only for friends list)
    if (m_type == Friends) {
        cocos2d::CCSprite* blockedSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_blocked_001.png");
        CCMenuItemSpriteExtra* blockedBtn = CCMenuItemSpriteExtra::create(blockedSprite, nullptr, this, menu_selector(FriendsProfilePage::onBlocked));
        m_buttonMenu->addChild(blockedBtn);
        blockedBtn->setPosition(m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f + 145.0f - 10.0f)));
    }
    
    // No internet/error label
    m_noInternet = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(m_noInternet, 10);
    m_noInternet->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    m_noInternet->setScale(0.6f);
    m_noInternet->setVisible(false);
    
    // Loading circle
    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = m_mainLayer;
    m_circle->m_fade = false;
    m_circle->retain();
    m_circle->show();
    m_circle->setVisible(false);
    m_circle->m_sprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Refresh button
    cocos2d::CCSprite* refreshSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    m_refreshBtn = CCMenuItemSpriteExtra::create(refreshSprite, nullptr, this, menu_selector(FriendsProfilePage::onUpdate));
    m_refreshBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_refreshBtn);
    m_refreshBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f - 145.0f + 10.0f + 1.0f)));
    m_refreshBtn->setVisible(false);
    
    GameLevelManager::sharedState()->m_userListDelegate = this;
    
    // Check if we have cached data
    cocos2d::CCArray* cachedUsers = GameLevelManager::sharedState()->getStoredUserList(m_type);
    if (cachedUsers) {
        getUserListFinished(cachedUsers, m_type);
    } else {
        m_circle->setVisible(true);
        setupUsersBrowser(nullptr, m_type);
        GameLevelManager::sharedState()->getUserList(m_type);
    }
    
    return true;
}

void FriendsProfilePage::keyBackClicked() {
    onClose(nullptr);
}

void FriendsProfilePage::onBlocked(cocos2d::CCObject* sender) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
    }
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    
    FriendsProfilePage* page = FriendsProfilePage::create(Blocked);
    page->show();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void FriendsProfilePage::onClose(cocos2d::CCObject* sender) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
    }
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    
    if (m_type == Blocked) {
        FriendsProfilePage* page = FriendsProfilePage::create(Friends);
        page->show();
    } else {
        GJAccountManager* acctMgr = GJAccountManager::sharedState();
        ProfilePage* profile = ProfilePage::create(acctMgr->m_accountID - acctMgr->m_unkInt1, true);
        profile->m_noElasticity = true;
        profile->show();
    }
    
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void FriendsProfilePage::onUpdate(cocos2d::CCObject* sender) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->getTimeLeft("upd_friends", 5.0f) > 5) {
        glm->makeTimeStamp("upd_friends");
        glm->invalidateUserList(m_type, true);
    }
}

void FriendsProfilePage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void FriendsProfilePage::setupUsersBrowser(cocos2d::CCArray* users, UserListType type) {
    if (m_listLayer) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_listLayer);
        m_listLayer->removeMeAndCleanup();
        m_listLayer = nullptr;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    CustomListView* listView = users ? CustomListView::create(users, BoomListType::User, 196.0f, 340.0f) : nullptr;
    
    m_listLayer = GJCommentListLayer::create(listView, nullptr,
        cocos2d::ccColor4B{0, 0, 0, 0}, 340.0f, 196.0f, false);
    m_mainLayer->addChild(m_listLayer);
    m_listLayer->setPosition(cocos2d::CCPoint((winSize.width - 340.0f) * 0.5f, (winSize.height - 196.0f) * 0.5f - 10.0f + 5.0f));
    
    // Update total friends label
    if (users && type == Friends) {
        int count = users->count();
        if (count > 0) {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("Total friends: %i", count);
            m_totalFriends->setString(str->getCString());
            
            // Report achievements
            GameManager* gm = GameManager::sharedState();
            gm->reportAchievementWithID("geometry.ach.friends01", 100, false);
            if (count > 9) {
                gm->reportAchievementWithID("geometry.ach.friends02", 100, false);
                if (count > 24) {
                    gm->reportAchievementWithID("geometry.ach.friends03", 100, false);
                    if (count > 49) {
                        gm->reportAchievementWithID("geometry.ach.friends04", 100, false);
                    }
                }
            }
        } else {
            m_totalFriends->setString(" ");
        }
    } else {
        m_totalFriends->setString(" ");
    }
}

void FriendsProfilePage::userListChanged(cocos2d::CCArray* scores, UserListType type) {
    getUserListFinished(scores, type);
}

FriendsProfilePage::~FriendsProfilePage() {
    if (m_circle) {
        m_circle->release();
        m_circle = nullptr;
    }
    if (m_users) {
        m_users->release();
        m_users = nullptr;
    }
}