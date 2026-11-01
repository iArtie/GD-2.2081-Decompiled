#include "Pages/ProfilePage.h"
#include "Utils/GameToolbox.h"
#include "Managers/GameManager.h"
#include "Managers/GJAccountManager.h"
#include "GameLevelManager.h"
#include "Models/GJUserScore.h"
#include "Layers/GJCommentListLayer.h"
#include "UI/LoadingCircle.h"
#include "Popups/UploadActionPopup.h"
#include "Sprites/SimplePlayer.h"
#include "Utils/PlatformToolbox.h"
#include "Layers/InfoLayer.h"
#include "Layers/ShareCommentLayer.h"
#include "UI/TextArea.h"
#include "Popups/TextAlertPopup.h"
#include "Pages/FriendsProfilePage.h"
#include "Pages/MessagesProfilePage.h"
#include "Pages/FRequestProfilePage.h"
#include "Popups/FriendRequestPopup.h"
#include "Popups/GJWriteMessagePopup.h"
#include "Layers/GJAccountSettingsLayer.h"
#include "Models/GJFriendRequest.h"
#include "Layers/LevelBrowserLayer.h"
#include "Popups/StarInfoPopup.h"
#include "Popups/DemonInfoPopup.h"
#include "UI/CustomListView.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Models/GJSearchObject.h"
#include "Layers/PlayLayer.h"

#include <cmath>

void ProfilePage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (!btn2)
        return;

    if (layer->getTag() == 1)
    {
        GameLevelManager::sharedState()->m_uploadActionDelegate = this;

        if (!GameLevelManager::sharedState()->deleteSentFriendRequest(m_accountID))
            return;

        m_popupDelegate = UploadActionPopup::create(this, "Removing Friend Request...");
        m_popupDelegate->show();
        return;
    }

    if (layer->getTag() == 2)
    {
        GameLevelManager::sharedState()->m_uploadActionDelegate = this;

        if (!GameLevelManager::sharedState()->removeFriend(m_accountID))
            return;

        m_popupDelegate = UploadActionPopup::create(this, "Removing Friend...");
        m_popupDelegate->show();
        return;
    }

    if (layer->getTag() == 3)
    {
        GameLevelManager::sharedState()->m_uploadActionDelegate = this;
        blockUser();
    }
}

void ProfilePage::blockUser()
{
    if (GameLevelManager::sharedState()->blockUser(m_score->m_accountID))
    {
        m_popupDelegate = UploadActionPopup::create(this, "Blocking user...");
        m_popupDelegate->show();
    }
}

void ProfilePage::commentDeleteFailed(int id, int parentID)
{
    GJAccountManager* accountManager = GJAccountManager::sharedState();

    if (id != accountManager->m_accountID - accountManager->m_unkInt1)
        return;

    FLAlertLayer::create(nullptr, "Failed", "Comment delete failed. Please try again later.")->show();

    cocos2d::CCArray* storedLevels = GameLevelManager::sharedState()->getStoredOnlineLevels(m_profileKey.c_str());

    if (!storedLevels)
        return;

    for (unsigned int i = 0; i < storedLevels->count(); i++)
    {
        cocos2d::CCObject* obj = storedLevels->objectAtIndex(i);

        if (obj->getTag() == parentID)
        {
            // LOBYTE(obj[6].m_uIndexInArray) = 0; → marcar como no-placeholder
            loadPage(m_page);
            return;
        }
    }
}

void ProfilePage::commentUploadFailed(int parentID, CommentError errorType)
{
    GJAccountManager* accountManager = GJAccountManager::sharedState();

    if (parentID != accountManager->m_accountID - accountManager->m_unkInt1)
        return;

    FLAlertLayer::create(nullptr, "Failed", "Comment upload failed. Please try again later.")->show();
}

void ProfilePage::commentUploadFinished(int parentID)
{
    GJAccountManager* accountManager = GJAccountManager::sharedState();

    if (parentID == accountManager->m_accountID - accountManager->m_unkInt1)
    {
        m_itemCount = 0;
        loadPage(0);
    }
}

ProfilePage* ProfilePage::create(int accountID, bool ownProfile)
{
    ProfilePage* returnVal = new ProfilePage();

    if (returnVal->init(accountID, ownProfile))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

void ProfilePage::getUserInfoFailed(int id)
{
    if (m_accountID != id)
        return;

    m_circle->setVisible(false);
    m_refreshBtn->setVisible(true);
    m_somethingWentWrong->setVisible(true);
}

void ProfilePage::getUserInfoFinished(GJUserScore* score)
{
    if (score->m_accountID != m_accountID)
        return;

    bool gameVariable = GameManager::sharedState()->getGameVariable("0076");

    loadPageFromUserInfo(score);

    if (gameVariable || score->m_friendReqStatus == 2)
        m_circle->setVisible(false);
    else
        loadPage(0);

    m_refreshBtn->setVisible(true);
    m_somethingWentWrong->setVisible(false);
}

bool ProfilePage::init(int accountID, bool ownProfile)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_accountID = accountID;

    GJAccountManager* accountManager = GJAccountManager::sharedState();

    if (ownProfile && accountID == accountManager->m_accountID - accountManager->m_unkInt1)
    {
        m_ownProfile = true;

        if (!GameManager::sharedState()->m_scoreValid)
        {
            GameLevelManager* glm = GameLevelManager::sharedState();
            glm->resetStoredUserInfo(accountManager->m_accountID - accountManager->m_unkInt1);
            glm->invalidateMessages(1, 0);
            glm->invalidateMessages(0, 0);
            glm->invalidateRequests(1, 0);
            glm->invalidateRequests(0, 0);
            glm->invalidateUserList(UserListType::Default, 0);
            glm->invalidateUserList(UserListType::Blocked, 0);
        }
    }
    else
    {
        m_ownProfile = false;
    }

    m_arrayWithUsernameLabel = cocos2d::CCArray::create();
    m_arrayWithUsernameLabel->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    if (accountID == accountManager->m_accountID - accountManager->m_unkInt1)
    {
        GameManager* gm = GameManager::sharedState();
        int userID = gm->m_playerUserID.random - gm->m_playerUserID.seed;
        GameLevelManager::sharedState()->storeUserName(userID, accountID, accountManager->m_username);
    }

    GameLevelManager::sharedState()->tryGetUsername(accountID);

    m_usernameLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_usernameLabel, 20);
    m_usernameLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 20.0f));

    if (isOnWatchlist(m_accountID))
        m_usernameLabel->setColor(cocos2d::ccc3(255, 200, 0));

    m_usernameLabel->limitLabelWidth(160.0f, 0.8f, 0.0f);
    m_arrayWithUsernameLabel->addObject(m_usernameLabel);

    cocos2d::CCSprite* floorLine = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
    floorLine->setScaleX(0.8f);
    floorLine->setScaleY(1.0f);
    floorLine->setOpacity(100);
    m_mainLayer->addChild(floorLine, 0);

    cocos2d::CCPoint linePos = m_usernameLabel->getPosition() + cocos2d::CCPoint(0.0f, -21.0f);
    floorLine->setPosition(linePos);
    floorLine->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

    m_arrayWithUsernameLabel->addObject(floorLine);

    cocos2d::CCPoint listPos(winSize.width * 0.5f, winSize.height * 0.5f + 40.0f);
    cocos2d::ccColor4B listColor = cocos2d::ccc4(0, 0, 0, 0);

    GJCommentListLayer* tempList = GJCommentListLayer::create(nullptr, " ", listColor, 340.0f, 45.0f, false);

    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(tempList);

    m_mainLayer->addChild(tempList, 1);
    tempList->setPosition(listPos + cocos2d::CCPoint(-170.0f, -22.5f));
    m_arrayWithUsernameLabel->addObject(tempList);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, menu_selector(ProfilePage::onClose));
    closeBtn->setSizeMult(1.6f);

    m_buttonMenu = cocos2d::CCMenu::create(closeBtn);
    m_buttonMenu->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f - 220.0f + 10.0f,
        winSize.height * 0.5f + 145.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);

    setupComments();

    m_buttons = cocos2d::CCArray::create();
    m_buttons->retain();

    m_somethingWentWrong = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_somethingWentWrong, 10);
    m_somethingWentWrong->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f - 53.0f));
    m_somethingWentWrong->setScale(0.6f);
    m_somethingWentWrong->setVisible(false);

    if (accountManager->m_accountID - accountManager->m_unkInt1 != m_accountID)
    {
        GameLevelManager* glm = GameLevelManager::sharedState();
        bool isFollowingUser = glm->isFollowingUser(m_accountID);

        cocos2d::CCPoint followPos(winSize.width * 0.5f - 158.0f, winSize.height * 0.5f - 123.0f);
        const char* followSprite = isFollowingUser ? "gj_heartOn_001.png" : "gj_heartOff_001.png";

        cocos2d::CCSprite* heartSprite = cocos2d::CCSprite::createWithSpriteFrameName(followSprite);
        m_followBtn = CCMenuItemSpriteExtra::create(heartSprite, nullptr, menu_selector(ProfilePage::onFollow));
        m_followBtn->setSizeMult(1.1f);
        m_buttonMenu->addChild(m_followBtn);

        cocos2d::CCPoint nodePos = m_buttonMenu->convertToNodeSpace(followPos);
        m_followBtn->setPosition(nodePos);

        if (!isFollowingUser)
        {
            cocos2d::CCSprite* followTxt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_followTxt_001.png");
            m_mainLayer->addChild(followTxt, 2);
            followTxt->setPosition(followPos + cocos2d::CCPoint(48.0f, -2.0f));
        }
    }

    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = m_mainLayer;
    m_circle->m_fade = false;
    m_circle->retain();
    m_circle->show();
    m_circle->setVisible(false);

    m_circle->m_sprite->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f - 53.0f));

    cocos2d::CCSprite* updateSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    m_refreshBtn = CCMenuItemSpriteExtra::create(updateSprite, nullptr, menu_selector(ProfilePage::onUpdate));
    m_refreshBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_refreshBtn);

    cocos2d::CCPoint refreshPos = cocos2d::CCPoint(
        winSize.width * 0.5f - 220.0f + 10.0f,
        winSize.height * 0.5f - 145.0f + 10.0f + 1.0f);
    m_refreshBtn->setPosition(m_buttonMenu->convertToNodeSpace(refreshPos));
    m_refreshBtn->setVisible(false);

    GameLevelManager::sharedState()->m_userInfoDelegate = this;

    if (!m_ownProfile || GameManager::sharedState()->m_scoreValid)
    {
        GJUserScore* score = GameLevelManager::sharedState()->userInfoForAccountID(m_accountID);

        if (score)
        {
            if (score->m_friendReqStatus == 2)
                setupCommentsBrowser(nullptr);
            else
                getUserInfoFinished(score);
        }
        else
        {
            m_circle->setVisible(true);
            m_refreshBtn->setVisible(false);
            setupCommentsBrowser(nullptr);
            GameLevelManager::sharedState()->getGJUserInfo(m_accountID);
        }
    }
    else
    {
        setupCommentsBrowser(nullptr);
        GameLevelManager::sharedState()->m_leaderboardManagerDelegate = this;
        m_circle->setVisible(true);
        m_refreshBtn->setVisible(false);
        GameLevelManager::sharedState()->updateUserScore();
    }

    return true;
}

bool ProfilePage::isCorrect(const char* key)
{
    return m_profileKey == key;
}

bool ProfilePage::isOnWatchlist(int id)
{
    return false;
}

void ProfilePage::keyBackClicked()
{
    onClose(nullptr);
}

void ProfilePage::loadCommentsFailed(const char* key)
{
    if (!isCorrect(key))
        return;

    m_circle->setVisible(false);
    m_refreshBtn->setVisible(true);
    m_somethingWentWrong->setVisible(true);
}

void ProfilePage::loadCommentsFinished(cocos2d::CCArray* comments, const char* key)
{
    if (!isCorrect(key))
        return;

    setupCommentsBrowser(comments);
    m_circle->setVisible(false);
    m_refreshBtn->setVisible(true);
    m_somethingWentWrong->setVisible(false);
}

void ProfilePage::loadPage(int page)
{
    std::string accountCommentKey = GameLevelManager::sharedState()->getAccountCommentKey(m_accountID, page);
    m_profileKey = accountCommentKey;

    m_circle->setVisible(true);
    m_refreshBtn->setVisible(false);

    if (page > 0)
        m_leftArrow->setVisible(true);
    else
        m_leftArrow->setVisible(false);

    cocos2d::CCArray* storedLevels = GameLevelManager::sharedState()->getStoredOnlineLevels(m_profileKey.c_str());

    if (storedLevels)
    {
        m_rightArrow->setVisible(true);
        std::string pageInfo = GameLevelManager::sharedState()->getPageInfo(m_profileKey.c_str());
        setupPageInfo(pageInfo, m_profileKey.c_str());
        loadCommentsFinished(storedLevels, m_profileKey.c_str());
    }
    else
    {
        m_rightArrow->setVisible(false);
        setupCommentsBrowser(nullptr);
        GameLevelManager::sharedState()->m_levelCommentDelegate = this;
        GameLevelManager::sharedState()->getAccountComments(m_accountID, page, m_itemCount);
    }

    m_page = page;
}

void ProfilePage::loadPageFromUserInfo(GJUserScore* score)
{
    m_mainLayer->setScaleX(1.0f);
    m_mainLayer->setScaleY(1.0f);

    if (m_score)
    {
        m_score->release();
        m_score = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    if (m_buttons)
    {
        for (unsigned int i = 0; i < m_buttons->count(); i++)
        {
            cocos2d::CCObject* obj = m_buttons->objectAtIndex(i);
            static_cast<cocos2d::CCNode*>(obj)->removeFromParentAndCleanup(true);
        }
        m_buttons->removeAllObjects();
    }

    if (!score)
        return;

    m_score = score;
    m_score->retain();

    if (m_score->m_friendReqStatus == 2)
        return;

    int socialMediaCount = 0;

    if (!m_score->m_youtubeURL.empty()) socialMediaCount++;
    if (!m_score->m_twitterURL.empty()) socialMediaCount++;
    if (!m_score->m_twitchURL.empty()) socialMediaCount++;
    if (!m_score->m_instagramURL.empty()) socialMediaCount++;
    if (!m_score->m_tiktokURL.empty()) socialMediaCount++;

    float statY = winSize.height * 0.5f + 85.0f;

    cocos2d::CCNode* statNode = cocos2d::CCNode::create();
    statNode->setContentSize(cocos2d::CCSize(440.0f, 40.0f));
    m_mainLayer->addChild(statNode);

    cocos2d::CCArray* statButtons = cocos2d::CCArray::create();

    bool hasCreatorPoints = m_score->m_creatorPoints > 0;
    int statCount = hasCreatorPoints ? 6 : 5;
    float labelMaxWidth = hasCreatorPoints ? 50.0f : 60.0f;

    float currentX = 0.0f;

    for (int i = 0; i < statCount; i++)
    {
        std::string spriteName;
        int tag = 0;
        int value = 0;

        switch (i)
        {
            case 0: spriteName = "GJ_starsIcon_001.png"; tag = 1; value = m_score->m_stars; break;
            case 1: spriteName = "GJ_moonsIcon_001.png"; tag = 2; value = m_score->m_moons; break;
            case 2: spriteName = "GJ_coinsIcon_001.png"; tag = 0; value = m_score->m_secretCoins; break;
            case 3: spriteName = "GJ_coinsIcon2_001.png"; tag = 0; value = m_score->m_userCoins; break;
            case 4: spriteName = "GJ_demonIcon_001.png"; tag = 3; value = m_score->m_demons; break;
            case 5: spriteName = "GJ_hammerIcon_001.png"; tag = 0; value = m_score->m_creatorPoints; break;
        }

        std::string valueStr = GameToolbox::pointsToString(value);

        cocos2d::CCLabelBMFont* valueLabel = cocos2d::CCLabelBMFont::create(valueStr.c_str(), "bigFont.fnt");
        statNode->addChild(valueLabel, 2);
        valueLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        valueLabel->setPosition(cocos2d::CCPoint(currentX, 0.0f));
        valueLabel->setScale(0.6f);
        valueLabel->limitLabelWidth(labelMaxWidth, 0.6f, 0.0f);

        m_buttons->addObject(valueLabel);

        cocos2d::CCSprite* iconSprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteName.c_str());
        iconSprite->setScale(0.9f);

        cocos2d::CCPoint iconPos(valueLabel->getPosition().x + valueLabel->getScaledContentSize().width + 14.0f, 0.0f);

        if (tag != 0)
        {
            CCMenuItemSpriteExtra* statBtn = CCMenuItemSpriteExtra::create(iconSprite, nullptr, menu_selector(ProfilePage::onStatInfo));
            statBtn->setTag(tag);
            statBtn->setContentSize(cocos2d::CCSize(30.0f, 30.0f));
            iconSprite->setPosition(cocos2d::CCPoint(15.0f, 15.0f));
            m_buttonMenu->addChild(statBtn);
            statBtn->setPosition(iconPos);

            m_buttons->addObject(statBtn);
            statButtons->addObject(statBtn);
        }
        else
        {
            statNode->addChild(iconSprite, 2);
            iconSprite->setPosition(iconPos);
            m_buttons->addObject(iconSprite);
        }

        currentX = iconPos.x + 20.0f;

        float labelWidth = valueLabel->getContentSize().width * valueLabel->getScale();
        currentX += labelWidth + (i > 0 ? 10.0f : 20.0f);
    }

    if (socialMediaCount <= 1)
    {
        if (currentX > 340.0f)
        {
            statNode->setScale(340.0f / currentX);
            currentX = 340.0f;
        }
    }
    else
    {
        if (currentX > 420.0f)
        {
            statNode->setScale(420.0f / currentX);
            currentX = 420.0f;
        }
    }

    statNode->setPosition(cocos2d::CCPoint(
        -(currentX * 0.5f - winSize.width * 0.5f),
        statY));

    for (unsigned int i = 0; i < statButtons->count(); i++)
    {
        CCMenuItemSpriteExtra* btn = static_cast<CCMenuItemSpriteExtra*>(statButtons->objectAtIndex(i));
        cocos2d::CCPoint worldPos = statNode->convertToWorldSpace(btn->getPosition());
        btn->setPosition(m_buttonMenu->convertToNodeSpace(worldPos));
        btn->setScale(btn->getScale() * statNode->getScale());
    }

    float iconRowY = winSize.height * 0.5f + 40.0f;
    cocos2d::CCPoint iconRowPos(winSize.width * 0.5f, iconRowY);

    cocos2d::ccColor3B color1 = GameManager::sharedState()->colorForIdx(m_score->m_color1);
    cocos2d::ccColor3B color2 = GameManager::sharedState()->colorForIdx(m_score->m_color2);

    float iconSpacing;
    float iconScale;

    if (socialMediaCount <= 3)
    {
        iconSpacing = 22.0f;
        iconScale = 1.0f;
    }
    else if (socialMediaCount == 4)
    {
        iconSpacing = 20.9f;
        iconScale = 0.9f;
    }
    else
    {
        iconSpacing = 19.8f;
        iconScale = 0.8f;
    }

    for (int iconIdx = 0; iconIdx < 8; iconIdx++)
    {
        IconType iconType = IconType::Cube;
        float scale = 0.95f;
        int frame = 0;

        switch (iconIdx)
        {
            case 0: iconType = IconType::Cube; frame = m_score->m_playerCube; break;
            case 1: iconType = IconType::Ship; frame = m_score->m_playerShip; break;
            case 2: iconType = IconType::Ball; frame = m_score->m_playerBall; break;
            case 3: iconType = IconType::UFO; frame = m_score->m_playerUfo; break;
            case 4: iconType = IconType::Wave; frame = m_score->m_playerWave; break;
            case 5: iconType = IconType::Robot; frame = m_score->m_playerRobot; break;
            case 6: iconType = IconType::Spider; frame = m_score->m_playerSpider; break;
            case 7: iconType = IconType::Swing; frame = m_score->m_playerSwing; scale = 0.9f; break;
        }

        SimplePlayer* simplePlayer = SimplePlayer::create(frame);

        float iconX = winSize.width * 0.5f + (iconIdx * 42.6f - 149.1f);
        cocos2d::CCPoint iconPos(iconX, iconRowPos.y);

        simplePlayer->setColor(&color1);
        simplePlayer->setSecondColor(&color2);
        simplePlayer->m_hasGlowOutline = m_score->m_special == 2 || m_score->m_glowEnabled;
        simplePlayer->tryEnableCustomGlowColor(m_score->m_color3);
        simplePlayer->updatePlayerFrame(frame, iconType);
        simplePlayer->updateColors();
        simplePlayer->setScale(scale);

        if (iconType == IconType::Ship)
        {
            CCMenuItemSpriteExtra* shipBtn = CCMenuItemSpriteExtra::create(simplePlayer, nullptr, menu_selector(ProfilePage::toggleShip));
            shipBtn->setTag(1);
            shipBtn->setContentSize(cocos2d::CCSize(30.0f, 30.0f));
            simplePlayer->setPosition(cocos2d::CCPoint(15.0f, 15.0f));
            m_buttonMenu->addChild(shipBtn);
            shipBtn->setPosition(m_buttonMenu->convertToNodeSpace(iconPos));
            m_buttons->addObject(shipBtn);
        }
        else
        {
            m_mainLayer->addChild(simplePlayer, 10);
            simplePlayer->setPosition(iconPos);
            m_buttons->addObject(simplePlayer);
        }
    }

    cocos2d::CCPoint socialStartPos(
        winSize.width * 0.5f + 220.0f - iconSpacing,
        winSize.height * 0.5f + 145.0f - iconSpacing);

    cocos2d::CCPoint leftArrowPos(
        winSize.width * 0.5f - 220.0f + iconSpacing,
        winSize.height * 0.5f + 145.0f - iconSpacing);

    float socialIndex = 0.0f;

    for (int socialIdx = 0; socialIdx < 5; socialIdx++)
    {
        std::string url;
        std::string iconName;

        switch (socialIdx)
        {
            case 0: url = m_score->m_youtubeURL; iconName = "gj_ytIcon_001.png"; break;
            case 1: url = m_score->m_twitterURL; iconName = "gj_twIcon_001.png"; break;
            case 2: url = m_score->m_twitchURL; iconName = "gj_twitchIcon_001.png"; break;
            case 3: url = m_score->m_instagramURL; iconName = "gj_instaIcon_001.png"; break;
            case 4: url = m_score->m_tiktokURL; iconName = "gj_tiktokIcon_001.png"; break;
        }

        if (!url.empty())
        {
            cocos2d::CCSprite* icon = cocos2d::CCSprite::createWithSpriteFrameName(iconName.c_str());
            icon->setScale(iconScale);

            CCMenuItemSpriteExtra* socialBtn = CCMenuItemSpriteExtra::create(icon, nullptr, menu_selector(ProfilePage::onSocialPage));
            socialBtn->setTag(socialIdx);
            m_buttonMenu->addChild(socialBtn);

            cocos2d::CCPoint iconOffset(0.0f, -socialIndex * iconSpacing);
            cocos2d::CCPoint worldPos = socialStartPos + iconOffset;
            cocos2d::CCPoint nodePos = m_buttonMenu->convertToNodeSpace(worldPos);
            socialBtn->setPosition(nodePos);

            m_buttons->addObject(socialBtn);
            socialIndex += 1.0f;
        }
    }

    if (m_score->m_commentHistoryStatus == 2
        || (m_score->m_commentHistoryStatus == 1 && m_score->m_friendReqStatus != 1)
        || m_ownProfile
        || GameManager::sharedState()->m_hasRP.random - GameManager::sharedState()->m_hasRP.seed == 2)
    {
        cocos2d::CCSprite* historySprite = cocos2d::CCSprite::createWithSpriteFrameName("btn_chatHistory_001.png");
        CCMenuItemSpriteExtra* historyBtn = CCMenuItemSpriteExtra::create(historySprite, nullptr, menu_selector(ProfilePage::onCommentHistory));
        m_buttonMenu->addChild(historyBtn);

        cocos2d::CCPoint historyPos(leftArrowPos.x, winSize.height * 0.5f);
        cocos2d::CCPoint historyWorld = m_mainLayer->convertToWorldSpace(historyPos);
        historyBtn->setPosition(m_buttonMenu->convertToNodeSpace(historyWorld));
        m_buttons->addObject(historyBtn);
    }

    if (m_score->m_globalRank > 0)
    {
        int rank = m_score->m_globalRank;
        const char* rankSprite;

        if (rank == 1) rankSprite = "rankIcon_1_001.png";
        else if (rank <= 10) rankSprite = "rankIcon_top10_001.png";
        else if (rank <= 50) rankSprite = "rankIcon_top50_001.png";
        else if (rank <= 100) rankSprite = "rankIcon_top100_001.png";
        else if (rank <= 200) rankSprite = "rankIcon_top200_001.png";
        else if (rank <= 500) rankSprite = "rankIcon_top500_001.png";
        else if (rank <= 1000) rankSprite = "rankIcon_top1000_001.png";
        else if (rank <= 5000) rankSprite = "rankIcon_top2500_001.png";
        else if (rank <= 10000) rankSprite = "rankIcon_top5000_001.png";
        else if (rank <= 50000) rankSprite = "rankIcon_top10000_001.png";
        else rankSprite = "rankIcon_all_001.png";

        cocos2d::CCPoint rankPos(winSize.width * 0.5f - 164.0f, winSize.height * 0.5f + 123.0f);

        cocos2d::CCSprite* rankIcon = cocos2d::CCSprite::createWithSpriteFrameName(rankSprite);
        m_mainLayer->addChild(rankIcon, 10);
        rankIcon->setPosition(rankPos);
        m_buttons->addObject(rankIcon);

        cocos2d::CCSprite* rankTxt = cocos2d::CCSprite::createWithSpriteFrameName("gj_globalRankTxt_001.png");
        m_mainLayer->addChild(rankTxt, 10);
        rankTxt->setPosition(rankPos + cocos2d::CCPoint(40.0f, 6.0f));
        m_buttons->addObject(rankTxt);

        cocos2d::CCLabelBMFont* rankLabel = cocos2d::CCLabelBMFont::create(
            cocos2d::CCString::createWithFormat("%i", rank)->getCString(), "bigFont.fnt");
        m_mainLayer->addChild(rankLabel, 10);
        rankLabel->setPosition(rankPos + cocos2d::CCPoint(40.0f, -6.0f));
        rankLabel->setScale(0.6f);
        m_buttons->addObject(rankLabel);
    }

    if (m_score->m_modBadgeLevel > 0)
    {
        cocos2d::CCPoint badgePos(winSize.width * 0.5f - 180.0f, winSize.height * 0.5f + 100.0f);

        if (m_usernameLabel)
        {
            m_usernameLabel->limitLabelWidth(140.0f, 0.9f, 0.0f);
            cocos2d::CCPoint labelPos = m_usernameLabel->getPosition();
            cocos2d::CCSize labelSize = m_usernameLabel->getScaledContentSize();
            badgePos = labelPos + cocos2d::CCPoint(-16.0f - labelSize.width * 0.5f, -1.0f);
        }

        const char* badgeSprite = "modBadge_01_001.png";
        if (m_score->m_modBadgeLevel == 3) badgeSprite = "modBadge_03_001.png";
        else if (m_score->m_modBadgeLevel == 2) badgeSprite = "modBadge_02_001.png";

        cocos2d::CCSprite* badge = cocos2d::CCSprite::createWithSpriteFrameName(badgeSprite);
        m_mainLayer->addChild(badge, 10);
        badge->setPosition(badgePos);
        m_buttons->addObject(badge);
    }

    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSprite->setScale(0.45f);

    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSprite, nullptr, menu_selector(ProfilePage::onInfo));
    infoBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(infoBtn);

    float labelScale = m_usernameLabel->getScale();
    cocos2d::CCPoint labelPos = m_usernameLabel->getPosition();
    cocos2d::CCSize labelSize = m_usernameLabel->getContentSize();
    float infoX = labelScale * (labelSize.width * 0.5f) + 8.0f;
    float infoY = labelScale * (labelSize.height * 0.5f) - 5.0f;
    cocos2d::CCPoint infoWorld = labelPos + cocos2d::CCPoint(infoX, infoY);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(infoWorld));
    m_buttons->addObject(infoBtn);

    cocos2d::CCSprite* copySprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    copySprite->setScaleY(0.8f);
    copySprite->setVisible(false);

    CCMenuItemSpriteExtra* copyBtn = CCMenuItemSpriteExtra::create(copySprite, nullptr, menu_selector(ProfilePage::onCopyName));
    copyBtn->m_animationEnabled = false;
    m_buttonMenu->addChild(copyBtn);
    copyBtn->setPosition(m_buttonMenu->convertToNodeSpace(labelPos + cocos2d::CCPoint(0.0f, -2.0f)));
    m_buttons->addObject(copyBtn);

    if (m_ownProfile)
    {
        GameLevelManager::sharedState()->m_commentUploadDelegate = this;

        cocos2d::CCArray* accountButtons = cocos2d::CCArray::create();

        cocos2d::CCSprite* msgSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_messages_001.png");
        CCMenuItemSpriteExtra* msgBtn = CCMenuItemSpriteExtra::create(msgSprite, nullptr, menu_selector(ProfilePage::onMessages));
        msgBtn->setSizeMult(1.1f);
        m_buttonMenu->addChild(msgBtn);
        accountButtons->addObject(msgBtn);
        m_buttons->addObject(msgBtn);

        cocos2d::CCSprite* friendsSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_friends_001.png");
        CCMenuItemSpriteExtra* friendsBtn = CCMenuItemSpriteExtra::create(friendsSprite, nullptr, menu_selector(ProfilePage::onFriends));
        friendsBtn->setSizeMult(1.1f);
        m_buttonMenu->addChild(friendsBtn);
        accountButtons->addObject(friendsBtn);
        m_buttons->addObject(friendsBtn);

        cocos2d::CCSprite* requestsSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_requests_001.png");
        CCMenuItemSpriteExtra* requestsBtn = CCMenuItemSpriteExtra::create(requestsSprite, nullptr, menu_selector(ProfilePage::onRequests));
        requestsBtn->setSizeMult(1.1f);
        m_buttonMenu->addChild(requestsBtn);
        accountButtons->addObject(requestsBtn);
        m_buttons->addObject(requestsBtn);

        cocos2d::CCSprite* settingsSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png");
        CCMenuItemSpriteExtra* settingsBtn = CCMenuItemSpriteExtra::create(settingsSprite, nullptr, menu_selector(ProfilePage::onSettings));
        settingsBtn->setSizeMult(1.1f);
        m_buttonMenu->addChild(settingsBtn);
        accountButtons->addObject(settingsBtn);
        m_buttons->addObject(settingsBtn);

        cocos2d::CCPoint btnRowPos(winSize.width * 0.5f, winSize.height * 0.5f - 145.0f + 10.0f);
        cocos2d::CCPoint btnNodePos = m_buttonMenu->convertToNodeSpace(
            m_mainLayer->convertToWorldSpace(btnRowPos));

        GameToolbox::alignItemsHorizontally(accountButtons, 30.0f, btnNodePos);

        int idx = 1;
        cocos2d::CCSprite* msgSpriteCopy = msgSprite;
        cocos2d::CCSprite* friendsSpriteCopy = friendsSprite;
        cocos2d::CCSprite* requestsSpriteCopy = requestsSprite;

        while (idx < 4)
        {
            cocos2d::CCSprite* targetSprite = nullptr;
            int count = 0;

            if (idx == 1)
            {
                count = m_score->m_newMsgCount;
                targetSprite = msgSpriteCopy;
                if (count <= 0)
                {
                    idx++;
                    continue;
                }
            }
            else if (idx == 2)
            {
                count = m_score->m_newFriendCount;
                targetSprite = friendsSpriteCopy;
                if (count <= 0)
                {
                    idx++;
                    continue;
                }
            }
            else
            {
                count = m_score->m_friendReqCount;
                targetSprite = requestsSpriteCopy;
                if (count <= 0)
                    break;
            }

            float badgeOffsetX = (count == 1) ? -0.5f : 0.0f;

            cocos2d::CCSprite* badge = cocos2d::CCSprite::createWithSpriteFrameName("d_ball_01_001.png");
            badge->setColor(cocos2d::ccc3(255, 200, 0));
            badge->setScale(0.6f);

            cocos2d::CCLabelBMFont* badgeLabel = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("%i", count)->getCString(), "bigFont.fnt");
            badgeLabel->limitLabelWidth(28.0f, 0.8f, 0.0f);
            badge->addChild(badgeLabel);
            badgeLabel->setPosition(cocos2d::CCPoint(badgeOffsetX, 1.0f));

            targetSprite->addChild(badge, 1);
            badge->setPosition(cocos2d::CCPoint(38.0f, 38.0f));

            idx++;
        }

        cocos2d::CCSprite* chatSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
        CCMenuItemSpriteExtra* chatBtn = CCMenuItemSpriteExtra::create(chatSprite, nullptr, menu_selector(ProfilePage::onComment));
        chatBtn->setSizeMult(1.5f);
        m_buttonMenu->addChild(chatBtn);

        cocos2d::CCPoint chatPos(winSize.width * 0.5f + 220.0f - 10.0f, winSize.height * 0.5f - 145.0f + 10.0f + 1.0f);
        cocos2d::CCPoint chatWorld = m_mainLayer->convertToWorldSpace(chatPos);
        chatBtn->setPosition(m_buttonMenu->convertToNodeSpace(chatWorld));
        m_buttons->addObject(chatBtn);

        if (GameManager::sharedState()->getGameVariable("0076"))
        {
            chatBtn->setVisible(false);
            chatBtn->setEnabled(false);
        }

        return;
    }

    if (m_score->m_friendReqStatus != 2)
    {
        cocos2d::CCArray* socialButtons = cocos2d::CCArray::create();

        GJAccountManager* accMgr = GJAccountManager::sharedState();

        if (accMgr->m_accountID - accMgr->m_unkInt1 != m_accountID)
        {
            int friendReqStatus = m_score->m_friendReqStatus;

            if (m_score->m_messageState != 2)
            {
                if (m_score->m_messageState == 1 && friendReqStatus != 1)
                {
                    cocos2d::CCSprite* msgSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_messages_001.png");
                    msgSprite->setScale(0.75f);
                    CCMenuItemSpriteExtra* msgBtn = CCMenuItemSpriteExtra::create(msgSprite, nullptr, menu_selector(ProfilePage::onSendMessage));
                    msgBtn->setSizeMult(1.2f);
                    m_buttonMenu->addChild(msgBtn);
                    socialButtons->addObject(msgBtn);
                    m_buttons->addObject(msgBtn);
                }
            }
            else
            {
                cocos2d::CCSprite* msgSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_messages_001.png");
                msgSprite->setScale(0.75f);
                CCMenuItemSpriteExtra* msgBtn = CCMenuItemSpriteExtra::create(msgSprite, nullptr, menu_selector(ProfilePage::onSendMessage));
                msgBtn->setSizeMult(1.2f);
                m_buttonMenu->addChild(msgBtn);
                socialButtons->addObject(msgBtn);
                m_buttons->addObject(msgBtn);
            }

            if (m_score->m_friendStatus != 1 || friendReqStatus != 0)
            {
                const char* friendSprite;
                switch (friendReqStatus)
                {
                    case 3: friendSprite = "accountBtn_pendingRequest_001.png"; break;
                    case 4: friendSprite = "accountBtn_pending_001.png"; break;
                    case 1: friendSprite = "accountBtn_removeFriend_001.png"; break;
                    default: friendSprite = "accountBtn_requests_001.png"; break;
                }

                cocos2d::CCSprite* friendIcon = cocos2d::CCSprite::createWithSpriteFrameName(friendSprite);
                friendIcon->setScale(0.75f);
                CCMenuItemSpriteExtra* friendBtn = CCMenuItemSpriteExtra::create(friendIcon, nullptr, menu_selector(ProfilePage::onFriend));
                friendBtn->setSizeMult(1.2f);
                m_buttonMenu->addChild(friendBtn);
                socialButtons->addObject(friendBtn);
                m_buttons->addObject(friendBtn);
            }

            cocos2d::CCSprite* blockIcon = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_blocked_001.png");
            blockIcon->setScale(0.75f);
            CCMenuItemSpriteExtra* blockBtn = CCMenuItemSpriteExtra::create(blockIcon, nullptr, menu_selector(ProfilePage::onBlockUser));
            blockBtn->setSizeMult(1.2f);
            m_buttonMenu->addChild(blockBtn);
            socialButtons->addObject(blockBtn);
            m_buttons->addObject(blockBtn);

            cocos2d::CCPoint socialBtnPos(winSize.width * 0.5f, winSize.height * 0.5f - 123.0f);
            cocos2d::CCPoint socialBtnNodePos = m_buttonMenu->convertToNodeSpace(
                m_mainLayer->convertToWorldSpace(socialBtnPos));

            GameToolbox::alignItemsHorizontally(socialButtons, 12.0f, socialBtnNodePos);

            cocos2d::CCPoint myLevelsPos(winSize.width * 0.5f + 220.0f - 22.0f - 1.0f, winSize.height * 0.5f - 145.0f + 22.0f);

            cocos2d::CCSprite* myLevelsSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_myLevels_001.png");
            myLevelsSprite->setScale(0.75f);
            CCMenuItemSpriteExtra* myLevelsBtn = CCMenuItemSpriteExtra::create(myLevelsSprite, nullptr, menu_selector(ProfilePage::onMyLevels));
            myLevelsBtn->setSizeMult(1.2f);
            m_buttonMenu->addChild(myLevelsBtn);
            socialButtons->addObject(myLevelsBtn);
            m_buttons->addObject(myLevelsBtn);

            cocos2d::CCPoint myLevelsWorld = m_mainLayer->convertToWorldSpace(myLevelsPos);
            myLevelsBtn->setPosition(m_buttonMenu->convertToNodeSpace(myLevelsWorld));

            cocos2d::CCSprite* myLevelsTxt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_myLevelsTxt_001.png");
            m_mainLayer->addChild(myLevelsTxt, 2);
            myLevelsTxt->setPosition(myLevelsPos + cocos2d::CCPoint(-1.0f, 26.0f));
            m_buttons->addObject(myLevelsTxt);

            cocos2d::CCPoint myListsPos = myLevelsPos + cocos2d::CCPoint(-42.0f, 0.0f);

            cocos2d::CCSprite* myListsSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_myLists_001.png");
            myListsSprite->setScale(0.75f);
            CCMenuItemSpriteExtra* myListsBtn = CCMenuItemSpriteExtra::create(myListsSprite, nullptr, menu_selector(ProfilePage::onMyLists));
            myListsBtn->setSizeMult(1.2f);
            m_buttonMenu->addChild(myListsBtn);
            socialButtons->addObject(myListsBtn);
            m_buttons->addObject(myListsBtn);

            cocos2d::CCPoint myListsWorld = m_mainLayer->convertToWorldSpace(myListsPos);
            myListsBtn->setPosition(m_buttonMenu->convertToNodeSpace(myListsWorld));

            cocos2d::CCSprite* myListsTxt = cocos2d::CCSprite::createWithSpriteFrameName("GJ_myListsTxt_001.png");
            m_mainLayer->addChild(myListsTxt, 2);
            myListsTxt->setPosition(myListsPos + cocos2d::CCPoint(-46.0f, 0.0f));
            m_buttons->addObject(myListsTxt);

            if (GameManager::sharedState()->getGameVariable("0077"))
            {
                myLevelsBtn->setVisible(false);
                myLevelsBtn->setEnabled(false);
                myLevelsTxt->setVisible(false);
                myListsBtn->setVisible(false);
                myListsBtn->setEnabled(false);
                myListsTxt->setVisible(false);
            }
        }
    }
}

void ProfilePage::onBlockUser(cocos2d::CCObject* sender)
{
    GJAccountManager* accMgr = GJAccountManager::sharedState();

    if (accMgr->m_accountID - accMgr->m_unkInt1 <= 0)
    {
        showNoAccountError();
        return;
    }

    std::string desc = cocos2d::CCString::createWithFormat(
        "Are you sure you want to block <cg>%s</c>?\n"
        "<cg>%s</c> will no longer be able to:\n"
        "- <cy>View your profile</c>\n"
        "- <cl>Send messages</c>\n"
        "- <cp>Send friend requests</c>\n"
        "- <cr>Messages from this user will be removed</c>",
        m_score->m_userName.c_str(),
        m_score->m_userName.c_str())->getCString();

    FLAlertLayer* alert = FLAlertLayer::create(this, "Confirm Block", desc, "Block");
    alert->setTag(3);
    alert->show();
}

void ProfilePage::onClose(cocos2d::CCObject* sender)
{
    if (m_list)
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_list);

    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ProfilePage::onClosePopup(UploadActionPopup* popup)
{
    popup->closePopup();

    if (m_popupDelegate == popup)
        m_popupDelegate = nullptr;
}

void ProfilePage::onComment(cocos2d::CCObject* sender)
{
    int timeLeft = GameLevelManager::sharedState()->getTimeLeft("acc_comment", 15.0);

    if ((unsigned int)(timeLeft - 1) > 14)
    {
        ShareCommentLayer* layer = ShareCommentLayer::create("Post Update", 140, CommentType::Account, nullptr);
        layer->show();
    }
    else
    {
        std::string msg = cocos2d::CCString::createWithFormat(
            "Please wait %i seconds before posting a new update!", timeLeft)->getCString();
        FLAlertLayer::create(nullptr, "Too fast", msg)->show();
    }
}

void ProfilePage::onCommentHistory(cocos2d::CCObject* sender)
{
    InfoLayer* layer = InfoLayer::create(nullptr, m_score, nullptr);
    layer->m_scene = this;
    layer->show();
}

void ProfilePage::onCopyName(cocos2d::CCObject* sender)
{
    if (!m_score)
        return;

    GameLevelManager::sharedState()->tryGetUsername(m_score->m_accountID);

    std::string username = GameLevelManager::sharedState()->tryGetUsername(m_score->m_accountID);

    if (username.empty())
        username = m_score->m_userName;

    if (username.empty())
        username = " ";

    PlatformToolbox::copyToClipboard(username);
    TextAlertPopup::create("Copied to clipboard", 0.5f, 0.6f, 150)->show();
}

void ProfilePage::onFollow(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->reportAchievementWithID("geometry.ach.followCreator", 100);

    if (GameLevelManager::sharedState()->isFollowingUser(m_accountID))
    {
        GameLevelManager::sharedState()->unfollowUser(m_accountID);
        cocos2d::CCSprite* normalImage = static_cast<cocos2d::CCSprite*>(m_followBtn->getNormalImage());
        normalImage->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("gj_heartOff_001.png"));
        return;
    }

    int followedCount = GameLevelManager::sharedState()->m_followedCreators->count();
    float countFloat = followedCount + 1.0f;

    GameManager::sharedState()->reportAchievementWithID("geometry.ach.followCreator02", (int)floorf((countFloat / 10.0f) * 100.0f));
    GameManager::sharedState()->reportAchievementWithID("geometry.ach.followCreator03", (int)((countFloat / 25.0f) * 100.0f));
    GameManager::sharedState()->reportAchievementWithID("geometry.ach.followCreator04", (int)((countFloat / 50.0f) * 100.0f));

    if (followedCount <= 99)
    {
        GameLevelManager::sharedState()->followUser(m_accountID);
        cocos2d::CCSprite* normalImage = static_cast<cocos2d::CCSprite*>(m_followBtn->getNormalImage());
        normalImage->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("gj_heartOn_001.png"));
        return;
    }

    std::string msg = cocos2d::CCString::createWithFormat(
        "You can only follow <cy>%i</c> users at the same time.", 100)->getCString();

    FLAlertLayer::create(nullptr, "Max Followed", msg)->show();
}

void ProfilePage::onFriend(cocos2d::CCObject* sender)
{
    GJAccountManager* accMgr = GJAccountManager::sharedState();

    if (accMgr->m_accountID - accMgr->m_unkInt1 <= 0)
    {
        showNoAccountError();
        return;
    }

    switch (m_score->m_friendReqStatus)
    {
        case 0:
        {
            ShareCommentLayer* layer = ShareCommentLayer::create("Friend request", 140, CommentType::FriendRequest, m_accountID);
            layer->show();
            break;
        }
        case 4:
        {
            FLAlertLayer* alert = FLAlertLayer::create(this, "Cancel friend request",
                "Are you sure you want to remove this friend request?", "Remove");
            alert->setTag(1);
            alert->show();
            break;
        }
        case 3:
        {
            GJFriendRequest* request = GameLevelManager::sharedState()->friendRequestFromAccountID(m_score->m_accountID);
            m_score->m_newFriendRequest = false;
            FriendRequestPopup::create(request)->show();
            break;
        }
        case 1:
        {
            FLAlertLayer* alert = FLAlertLayer::create(this, "Unfriend",
                "Are you sure you want to remove this friend?", "Unfriend");
            alert->setTag(2);
            alert->show();
            break;
        }
    }
}

void ProfilePage::onFriends(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    FriendsProfilePage::create(UserListType::Default)->show();
}

void ProfilePage::onInfo(cocos2d::CCObject* sender)
{
    if (!m_score)
        return;

    GameLevelManager::sharedState()->tryGetUsername(m_score->m_accountID);

    std::string username = GameLevelManager::sharedState()->tryGetUsername(m_score->m_accountID);

    if (username.empty())
        username = m_score->m_userName;

    if (username.empty())
        username = " ";

    std::string desc = cocos2d::CCString::createWithFormat(
        "<cy>Stars:</c> %s\n"
        "<cb>Moons:</c> %s\n"
        "<cf>Diamonds:</c> %s\n"
        "<co>Secret Coins:</c> %s\n"
        "<cc>User Coins:</c> %s\n"
        "<cr>Demons:</c> %s",
        GameToolbox::pointsToString(m_score->m_stars).c_str(),
        GameToolbox::pointsToString(m_score->m_moons).c_str(),
        GameToolbox::pointsToString(m_score->m_diamonds).c_str(),
        GameToolbox::pointsToString(m_score->m_secretCoins).c_str(),
        GameToolbox::pointsToString(m_score->m_userCoins).c_str(),
        GameToolbox::pointsToString(m_score->m_demons).c_str())->getCString();

    if (m_score->m_creatorPoints > 0)
    {
        desc += "\n<cg>Creator Points:</c> " + GameToolbox::pointsToString(m_score->m_creatorPoints);
    }

    if (!m_score->m_discordUsername.empty())
    {
        desc += "\n<cl>Discord:</c> @" + m_score->m_discordUsername;
    }

    FLAlertLayer::create(nullptr, username, desc)->show();
}

void ProfilePage::onMessages(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    MessagesProfilePage::create(false)->show();
}

void ProfilePage::onMyLevels(cocos2d::CCObject* sender)
{
    if (m_ownProfile)
    {
        GJSearchObject* search = GJSearchObject::create(SearchType::MyLevels);
        cocos2d::CCScene* scene = LevelBrowserLayer::scene(search);
        cocos2d::CCDirector::sharedDirector()->pushScene(cocos2d::CCTransitionFade::create(0.5f, scene));
        return;
    }

    if (cocos2d::CCDirector::sharedDirector()->m_bIsTransitioning)
        return;

    if (cocos2d::CCDirector::sharedDirector()->m_pRunningScene->getObjType() != MenuLayerObjectType)
    {
        if (cocos2d::CCDirector::sharedDirector()->m_pSceneReference)
            cocos2d::CCDirector::sharedDirector()->popToSceneInStack(cocos2d::CCDirector::sharedDirector()->m_pSceneReference);
        else
            cocos2d::CCDirector::sharedDirector()->m_pSceneReference = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    }

    int userID = GameLevelManager::sharedState()->userIDForAccountID(m_accountID);
    GJSearchObject* search = GJSearchObject::create(SearchType::UsersLevels,
        cocos2d::CCString::createWithFormat("%i", userID)->getCString());
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(search);
    cocos2d::CCDirector::sharedDirector()->pushScene(cocos2d::CCTransitionFade::create(0.5f, scene));
}

void ProfilePage::onMyLists(cocos2d::CCObject* sender)
{
    GJSearchObject* search = nullptr;

    if (m_ownProfile)
    {
        search = GJSearchObject::create(SearchType::MyLists);
    }
    else
    {
        if (cocos2d::CCDirector::sharedDirector()->m_bIsTransitioning)
            return;

        if (cocos2d::CCDirector::sharedDirector()->m_pRunningScene->getObjType() != MenuLayerObjectType)
        {
            if (cocos2d::CCDirector::sharedDirector()->m_pSceneReference)
                cocos2d::CCDirector::sharedDirector()->popToSceneInStack(cocos2d::CCDirector::sharedDirector()->m_pSceneReference);
            else
                cocos2d::CCDirector::sharedDirector()->m_pSceneReference = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
        }

        search = GJSearchObject::create(SearchType::UsersLevels,
            cocos2d::CCString::createWithFormat("%i", m_accountID)->getCString());
    }

    search->m_searchMode = 1;
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(search);
    cocos2d::CCDirector::sharedDirector()->pushScene(cocos2d::CCTransitionFade::create(0.5f, scene));
}

void ProfilePage::onNextPage(cocos2d::CCObject* sender)
{
    loadPage(m_page + 1);
}

void ProfilePage::onPrevPage(cocos2d::CCObject* sender)
{
    loadPage(m_page - 1);
}

void ProfilePage::onRequests(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    FRequestProfilePage::create(false)->show();
}

void ProfilePage::onSendMessage(cocos2d::CCObject* sender)
{
    GJAccountManager* accMgr = GJAccountManager::sharedState();

    if (accMgr->m_accountID - accMgr->m_unkInt1 <= 0)
    {
        showNoAccountError();
        return;
    }

    GJWriteMessagePopup::create(m_accountID, 0)->show();
}

void ProfilePage::onSettings(cocos2d::CCObject* sender)
{
    GJAccountSettingsLayer::create(m_accountID)->show();
}

void ProfilePage::onSocialPage(cocos2d::CCObject* sender)
{
    if (!m_score)
        return;

    std::string url;

    switch (sender->getTag())
    {
        case 0:
        {
            std::string yt = m_score->m_youtubeURL;
            if (yt.find("UC") == 0 && yt.length() == 24)
                url = "https://www.youtube.com/channel/" + yt;
            else
                url = "https://www.youtube.com/user/" + yt;
            break;
        }
        case 1: url = "https://twitter.com/" + m_score->m_twitterURL; break;
        case 2: url = "https://www.twitch.tv/" + m_score->m_twitchURL; break;
        case 3: url = "https://www.instagram.com/" + m_score->m_instagramURL; break;
        case 4: url = "https://www.tiktok.com/@" + m_score->m_tiktokURL; break;
        default: return;
    }

    if (!url.empty())
        cocos2d::CCApplication::sharedApplication()->openURL(url.c_str());
}

void ProfilePage::onStatInfo(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag == 1)
    {
        if (!m_score || m_score->m_starsInfo.empty())
            return;

        StarInfoPopup::createFromString(m_score->m_starsInfo)->show();
        return;
    }

    if (tag == 2)
    {
        if (!m_score || m_score->m_platformerInfo.empty())
            return;

        StarInfoPopup::createFromStringMoons(m_score->m_platformerInfo)->show();
        return;
    }

    if (tag == 3)
    {
        if (!m_score || m_score->m_demonInfo.empty())
            return;

        DemonInfoPopup::createFromString(m_score->m_demonInfo)->show();
    }
}

void ProfilePage::onUpdate(cocos2d::CCObject* sender)
{
    if (m_ownProfile)
    {
        if ((unsigned int)(GameLevelManager::sharedState()->getTimeLeft("upd_profile", 5.0) - 1) > 4)
            return;

        GameLevelManager::sharedState()->makeTimeStamp("upd_profile");

        GJAccountManager* accMgr = GJAccountManager::sharedState();
        GameLevelManager* glm = GameLevelManager::sharedState();

        glm->resetStoredUserInfo(accMgr->m_accountID - accMgr->m_unkInt1);
        glm->resetAccountComments(accMgr->m_accountID - accMgr->m_unkInt1);
        glm->resetCommentTimersForAccountID(accMgr->m_accountID - accMgr->m_unkInt1);
        glm->invalidateMessages(1, 0);
        glm->invalidateMessages(0, 0);
        glm->invalidateRequests(1, 0);
        glm->invalidateRequests(0, 0);
        glm->invalidateUserList(UserListType::Default, 0);
        glm->invalidateUserList(UserListType::Blocked, 0);

        if (m_score)
        {
            cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            m_circle->m_sprite->setPosition(cocos2d::CCPoint(
                winSize.width * 0.5f - 220.0f + 10.0f,
                winSize.height * 0.5f - 145.0f + 10.0f + 1.0f));
            m_circle->m_sprite->setScale(0.8f);
        }

        m_somethingWentWrong->setVisible(false);
        m_circle->setVisible(true);
        m_refreshBtn->setVisible(false);
        GameLevelManager::sharedState()->getGJUserInfo(m_accountID);
    }
    else
    {
        GameLevelManager::sharedState()->resetCommentTimersForAccountID(m_accountID);
        m_circle->setVisible(true);
        m_leftArrow->setVisible(false);
        m_rightArrow->setVisible(false);
        m_refreshBtn->setVisible(false);
        setupCommentsBrowser(nullptr);
        GameLevelManager::sharedState()->getGJUserInfo(m_accountID);
    }
}

void ProfilePage::registerWithTouchDispatcher()
{
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void ProfilePage::setupComments()
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float arrowsY = winSize.height * 0.5f - 53.0f;

    cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftSprite->setScale(0.8f);
    m_leftArrow = CCMenuItemSpriteExtra::create(leftSprite, nullptr, menu_selector(ProfilePage::onPrevPage));
    m_buttonMenu->addChild(m_leftArrow);
    m_leftArrow->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 195.0f, arrowsY)));
    m_leftArrow->setSizeMult(2.0f);

    cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightSprite->setFlipX(true);
    rightSprite->setScale(0.8f);
    m_rightArrow = CCMenuItemSpriteExtra::create(rightSprite, nullptr, menu_selector(ProfilePage::onNextPage));
    m_buttonMenu->addChild(m_rightArrow);
    m_rightArrow->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 195.0f, arrowsY)));
    m_rightArrow->setSizeMult(2.0f);

    m_leftArrow->setVisible(false);
    m_rightArrow->setVisible(false);
}

void ProfilePage::setupCommentsBrowser(cocos2d::CCArray* comments)
{
    if (m_list)
    {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_list);
        m_list->removeFromParentAndCleanup(true);
        m_list = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    CustomListView* listView = comments ? CustomListView::create(comments, BoomListType::Level, 100.0f, 340.0f) : nullptr;

    m_list = GJCommentListLayer::create(listView, "Comments", cocos2d::ccc4(0, 0, 0, 0), 340.0f, 100.0f, false);
    m_mainLayer->addChild(m_list);
    m_list->setPosition(cocos2d::CCPoint(
        (winSize.width - 340.0f) * 0.5f,
        (winSize.height - 100.0f) * 0.5f - 53.0f));

    if (m_ownProfile)
        m_buttonMenu->setTouchPriority(-503);
}

void ProfilePage::setupPageInfo(std::string info, const char* key)
{
    if (!isCorrect(key))
        return;

    // Formato: "itemCount:pageStartIdx:pageEndIdx"
    std::vector<std::string> tokens;
    std::string current;
    for (char c : info)
    {
        if (c == ':')
        {
            tokens.push_back(current);
            current.clear();
        }
        else
        {
            current += c;
        }
    }
    tokens.push_back(current);

    if (tokens.size() <= 2)
        return;

    m_itemCount = atoi(tokens[0].c_str());
    m_pageStartIdx = atoi(tokens[1].c_str());
    m_pageEndIdx = atoi(tokens[2].c_str());

    updatePageArrows();
}

void ProfilePage::show()
{
    if (m_noElasticity)
    {
        FLAlertLayer::show();
        return;
    }

    unsigned __int8 opacity = getOpacity();

    m_mainLayer->setScale(0.1f);
    m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
        cocos2d::CCScaleTo::create(0.3f, 1.0f), 1.6f));

    cocos2d::CCScene* scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;

    if (!m_scene)
    {
        int highestZ = cocos2d::CCScene::getHighestChildZ(scene);
        m_ZOrder = (highestZ > 104) ? (highestZ + 1) : 105;
        m_scene = scene;
    }
    else if (!m_ZOrder)
    {
        m_ZOrder = 105;
    }

    m_scene->addChild(this, m_ZOrder);
    setOpacity(0);
    runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
    setVisible(true);
}

void ProfilePage::showNoAccountError()
{
    FLAlertLayer::create(nullptr, "No Account",
        "Create an <cg>account</c> to send messages, add friends and more! "
        "You can create an account from the main menu.")->show();
}

void ProfilePage::toggleMainPageVisibility(bool visible)
{
    if (m_arrayWithUsernameLabel)
    {
        for (unsigned int i = 0; i < m_arrayWithUsernameLabel->count(); i++)
        {
            cocos2d::CCNode* node = static_cast<cocos2d::CCNode*>(m_arrayWithUsernameLabel->objectAtIndex(i));
            node->setVisible(visible);
        }
    }

    if (m_buttons)
    {
        for (unsigned int i = 0; i < m_buttons->count(); i++)
        {
            cocos2d::CCNode* node = static_cast<cocos2d::CCNode*>(m_buttons->objectAtIndex(i));
            node->setVisible(visible);
        }
    }

    if (m_list)
        m_list->setVisible(visible);

    if (visible)
    {
        updatePageArrows();
    }
    else
    {
        m_rightArrow->setVisible(false);
        m_leftArrow->setVisible(false);
    }
}

void ProfilePage::toggleShip(cocos2d::CCObject* sender)
{
    SimplePlayer* player = static_cast<SimplePlayer*>(sender->getUserObject());

    bool isShip = sender->getTag() == 1;

    int frame;
    IconType type;
    float scale;

    if (isShip)
    {
        frame = 8;
        type = IconType::Jetpack;
        scale = 0.9f;
    }
    else
    {
        frame = m_score->m_playerShip;
        type = IconType::Ship;
        scale = 1.0f;
    }

    player->updatePlayerFrame(frame, type);
    player->setScale(scale);
    sender->setTag(isShip ? 0 : 1);
}

void ProfilePage::updateLevelsLabel()
{
}

void ProfilePage::updatePageArrows()
{
    if (m_pageStartIdx)
        m_leftArrow->setVisible(true);
    else
        m_leftArrow->setVisible(false);

    if (m_pageStartIdx + m_pageEndIdx < m_itemCount)
        m_rightArrow->setVisible(true);
    else
        m_rightArrow->setVisible(false);
}

void ProfilePage::updateUserScoreFailed()
{
    m_circle->setVisible(false);
    m_refreshBtn->setVisible(true);
    m_somethingWentWrong->setVisible(true);
}

void ProfilePage::updateUserScoreFinished()
{
    GJUserScore* score = GameLevelManager::sharedState()->userInfoForAccountID(m_accountID);

    if (score)
    {
        getUserInfoFinished(score);
        return;
    }

    m_circle->setVisible(true);
    m_refreshBtn->setVisible(false);
    setupCommentsBrowser(nullptr);
    GameLevelManager::sharedState()->getGJUserInfo(m_accountID);
}

void ProfilePage::uploadActionFailed(int id, int response)
{
    if (m_accountID == id && m_popupDelegate)
        m_popupDelegate->showFailMessage("Failed. Please try again later.");
}

void ProfilePage::uploadActionFinished(int id, int response)
{
    if (m_accountID != id || !m_popupDelegate)
        return;

    std::string message = "Unknown action finished";

    switch (response)
    {
        case 30: message = "Request removed"; break;
        case 33: message = "Friend removed"; break;
        case 34: message = "User blocked"; break;
    }

    m_popupDelegate->showSuccessMessage(message);
}

void ProfilePage::userInfoChanged(GJUserScore* score)
{
    if (score->m_accountID != m_accountID)
        return;

    loadPageFromUserInfo(score);

    if (score->m_friendReqStatus == 2)
    {
        setupCommentsBrowser(nullptr);
        m_rightArrow->setVisible(false);
        m_leftArrow->setVisible(false);
    }
}

ProfilePage::~ProfilePage()
{
}