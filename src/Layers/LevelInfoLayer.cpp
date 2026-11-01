#include "Layers/LevelInfoLayer.h"
#include "Models/GJGameLevel.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Utils/GameToolbox.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/LoadingCircle.h"
#include "UI/CustomSongWidget.h"
#include "Sprites/GJDifficultySprite.h"
#include "Layers/FLAlertLayer.h"
#include "Layers/NumberInputLayer.h"
#include "Popups/SetIDPopup.h"
#include "Popups/SetFolderPopup.h"
#include "Layers/PlayLayer.h"
#include "Layers/EditLevelLayer.h"
#include "Layers/LevelBrowserLayer.h"
#include "Layers/LevelLeaderboard.h"
#include "Layers/GameLevelOptionsLayer.h"
#include "Layers/RateLevelLayer.h"
#include "Layers/RateStarsLayer.h"
#include "Layers/RateDemonLayer.h"
#include "Layers/GJRateLevelLayer.h"
#include "Pages/ProfilePage.h"
#include "Models/SongInfoObject.h"
#include "Managers/MusicDownloadManager.h"
#include "Utils/LevelTools.h"
#include "Models/GJSearchObject.h"
#include "Layers/GauntletLayer.h"
#include "Layers/GJGarageLayer.h"
#include "Layers/InfoLayer.h"
#include "Utils/PlatformToolbox.h"
#include "Audio/FMODAudioEngine.h"
#include "Delegates/AppDelegate.h"
#include <ctime>

void LevelInfoLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    switch (layer->getTag()) {
        case 4:
            if (btn2) onDelete(nullptr);
            break;
        case 5:
            if (btn2) onOwnerDelete(nullptr);
            break;
        case 6:
            if (btn2) onClone(nullptr);
            break;
        case 7:
            onRate(nullptr);
            break;
        case 8:
            onRateStars(nullptr);
            break;
        case 9:
            if (btn2) {
                m_level->m_showedSongWarning = true;
                onPlay(nullptr);
            }
            break;
        case 10:
            if (btn2) {
                m_level->m_highObjectsEnabled = true;
                onPlay(nullptr);
            }
            break;
        case 11:
            if (btn2 && m_level) {
                m_level->m_levelIndex = GameLevelManager::sharedState()->getHighestLevelOrder() + 1;
            }
            break;
        case 12:
            if (btn2 && m_level) {
                m_level->m_levelIndex = GameLevelManager::sharedState()->getLowestLevelOrder() - 1;
            }
            break;
        default:
            break;
    }
}

void LevelInfoLayer::confirmClone(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        int userID = m_level->m_userID.random - m_level->m_userID.seed;
        int currentUserID = GameManager::sharedState()->m_playerUserID.random - GameManager::sharedState()->m_playerUserID.seed;

        std::string message;
        if (userID == currentUserID) {
            message = "Create a <cl>copy</c> of this <cg>level</c>?\n<cy>(Creator enabled)</c>";
        } else {
            message = "Create a <cl>copy</c> of this <cg>level</c>?";
        }

        FLAlertLayer* alert = FLAlertLayer::create(this, "Clone Level", message.c_str());
        alert->setTag(6);
        alert->show();
    }
}

void LevelInfoLayer::confirmDelete(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        std::string message = "Are you sure you want to <cr>delete</c> this level?";
        FLAlertLayer* alert = FLAlertLayer::create(this, "Delete Level", message.c_str());
        alert->setTag(4);
        alert->show();
    }
}

void LevelInfoLayer::confirmMoveToBottom(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        std::string message = "Move this level to the bottom of the levels list?";
        FLAlertLayer* alert = FLAlertLayer::create(this, "Move to Bottom", message.c_str());
        alert->setTag(12);
        alert->show();
    }
}

void LevelInfoLayer::confirmMoveToTop(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        std::string message = "Move this level to the top of the levels list?";
        FLAlertLayer* alert = FLAlertLayer::create(this, "Move to Top", message.c_str());
        alert->setTag(11);
        alert->show();
    }
}

void LevelInfoLayer::confirmOwnerDelete(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        GameManager* GM = GameManager::sharedState();
        GJAccountManager* AM = GJAccountManager::sharedState();

        int userID = m_level->m_userID.random - m_level->m_userID.seed;
        int currentUserID = GM->m_playerUserID.random - GM->m_playerUserID.seed;
        int accountID = m_level->m_accountID.random - m_level->m_accountID.seed;
        int currentAccountID = AM->m_accountID - AM->m_unkInt1;

        std::string message;
        if (currentUserID == userID || (AM->m_accountID != AM->m_unkInt1 && currentAccountID == accountID)) {
            message = "Do you want to <cr>delete</c> this <cg>level</c> from the <cl>server</c>?\n<cy>(Your level will no longer be available online)</c>";
        } else {
            int modRank = GM->m_hasRP.random - GM->m_hasRP.seed;
            message = "Do you want to <cr>delete</c> this <cg>level</c> from the <cl>server</c>?\n<cy>(Your level will no longer be available online)</c>";
            if (modRank > 1) {
                message = "Do you want to <cr>delete</c> this <cg>level</c> from the <cl>server</c>?\n<cy>(The level will no longer be available online)</c>\n<cg>(Elder Enabled)</c>";
            }
        }

        FLAlertLayer* alert = FLAlertLayer::create(this, "Remove Server Level", message.c_str());
        ButtonSprite::updateBGImage(alert->m_button2, "GJ_button_06.png");
        alert->setTag(5);
        alert->show();
    }
}

LevelInfoLayer* LevelInfoLayer::create(GJGameLevel* level, bool challenge) {
    LevelInfoLayer* layer = new LevelInfoLayer();
    if (layer && layer->init(level, challenge)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void LevelInfoLayer::downloadLevel() {
    m_playBtnMenu->setVisible(false);
    m_circle->setVisible(true);

    GameLevelManager* GLM = GameLevelManager::sharedState();
    if (GLM->m_searchType != SearchType::SavedLevels && GLM->m_searchType != SearchType::FavouriteLevels) {
        if (GameManager::sharedState()->getGameVariable("0084")) {
            if (m_level->m_levelIndex == 0) {
                m_level->m_levelIndex = GLM->getLowestLevelOrder();
            }
        } else {
            m_level->m_levelIndex = GLM->getHighestLevelOrder() + 1;
        }
    }

    GLM->saveLevel(m_level);
    GLM->m_levelDownloadDelegate = this;
    GLM->downloadLevel(m_level->m_levelID.random - m_level->m_levelID.seed, false, 0);
}

void LevelInfoLayer::incrementDislikes() {
    ++m_level->m_dislikes;
    updateLabelValues();
}

void LevelInfoLayer::incrementLikes() {
    ++m_level->m_likes;
    updateLabelValues();
}

bool LevelInfoLayer::init(GJGameLevel* level, bool challenge) {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    m_level = level;
    m_level->retain();
    m_challenge = challenge;

    m_coins = cocos2d::CCArray::create();
    m_coins->retain();

    m_levelType = level->m_levelType;
    m_level->m_levelNotDownloaded = false;

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();

    cocos2d::CCSprite* bg = cocos2d::CCSprite::create("GJ_gradientBG.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    bg->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    bg->setScaleX((winSize.width + 10.0f) / bg->getContentSize().width);
    bg->setScaleY((winSize.height + 10.0f) / bg->getContentSize().height);
    bg->setColor(GameLevelManager::sharedState()->colorForIdx(level->m_levelIndex));
    this->addChild(bg, -2);

    cocos2d::CCSprite* cornerBL = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBL->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    cornerBL->setPosition(cocos2d::CCPoint(director->getScreenLeft(), director->getScreenBottom()));
    this->addChild(cornerBL, -1);

    cocos2d::CCSprite* cornerBR = cocos2d::CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBR->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.0f));
    cornerBR->setPosition(cocos2d::CCPoint(director->getScreenRight(), director->getScreenBottom()));
    cornerBR->setFlipX(true);
    this->addChild(cornerBR, -1);

    std::string levelName = m_level->m_levelName;
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, director->getScreenTop() - 30.0f));
    this->addChild(titleLabel);

    std::string creatorText;
    if (m_level->m_accountID.random - m_level->m_accountID.seed > 0) {
        creatorText = "By " + m_level->m_creatorName;
    } else if (m_level->m_userName.empty()) {
        creatorText = "By Unknown";
    } else {
        creatorText = "By " + m_level->m_userName;
    }

    cocos2d::CCLabelBMFont* creatorLabel = cocos2d::CCLabelBMFont::create(creatorText.c_str(), "goldFont.fnt");
    creatorLabel->setScale(0.8f);

    cocos2d::CCMenu* creatorMenu = cocos2d::CCMenu::create();
    this->addChild(creatorMenu);
    creatorMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, titleLabel->getPositionY() - 25.0f));

    CCMenuItemSpriteExtra* creatorBtn = CCMenuItemSpriteExtra::create(creatorLabel, nullptr, this, menu_selector(LevelInfoLayer::onViewProfile));
    creatorMenu->addChild(creatorBtn);

    m_playBtnMenu = cocos2d::CCMenu::create();
    this->addChild(m_playBtnMenu, 2);

    m_playSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    CCMenuItemSpriteExtra* playBtn = CCMenuItemSpriteExtra::create(m_playSprite, nullptr, this, menu_selector(LevelInfoLayer::onPlay));
    m_playBtnMenu->addChild(playBtn);
    playBtn->setPosition(cocos2d::CCPoint(0.0f, -40.0f));

    m_circle = LoadingCircle::create();
    m_circle->setParentLayer(this);
    m_circle->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 40.0f));
    m_circle->setVisible(false);

    if (m_level->m_levelType == GJLevelType::SearchResult) {
        downloadLevel();
    } else {
        if (m_level->m_levelString.empty()) {
            if (GameLevelManager::sharedState()->m_searchType != SearchType::SavedLevels &&
                GameLevelManager::sharedState()->m_searchType != SearchType::FavouriteLevels) {
                downloadLevel();
            } else {
                m_noUpdateAlert = true;
                m_playBtnMenu->setVisible(false);
                m_circle->setVisible(true);
                GameLevelManager::sharedState()->m_levelUpdateDelegate = this;

                int oldVersion = m_level->m_levelVersion;
                m_level->m_levelVersion = -1;
                GameLevelManager::sharedState()->updateLevel(m_level);
                m_level->m_levelVersion = oldVersion;
            }
        }
    }

    setupLevelInfo();
    if (!m_challenge) {
        if (GJGameLevel::isPlatformer(m_level)) {
            setupPlatformerStats();
        } else {
            setupProgressBars();
        }
    }

    cocos2d::CCMenu* rightMenu = cocos2d::CCMenu::create();
    this->addChild(rightMenu, 10);
    rightMenu->setPosition(cocos2d::CCPoint(director->getScreenRight() - 30.0f, director->getScreenTop() - 30.0f));

    float rightBtnY = 0.0f;

    if (m_level->m_dailyID.random - m_level->m_dailyID.seed <= 0) {
        cocos2d::CCSprite* deleteSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
        CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(deleteSpr, nullptr, this, menu_selector(LevelInfoLayer::confirmDelete));
        rightMenu->addChild(deleteBtn);
        deleteBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
        rightBtnY -= 50.0f;
    }

    cocos2d::CCSprite* updateSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    CCMenuItemSpriteExtra* updateBtn = CCMenuItemSpriteExtra::create(updateSpr, nullptr, this, menu_selector(LevelInfoLayer::onUpdate));
    rightMenu->addChild(updateBtn);
    updateBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
    rightBtnY -= 50.0f;

    cocos2d::CCSprite* infoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSpr, nullptr, this, menu_selector(LevelInfoLayer::onInfo));
    rightMenu->addChild(infoBtn);
    infoBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
    rightBtnY -= 50.0f;

    cocos2d::CCSprite* leaderboardSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_levelLeaderboardBtn_001.png");
    CCMenuItemSpriteExtra* leaderboardBtn = CCMenuItemSpriteExtra::create(leaderboardSpr, nullptr, this, menu_selector(LevelInfoLayer::onLevelLeaderboard));
    rightMenu->addChild(leaderboardBtn);
    leaderboardBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
    rightBtnY -= 50.0f;

    cocos2d::CCSprite* likeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_like2Btn_001.png");
    m_likeBtn = CCMenuItemSpriteExtra::create(likeSpr, nullptr, this, menu_selector(LevelInfoLayer::onLike));
    rightMenu->addChild(m_likeBtn);
    m_likeBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
    rightBtnY -= 50.0f;

    cocos2d::CCSprite* starRateSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starBtn_001.png");
    m_starRateBtn = CCMenuItemSpriteExtra::create(starRateSpr, nullptr, this, menu_selector(LevelInfoLayer::onRateStars));
    rightMenu->addChild(m_starRateBtn);
    m_starRateBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
    rightBtnY -= 50.0f;

    if (m_level->m_demon.random - m_level->m_demon.seed == 1) {
        cocos2d::CCSprite* demonRateSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_rateDiffBtn_001.png");
        m_demonRateBtn = CCMenuItemSpriteExtra::create(demonRateSpr, nullptr, this, menu_selector(LevelInfoLayer::onRateDemon));
        rightMenu->addChild(m_demonRateBtn);
        m_demonRateBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
        rightBtnY -= 50.0f;
    }

    GameManager* GM = GameManager::sharedState();
    if (GM->m_hasRP.random - GM->m_hasRP.seed > 0 || GM->m_hasDRP) {
        cocos2d::CCSprite* modRateSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starBtnMod_001.png");
        CCMenuItemSpriteExtra* modRateBtn = CCMenuItemSpriteExtra::create(modRateSpr, nullptr, this, menu_selector(LevelInfoLayer::onRateStarsMod));
        rightMenu->addChild(modRateBtn);
        modRateBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
        rightBtnY -= 50.0f;
    }

    GJAccountManager* AM = GJAccountManager::sharedState();
    int currentUserID = GM->m_playerUserID.random - GM->m_playerUserID.seed;
    int levelUserID = m_level->m_userID.random - m_level->m_userID.seed;
    int currentAccountID = AM->m_accountID - AM->m_unkInt1;
    int levelAccountID = m_level->m_accountID.random - m_level->m_accountID.seed;

    if (currentUserID == levelUserID || (AM->m_accountID != AM->m_unkInt1 && currentAccountID == levelAccountID) || (GM->m_hasRP.random - GM->m_hasRP.seed > 1)) {
        cocos2d::CCSprite* serverDeleteSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteServerBtn_001.png");
        CCMenuItemSpriteExtra* serverDeleteBtn = CCMenuItemSpriteExtra::create(serverDeleteSpr, nullptr, this, menu_selector(LevelInfoLayer::confirmOwnerDelete));
        rightMenu->addChild(serverDeleteBtn);
        serverDeleteBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
        rightBtnY -= 50.0f;
    }

    if (m_level->m_password.random - m_level->m_password.seed != 0) {
        std::string cloneFrame = (m_level->m_password.random - m_level->m_password.seed == 1) ? "GJ_duplicateBtn_001.png" : "GJ_duplicateLockedBtn_001.png";
        cocos2d::CCSprite* cloneSpr = cocos2d::CCSprite::createWithSpriteFrameName(cloneFrame.c_str());
        m_cloneBtn = CCMenuItemSpriteExtra::create(cloneSpr, nullptr, this, menu_selector(LevelInfoLayer::tryCloneLevel));
        rightMenu->addChild(m_cloneBtn);
        m_cloneBtn->setPosition(cocos2d::CCPoint(0.0f, rightBtnY));
    }

    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create();
    this->addChild(backMenu, 10);
    backMenu->setPosition(cocos2d::CCPoint(director->getScreenLeft() + 25.0f, director->getScreenTop() - 22.0f));

    cocos2d::CCSprite* backSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(backSpr, nullptr, this, menu_selector(LevelInfoLayer::onBack));
    backMenu->addChild(backBtn);

    cocos2d::CCMenu* bottomLeftMenu = cocos2d::CCMenu::create();
    this->addChild(bottomLeftMenu, 10);
    bottomLeftMenu->setPosition(cocos2d::CCPoint(director->getScreenLeft() + 30.0f, director->getScreenBottom() + 30.0f));

    cocos2d::CCSprite* levelInfoSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* levelInfoBtn = CCMenuItemSpriteExtra::create(levelInfoSpr, nullptr, this, menu_selector(LevelInfoLayer::onLevelInfo));
    bottomLeftMenu->addChild(levelInfoBtn);
    levelInfoBtn->setPosition(cocos2d::CCPoint(0.0f, 0.0f));

    std::string heartFrame = m_level->m_levelFavorited ? "gj_heartOn_001.png" : "gj_heartOff_001.png";
    cocos2d::CCSprite* favSpr = cocos2d::CCSprite::createWithSpriteFrameName(heartFrame.c_str());
    CCMenuItemSpriteExtra* favBtn = CCMenuItemSpriteExtra::create(favSpr, nullptr, this, menu_selector(LevelInfoLayer::onFavorite));
    bottomLeftMenu->addChild(favBtn);
    favBtn->setPosition(cocos2d::CCPoint(40.0f, 0.0f));

    cocos2d::CCSprite* moveTopSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_moveTopBtn_001.png");
    CCMenuItemSpriteExtra* moveTopBtn = CCMenuItemSpriteExtra::create(moveTopSpr, nullptr, this, menu_selector(LevelInfoLayer::confirmMoveToTop));
    bottomLeftMenu->addChild(moveTopBtn);
    moveTopBtn->setPosition(cocos2d::CCPoint(80.0f, 0.0f));

    cocos2d::CCSprite* moveBottomSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_moveBottomBtn_001.png");
    CCMenuItemSpriteExtra* moveBottomBtn = CCMenuItemSpriteExtra::create(moveBottomSpr, nullptr, this, menu_selector(LevelInfoLayer::confirmMoveToBottom));
    bottomLeftMenu->addChild(moveBottomBtn);
    moveBottomBtn->setPosition(cocos2d::CCPoint(120.0f, 0.0f));

    cocos2d::CCSprite* folderSpr = cocos2d::CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
    CCMenuItemSpriteExtra* folderBtn = CCMenuItemSpriteExtra::create(folderSpr, nullptr, this, menu_selector(LevelInfoLayer::onSetFolder));
    bottomLeftMenu->addChild(folderBtn);
    folderBtn->setPosition(cocos2d::CCPoint(160.0f, 0.0f));

    m_folderLabel = cocos2d::CCLabelBMFont::create(cocos2d::CCString::createWithFormat("%i", m_level->m_levelFolder)->getCString(), "bigFont.fnt");
    m_folderLabel->setScale(0.55f);
    m_folderLabel->setPosition(folderBtn->getPosition());
    bottomLeftMenu->addChild(m_folderLabel, 1);
    m_folderLabel->limitLabelWidth(24.0f, 0.55f, 0.0f);

    cocos2d::CCSprite* addToListSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_listAddBtn_001.png");
    addToListSpr->setScale(0.9f);
    CCMenuItemSpriteExtra* addToListBtn = CCMenuItemSpriteExtra::create(addToListSpr, nullptr, this, menu_selector(LevelInfoLayer::onAddToList));
    bottomLeftMenu->addChild(addToListBtn);
    addToListBtn->setPosition(folderBtn->getPosition() + cocos2d::CCPoint(0.0f, 35.0f));

    if (m_level->m_dailyID.random - m_level->m_dailyID.seed > 0 || m_level->m_gauntletLevel) {
        folderBtn->setVisible(false);
        folderBtn->setEnabled(false);
        moveTopBtn->setVisible(false);
        moveTopBtn->setEnabled(false);
        moveBottomBtn->setVisible(false);
        moveBottomBtn->setEnabled(false);
        favBtn->setVisible(false);
        favBtn->setEnabled(false);
        addToListBtn->setVisible(false);
        addToListBtn->setEnabled(false);
    }

    m_songWidget = CustomSongWidget::create(
        m_level->m_songObject,
        MusicDownloadManager::sharedState(),
        false,
        true,
        true,
        true,
        false
    );
    this->addChild(m_songWidget, 10);
    m_songWidget->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, director->getScreenBottom() + 35.0f));

    cocos2d::CCMenu* optionsMenu = cocos2d::CCMenu::create();
    this->addChild(optionsMenu, 10);
    optionsMenu->setPosition(cocos2d::CCPoint(director->getScreenRight() - 30.0f, director->getScreenBottom() + 30.0f));

    cocos2d::CCSprite* optionsSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    CCMenuItemSpriteExtra* optionsBtn = CCMenuItemSpriteExtra::create(optionsSpr, nullptr, this, menu_selector(LevelInfoLayer::onLevelOptions));
    optionsMenu->addChild(optionsBtn);

    cocos2d::CCSprite* garageSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_garageBtn_001.png");
    garageSpr->setScale(0.8f);
    CCMenuItemSpriteExtra* garageBtn = CCMenuItemSpriteExtra::create(garageSpr, nullptr, this, menu_selector(LevelInfoLayer::onGarage));
    optionsMenu->addChild(garageBtn);
    garageBtn->setPosition(cocos2d::CCPoint(0.0f, 45.0f));

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backBtn);
        cocos2d::CCSprite* controllerSpr = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        m_playSprite->addChild(controllerSpr);
        controllerSpr->setPosition(cocos2d::CCPoint(m_playSprite->getContentSize().width * 0.5f, m_playSprite->getContentSize().height * 0.5f - 40.0f));
    }

    if ((m_level->m_savedTime || m_level->m_savedPoints) && m_level->m_levelType != GJLevelType::Editor &&
        m_level->m_demon.random - m_level->m_demon.seed == 1 && PlatformToolbox::isNetworkAvailable()) {
        if (m_level->m_savedTime) {
            GameLevelManager::sharedState()->getLevelLeaderboard(m_level, LevelLeaderboardType::Friends, LevelLeaderboardMode::Time);
        }
        if (m_level->m_savedPoints) {
            GameLevelManager::sharedState()->getLevelLeaderboard(m_level, LevelLeaderboardType::Friends, LevelLeaderboardMode::Points);
        }
    }

    this->setKeypadEnabled(true);
    this->setKeyboardEnabled(true);

    return true;
}

void LevelInfoLayer::keyBackClicked() {
    onBack(nullptr);
}

void LevelInfoLayer::levelDeleteFailed(int id) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "Level delete failed, please try again later.");
        alert->show();
        m_circle->setVisible(false);
    }
}

void LevelInfoLayer::levelDeleteFinished(int id) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Level Deleted", "The level has been removed from the server.");
        alert->show();
        m_circle->setVisible(false);
    }
}

void LevelInfoLayer::levelDownloadFailed(int response) {
    m_circle->setVisible(false);
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "Level download failed, please try again later.");
    alert->m_scene = AppDelegate::get();
    alert->show();
}

void LevelInfoLayer::levelDownloadFinished(GJGameLevel* level) {
    if (!m_level || m_level->m_levelID.random - m_level->m_levelID.seed == level->m_levelID.random - level->m_levelID.seed) {
        m_circle->setVisible(false);
        if (m_level) {
            m_level->release();
        }
        m_level = level;
        m_level->retain();
        m_playBtnMenu->setVisible(true);
        updateLabelValues();
        if (m_cloneBtn) {
            m_cloneBtn->setVisible(m_level->m_password.random != m_level->m_password.seed);
            if (m_cloneBtn->isVisible()) {
                std::string frame = "GJ_duplicateBtn_001.png";
                if (m_level->m_password.random - m_level->m_password.seed != 1) {
                    frame = "GJ_duplicateLockedBtn_001.png";
                }
                m_cloneBtn->getNormalImage()->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame.c_str()));
            }
            m_cloneBtn->setEnabled(m_cloneBtn->isVisible());
        }
    }
}

void LevelInfoLayer::levelUpdateFailed(int response) {
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "Level update failed, please try again later.");
    alert->m_scene = AppDelegate::get();
    alert->show();
    m_circle->setVisible(false);
}

void LevelInfoLayer::levelUpdateFinished(GJGameLevel* level, UpdateResponse response) {
    if (!m_level || m_level->m_levelID.random - m_level->m_levelID.seed == level->m_levelID.random - m_level->m_levelID.seed) {
        if (m_noUpdateAlert) {
            levelDownloadFinished(level);
        } else {
            showUpdateAlert(response);
            levelDownloadFinished(level);
        }
        m_noUpdateAlert = false;
    }
}

void LevelInfoLayer::likedItem(LikeItemType type, int id, bool liked) {
    if (id == m_level->m_levelID.random - m_level->m_levelID.seed) {
        updateSideButtons();
        if (liked) {
            incrementLikes();
        } else {
            incrementDislikes();
        }
    }
}

void LevelInfoLayer::loadLevelStep() {
    cocos2d::CCArray* children = m_playScene->getChildren();
    PlayLayer* playLayer = static_cast<PlayLayer*>(children->objectAtIndex(0));
    playLayer->processCreateObjectsFromSetup();
    m_progressTimer->setPercentage(playLayer->m_loadingProgress * 100.0f);
    if (playLayer->m_loadingProgress < 1.0f) {
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(LevelInfoLayer::loadLevelStep)),
            nullptr
        );
        this->runAction(seq);
    } else {
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(LevelInfoLayer::playStep4)),
            nullptr
        );
        this->runAction(seq);
    }
}

void LevelInfoLayer::numberInputClosed(NumberInputLayer* layer) {
    int inputVal = atoi(layer->m_inputString.c_str());
    int password = m_level->m_password.random - m_level->m_password.seed;
    if (inputVal + 1000000 == password || inputVal + 10000 == password) {
        confirmClone(nullptr);
        return;
    }
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "Incorrect password!");
    alert->show();
    ++m_level->m_failedPasswordAttempts;
}

void LevelInfoLayer::onAddToList(cocos2d::CCObject* sender) {
    GJSearchObject* searchObj = GJSearchObject::create(SearchType::MyLevels);
    searchObj->m_searchIsOverlay = true;
    LevelBrowserLayer* browser = LevelBrowserLayer::create(searchObj);
    browser->m_delegate = this;
    browser->show();
}

void LevelInfoLayer::onBack(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        setKeypadEnabled(false);
        if (m_level->m_gauntletLevel) {
            cocos2d::CCScene* scene = GauntletLayer::scene(static_cast<GauntletType>(GameManager::sharedState()->m_currentGauntlet));
            cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
            cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
        } else {
            GameManager::sharedState()->safePopScene();
        }
    }
}

void LevelInfoLayer::onClone(cocos2d::CCObject* sender) {
    if (!shouldDownloadLevel() && !m_isBusy) {
        setKeypadEnabled(false);
        m_isBusy = true;
        GameManager::sharedState()->m_sceneEnum = 2;
        GJGameLevel* newLevel = GameLevelManager::sharedState()->createNewLevel();
        newLevel->copyLevelInfo(m_level);

        int originalLevel;
        if (m_level->m_originalLevel.random == m_level->m_originalLevel.seed) {
            int levelID = m_level->m_levelID.random - m_level->m_levelID.seed;
            originalLevel = levelID;
        } else {
            originalLevel = m_level->m_originalLevel.random - m_level->m_originalLevel.seed;
        }
        newLevel->m_originalLevel.value = originalLevel;

        int randVal = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
        newLevel->m_originalLevel.seed = randVal;
        newLevel->m_originalLevel.random = originalLevel + randVal;

        cocos2d::CCScene* scene = EditLevelLayer::scene(newLevel);
        cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
    }
}

void LevelInfoLayer::onDelete(cocos2d::CCObject* sender) {
    m_playBtnMenu->setEnabled(false);
    GameLevelManager::sharedState()->deleteLevel(m_level);
    onBack(nullptr);
    m_isBusy = true;
}

void LevelInfoLayer::onEnterTransitionDidFinish() {
    cocos2d::CCLayer::onEnterTransitionDidFinish();
    m_enterTransitionFinished = true;
}

void LevelInfoLayer::onFavorite(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        bool wasFavorited = m_level->m_levelFavorited;
        m_level->m_levelFavorited = !wasFavorited;

        cocos2d::CCNode* normalImage = static_cast<CCMenuItemSpriteExtra*>(sender)->getNormalImage();
        std::string frame = wasFavorited ? "gj_heartOff_001.png" : "gj_heartOn_001.png";
        normalImage->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame.c_str()));
    }
}

void LevelInfoLayer::onGarage(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        cocos2d::CCScene* scene = GJGarageLayer::scene();
        cocos2d::CCTransitionMoveInT* transition = cocos2d::CCTransitionMoveInT::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(transition);
        GameManager::sharedState()->m_ropeGarageEnter = true;
    }
}

void LevelInfoLayer::onInfo(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        InfoLayer* infoLayer = InfoLayer::create(m_level, nullptr, nullptr);
        infoLayer->show();
    }
}

void LevelInfoLayer::onLevelInfo(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        int attempts = m_level->m_attempts.random - m_level->m_attempts.seed;
        int jumps = m_level->m_jumps.random - m_level->m_jumps.seed;
        int normalPercent = GJGameLevel::getNormalPercent(m_level);

        std::string stats = cocos2d::CCString::createWithFormat(
            "<cy>%s</c>\n"
            "<cg>Total Attempts</c>: %i\n"
            "<cl>Total Jumps</c>: %i\n"
            "<cp>Normal</c>: %i%%\n"
            "<co>Practice</c>: %i%%",
            m_level->m_levelName.c_str(),
            attempts,
            jumps,
            normalPercent,
            m_level->m_practicePercent
        )->getCString();

        int newNormalPercent = m_level->m_newNormalPercent2.random - m_level->m_newNormalPercent2.seed;
        if (normalPercent != newNormalPercent) {
            stats += cocos2d::CCString::createWithFormat("\n<cj>Leaderboard</c>: %i%%", newNormalPercent)->getCString();
        }

        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Level Stats", stats.c_str());
        alert->show();
    }
}

void LevelInfoLayer::onLevelLeaderboard(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        LevelLeaderboardType type = static_cast<LevelLeaderboardType>(GameManager::sharedState()->getIntGameVariable("0098"));
        LevelLeaderboardMode mode = static_cast<LevelLeaderboardMode>(GameManager::sharedState()->getIntGameVariable("0164"));
        LevelLeaderboard* leaderboard = LevelLeaderboard::create(m_level, type, mode);
        leaderboard->show();
    }
}

void LevelInfoLayer::onLevelOptions(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        if (m_level->m_levelType != GJLevelType::SearchResult) {
            if (!m_level->m_levelString.empty()) {
                GameLevelOptionsLayer* optionsLayer = GameLevelOptionsLayer::create(m_level);
                optionsLayer->show();
            }
        }
    }
}

void LevelInfoLayer::onLike(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        GameManager* GM = GameManager::sharedState();
        if (GM->m_playerUserID.random == GM->m_playerUserID.seed) {
            GameLevelManager::sharedState()->updateUserScore();
        }
        LikeItemLayer* likeLayer = LikeItemLayer::create(LikeItemType::Level, m_level->m_levelID.random - m_level->m_levelID.seed, 0);
        likeLayer->m_likeDelegate = this;
        likeLayer->show();
    }
}

void LevelInfoLayer::onOwnerDelete(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        GameLevelManager::sharedState()->m_levelDeleteDelegate = this;
        GameLevelManager::sharedState()->deleteServerLevel(m_level->m_levelID.random - m_level->m_levelID.seed);
        m_circle->setVisible(true);
    }
}

void LevelInfoLayer::onPlay(cocos2d::CCObject* sender) {
    if (m_isBusy || !m_enterTransitionFinished) {
        return;
    }
    if (shouldDownloadLevel()) {
        downloadLevel();
        return;
    }
    GameManager* GM = GameManager::sharedState();
    if (GM->getGameVariable("0083")) {
        // Skip song warning
    } else if (m_level->m_songID && !m_level->m_showedSongWarning) {
        if (!MusicDownloadManager::sharedState()->isSongDownloaded(m_level->m_songID)) {
            showSongWarning();
            return;
        }
    }

    if (m_level->m_coins <= 0
        || m_level->m_coinsVerified.random != m_level->m_coinsVerified.seed
        || GM->getGameVariable("0063")) {
        GameManager* gm2 = GameManager::sharedState();
        if (gm2->getGameVariable("0082")
            || (m_level->m_objectCount.random - m_level->m_objectCount.seed) <= 40000
            || m_level->m_highObjectsEnabled) {
            if (!m_isBusy) {
                setKeypadEnabled(false);
                m_isBusy = true;
                FMODAudioEngine::sharedEngine()->stopAllMusic(true);
                FMODAudioEngine::sharedEngine()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);
                GameManager::sharedState()->m_loadingLevel = true;

                cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(0.0f),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(LevelInfoLayer::playStep2)),
                    nullptr
                );
                this->runAction(seq);

                cocos2d::ccColor3B color1 = { 0, 0, 0 };
                cocos2d::ccColor3B color2 = { 0, 0, 0 };
                cocos2d::ccColor3B color3 = { 0, 0, 0 };
                if (m_level->m_dailyID.random - m_level->m_dailyID.seed <= 100000) {
                    if (m_level->m_gauntletLevel) {
                        color1 = { 23, 23, 23 };
                        color2 = { 23, 23, 23 };
                        color3 = { 23, 23, 23 };
                    }
                } else {
                    color1 = { 43, 43, 43 };
                    color2 = { 43, 43, 43 };
                    color3 = { 43, 43, 43 };
                }

                cocos2d::CCSprite* circle1 = cocos2d::CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
                circle1->setColor(color1);
                circle1->setScale(1.88f);
                circle1->setPosition(m_playSprite->getContentSize().width * 0.5f, m_playSprite->getContentSize().height * 0.5f);
                m_playSprite->addChild(circle1, -5);

                cocos2d::CCSprite* circle2 = cocos2d::CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
                circle2->setColor(color2);
                circle2->setScale(1.63f);
                circle2->setPosition(m_playSprite->getContentSize().width * 0.5f, m_playSprite->getContentSize().height * 0.5f);
                m_playSprite->addChild(circle2, -2);

                cocos2d::CCSprite* circle3 = cocos2d::CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
                circle3->setColor(color3);
                circle3->setScale(1.75f);
                circle3->setPosition(m_playSprite->getContentSize().width * 0.5f, m_playSprite->getContentSize().height * 0.5f);
                m_playSprite->addChild(circle3, -4);

                m_playSprite->setColor(cocos2d::ccc3(125, 125, 125));

                cocos2d::CCSprite* progressSprite = cocos2d::CCSprite::createWithSpriteFrameName("d_circle_01_001.png");
                progressSprite->setColor(cocos2d::ccc3(130, 55, 0));
                m_progressTimer = cocos2d::CCProgressTimer::create(progressSprite);
                m_progressTimer->setPosition(m_playSprite->getContentSize().width * 0.5f, m_playSprite->getContentSize().height * 0.5f);
                m_progressTimer->setScale(1.75f);
                m_progressTimer->setPercentage(0.0f);
                m_playSprite->addChild(m_progressTimer, -3);

                m_playBtnMenu->setTouchEnabled(false);
                m_songWidget->m_buttonMenu->setTouchEnabled(false);
            }
        } else {
            int objectCount = m_level->m_objectCount.random - m_level->m_objectCount.seed;
            std::string message;
            if (objectCount <= 80000) {
                message = "This level has a <co>high object</c> count and can be <cr>unstable</c> on some devices. This may effect <cg>performance</c>, <cl>load time</c> etc.";
            } else {
                message = "This level has a <cr>VERY HIGH object</c> count and can be <cr>unstable</c> on some devices. This may effect <cg>performance</c>, <cl>load time</c> etc.";
            }
            FLAlertLayer* alert = FLAlertLayer::create(this, "High Objects", message.c_str());
            alert->setTag(10);
            alert->show();
        }
    } else {
        GM->setGameVariable("0063", true);
        FLAlertLayer* alert = FLAlertLayer::create(
            this,
            "Unverified Coins",
            "This level contains <cr>unverified</c> user coins (bronze).\n"
            "The coins will not count until they become <cg>verified</c> (silver)."
        );
        alert->show();
    }
}

void LevelInfoLayer::onPlayReplay(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        if (shouldDownloadLevel()) {
            downloadLevel();
        } else {
            FMODAudioEngine::sharedEngine()->stopAllMusic(true);
            FMODAudioEngine::sharedEngine()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);
            GameManager::sharedState()->m_sceneEnum = 3;
            cocos2d::CCScene* scene = PlayLayer::scene(m_level, true, nullptr);
            cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
            cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
        }
    }
}

void LevelInfoLayer::onRate(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        if (GameManager::sharedState()->m_showedRateDiffDialog) {
            RateLevelLayer* layer = RateLevelLayer::create(m_level->m_levelID.random - m_level->m_levelID.seed);
            layer->m_delegate = this;
            layer->show();
        } else {
            GameManager::sharedState()->m_showedRateDiffDialog = true;
            FLAlertLayer* alert = FLAlertLayer::create(
                this,
                "Rate Difficulty",
                "Please <cg>rate</c> the <cl>difficulty</c> as fairly as possible. \n"
                "<cy>(Demon and Auto levels are manually verified before accepted)</c>"
            );
            alert->setTag(7);
            alert->show();
        }
    }
}

void LevelInfoLayer::onRateDemon(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        RateDemonLayer* layer = RateDemonLayer::create(m_level->m_levelID.random - m_level->m_levelID.seed);
        layer->m_delegate = this;
        layer->show();
    }
}

void LevelInfoLayer::onRateStars(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        GameManager* GM = GameManager::sharedState();
        if (GM->m_playerUserID.random == GM->m_playerUserID.seed) {
            GameLevelManager::sharedState()->updateUserScore();
        }
        if (GM->m_showedRateStarDialog) {
            bool isPlatformer = GJGameLevel::isPlatformer(m_level);
            RateStarsLayer* layer = RateStarsLayer::create(m_level->m_levelID.random - m_level->m_levelID.seed, isPlatformer, false);
            layer->m_delegate = this;
            layer->show();
        } else {
            GM->m_showedRateStarDialog = true;
            FLAlertLayer* alert = FLAlertLayer::create(
                this,
                "Rating guide",
                "<co>Auto:</c> 1 Star or Moon\n"
                "<cl>Easy:</c> 2 Stars or Moons\n"
                "<cg>Normal:</c> 3 Stars or Moons\n"
                "<cy>Hard:</c> 4 to 5 Stars or Moons\n"
                "<cr>Harder:</c> 6 to 7 Stars or Moons\n"
                "<ca>Insane:</c> 8 to 9 Stars or Moons\n"
                "<cp>Demon:</c> 10 Stars or Moons\n"
                "(These values are not assigned before a rating is manually given by RobTop.)"
            );
            alert->setTag(8);
            alert->show();
        }
    }
}

void LevelInfoLayer::onRateStarsMod(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        GJAccountManager* AM = GJAccountManager::sharedState();
        void* layer = nullptr;
        if (AM->m_accountID - AM->m_unkInt1 == 2795) {
            layer = GJRateLevelLayer::create(m_level);
        } else {
            bool isPlatformer = GJGameLevel::isPlatformer(m_level);
            layer = RateStarsLayer::create(m_level->m_levelID.random - m_level->m_levelID.seed, isPlatformer, true);
        }
        reinterpret_cast<cocos2d::CCNode*>(layer)->show();
    }
}

void LevelInfoLayer::onSetFolder(cocos2d::CCObject* sender) {
    if (m_level && !m_isBusy) {
        SetFolderPopup* popup = SetFolderPopup::create(m_level->m_levelFolder, false, "Set Folder");
        popup->m_delegate = this;
        popup->show();
    }
}

void LevelInfoLayer::onUpdate(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        if (shouldDownloadLevel()) {
            downloadLevel();
        } else {
            if (GameLevelManager::sharedState()->isUpdateValid(m_level->m_levelID.random - m_level->m_levelID.seed)) {
                showUpdateAlert(UpdateResponse::UpToDate);
            } else {
                GameLevelManager::sharedState()->m_levelUpdateDelegate = this;
                GameLevelManager::sharedState()->updateLevel(m_level);
            }
        }
    }
}

void LevelInfoLayer::onViewProfile(cocos2d::CCObject* sender) {
    if (!m_isBusy && m_level->m_accountID.random - m_level->m_accountID.seed > 0) {
        GameManager::sharedState()->setGameVariable("0047", true);
        ProfilePage* page = ProfilePage::create(m_level->m_accountID.random - m_level->m_accountID.seed, false);
        page->show();
    }
}

void LevelInfoLayer::playStep2() {
    FMODAudioEngine::sharedEngine()->loadMusic(m_level->getAudioFileName(), 1.0f, 0.0f, 1.0f, true, 0, 0, 0);

    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(LevelInfoLayer::playStep3)),
        nullptr
    );
    this->runAction(seq);
}

void LevelInfoLayer::playStep3() {
    GameManager::sharedState()->m_sceneEnum = 3;
    m_playScene = PlayLayer::scene(m_level, false, true);
    m_playScene->retain();

    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(LevelInfoLayer::loadLevelStep)),
        nullptr
    );
    this->runAction(seq);
    dwTickStart = clock();
}

void LevelInfoLayer::playStep4() {
    dwTickEnd = clock();
    dwDuration = dwTickEnd - dwTickStart;
    GameManager::sharedState()->m_loadingLevel = false;
    cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, m_playScene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
    m_playScene->release();
    m_playScene = nullptr;
}

cocos2d::CCScene* LevelInfoLayer::scene(GJGameLevel* level, bool challenge) {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    AppDelegate::get()->m_scene = scene;
    LevelInfoLayer* layer = LevelInfoLayer::create(level, challenge);
    scene->addChild(layer);
    return scene;
}

void LevelInfoLayer::setIDPopupClosed(SetIDPopup* popup, int value) {
    if (value < 0) {
        value = 0;
    } else if (value >= 1000) {
        value = 999;
    }
    m_level->m_levelFolder = value;
    m_folderLabel->setString(cocos2d::CCString::createWithFormat("%i", value)->getCString());
    m_folderLabel->limitLabelWidth(24.0f, 0.55f, 0.0f);
}

void LevelInfoLayer::setupLevelInfo() {
    m_icons = cocos2d::CCArray::create();
    m_icons->retain();

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();

    cocos2d::CCSprite* timeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
    cocos2d::CCSprite* downloadsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
    m_orbsIcon = cocos2d::CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
    m_likesIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_likesIcon_001.png");

    m_lengthLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_lengthLabel->setScale(0.5f);
    m_lengthLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));

    m_exactLengthLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_exactLengthLabel->setScale(0.5f);
    m_exactLengthLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));

    m_downloadsLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_downloadsLabel->setScale(0.5f);
    m_downloadsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));

    m_likesLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_likesLabel->setScale(0.5f);
    m_likesLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));

    m_orbsLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_orbsLabel->setScale(0.5f);
    m_orbsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));

    m_icons->addObject(timeIcon);
    m_icons->addObject(downloadsIcon);
    m_icons->addObject(m_orbsIcon);
    m_icons->addObject(m_likesIcon);
    m_icons->addObject(m_downloadsLabel);
    m_icons->addObject(m_lengthLabel);
    m_icons->addObject(m_exactLengthLabel);
    m_icons->addObject(m_likesLabel);
    m_icons->addObject(m_orbsLabel);

    this->addChild(timeIcon);
    this->addChild(downloadsIcon);
    this->addChild(m_orbsIcon);
    this->addChild(m_likesIcon);
    this->addChild(m_downloadsLabel);
    this->addChild(m_lengthLabel);
    this->addChild(m_exactLengthLabel);
    this->addChild(m_likesLabel);
    this->addChild(m_orbsLabel);

    bool hasStars = m_level->m_stars.random - m_level->m_stars.seed > 0;
    bool isPlatformer = GJGameLevel::isPlatformer(m_level);

    float baseX = winSize.width * 0.5f + 80.0f;
    float baseY = winSize.height * 0.5f + 51.0f;
    float spacing = hasStars ? 28.0f : 30.0f;
    if (hasStars) {
        baseY += 10.0f;
    }

    m_iconOffset = 10.0f;

    downloadsIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    downloadsIcon->setPosition(cocos2d::CCPoint(baseX, baseY + spacing));
    m_downloadsLabel->setPosition(cocos2d::CCPoint(baseX + 33.0f, downloadsIcon->getPositionY()));

    m_likesIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_likesIcon->setPosition(cocos2d::CCPoint(baseX, baseY + 1.0f));
    m_likesLabel->setPosition(cocos2d::CCPoint(baseX + 33.0f, m_likesIcon->getPositionY()));

    timeIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    timeIcon->setPosition(cocos2d::CCPoint(baseX, baseY - spacing));
    float lengthOffsetY = isPlatformer ? 6.0f : 0.0f;
    m_lengthLabel->setPosition(cocos2d::CCPoint(baseX + 33.0f, timeIcon->getPositionY() + lengthOffsetY));
    m_exactLengthLabel->setPosition(m_lengthLabel->getPosition() + cocos2d::CCPoint(0.0f, -14.0f));

    m_orbsIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_orbsIcon->setPosition(cocos2d::CCPoint(baseX + 1.0f, baseY - spacing * 2.0f));
    m_orbsLabel->setPosition(cocos2d::CCPoint(baseX + 33.0f, m_orbsIcon->getPositionY()));

    m_orbsIcon->setVisible(hasStars);
    m_orbsLabel->setVisible(hasStars);

    m_basePosition = cocos2d::CCPoint(winSize.width * 0.5f - 100.0f, baseY + 5.0f);

    m_difficultySprite = GJDifficultySprite::create(1, GJDifficultyName::Long);
    this->addChild(m_difficultySprite);
    m_difficultySprite->setPosition(m_basePosition);

    std::string starIconFrame = isPlatformer ? "moon_small01_001.png" : "star_small01_001.png";
    m_starsIcon = cocos2d::CCSprite::createWithSpriteFrameName(starIconFrame.c_str());
    this->addChild(m_starsIcon);
    m_starsIcon->setPosition(m_basePosition + cocos2d::CCPoint(8.0f, -30.0f));

    m_starsLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    this->addChild(m_starsLabel);
    m_starsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_starsLabel->setPosition(m_starsIcon->getPosition() + cocos2d::CCPoint(-8.0f, 0.0f));
    m_starsLabel->setScale(0.4f);
    m_baseStarsY = m_starsLabel->getPositionY();

    if (hasStars) {
        if (m_level->m_dailyID.random - m_level->m_dailyID.seed > 0 || m_level->m_gauntletLevel) {
            cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
            this->addChild(diamondIcon);

            int baseDiamonds = GameStatsManager::sharedState()->getBaseDiamonds(m_level->m_stars.random - m_level->m_stars.seed);
            int awardedDiamonds = GameStatsManager::sharedState()->getAwardedDiamondsForLevel(m_level);
            std::string diamondText = cocos2d::CCString::createWithFormat("%i/%i", awardedDiamonds, baseDiamonds)->getCString();

            cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create(diamondText.c_str(), "bigFont.fnt");
            this->addChild(diamondLabel);
            diamondLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
            diamondLabel->setScale(0.35f);

            float labelWidth = diamondLabel->getContentSize().width * diamondLabel->getScale();
            float posX = labelWidth * 0.5f + 2.0f;
            float posY = (m_level->m_stars.random - m_level->m_stars.seed == 10) ? -48.5f : -44.5f;
            diamondIcon->setPosition(m_basePosition + cocos2d::CCPoint(posX, posY));
            diamondLabel->setPosition(diamondIcon->getPosition() + cocos2d::CCPoint(-8.0f, 0.0f));

            if (baseDiamonds == awardedDiamonds) {
                diamondLabel->setColor(cocos2d::ccc3(255, 255, 50));
            }
        }
    }

    if (m_challenge) {
        for (unsigned int i = 0; i < m_icons->count(); ++i) {
            cocos2d::CCObject* obj = m_icons->objectAtIndex(i);
            static_cast<cocos2d::CCNode*>(obj)->setVisible(false);
        }
        m_orbsIcon->setVisible(false);
        m_orbsLabel->setVisible(false);
    }

    updateLabelValues();
}

void LevelInfoLayer::setupPlatformerStats() {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();

    std::string bestTimeText = "No Best Time";
    float posY = -10.0f;

    int bestTime = m_level->m_bestTime;
    if (bestTime > 0) {
        int hours = bestTime / 1000 / 3600;
        int minutes = bestTime / 1000 / 60 % 60;
        int seconds = bestTime / 1000 % 60;
        int millis = bestTime % 1000;

        if (minutes || hours) {
            if (hours) {
                bestTimeText = cocos2d::CCString::createWithFormat("%02d:%02d:%02d.%03d", hours, minutes, seconds, millis)->getCString();
            } else {
                bestTimeText = cocos2d::CCString::createWithFormat("%02d:%02d.%03d", minutes, seconds, millis)->getCString();
            }
        } else {
            bestTimeText = cocos2d::CCString::createWithFormat("%i.%03d", seconds, millis)->getCString();
        }
        bestTimeText = cocos2d::CCString::createWithFormat("Best Time: %s", bestTimeText.c_str())->getCString();
        posY = 0.0f;
    }

    cocos2d::CCLabelBMFont* bestTimeLabel = cocos2d::CCLabelBMFont::create(bestTimeText.c_str(), "bigFont.fnt");
    this->addChild(bestTimeLabel, 3);
    bestTimeLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, posY + 145.0f));
    bestTimeLabel->limitLabelWidth(200.0f, 0.6f, 0.0f);

    int bestPoints = m_level->m_bestPoints;
    if (bestPoints) {
        std::string pointsText = "Best Points: " + GameToolbox::pointsToString(bestPoints);
        cocos2d::CCLabelBMFont* pointsLabel = cocos2d::CCLabelBMFont::create(pointsText.c_str(), "bigFont.fnt");
        this->addChild(pointsLabel, 3);
        pointsLabel->setPosition(bestTimeLabel->getPosition() + cocos2d::CCPoint(0.0f, -28.0f));
        pointsLabel->limitLabelWidth(200.0f, 0.6f, 0.0f);
    }
}

void LevelInfoLayer::setupProgressBars() {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = director->getWinSize();

    cocos2d::CCSprite* normalBarBg = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    normalBarBg->setColor(cocos2d::ccc3(0, 0, 0));
    normalBarBg->setOpacity(125);
    normalBarBg->setScale(0.7f);
    this->addChild(normalBarBg, 3);
    normalBarBg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, director->getScreenBottom() + 136.0f));

    cocos2d::CCSprite* normalBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    normalBar->setScaleX(0.992f);
    normalBar->setScaleY(0.86f);
    normalBar->setColor(cocos2d::ccc3(0, 255, 0));
    normalBarBg->addChild(normalBar, 1);
    normalBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    float barWidth = normalBar->getContentSize().width - (normalBar->getContentSize().width * 0.992f);
    normalBar->setPosition(cocos2d::CCPoint(barWidth * 0.5f, normalBarBg->getContentSize().height * 0.5f));
    float maxWidth = normalBar->getContentSize().width;
    float fillWidth = maxWidth * (static_cast<float>(GJGameLevel::getNormalPercent(m_level)) / 100.0f);
    if (fillWidth > maxWidth) {
        fillWidth = maxWidth;
    }
    normalBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, fillWidth, normalBar->getContentSize().height));

    cocos2d::CCSprite* practiceBarBg = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    practiceBarBg->setColor(cocos2d::ccc3(0, 0, 0));
    practiceBarBg->setOpacity(125);
    practiceBarBg->setScale(0.7f);
    this->addChild(practiceBarBg, 3);
    practiceBarBg->setPosition(normalBarBg->getPosition() + cocos2d::CCPoint(0.0f, -32.0f));

    cocos2d::CCSprite* practiceBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    practiceBar->setScaleX(0.992f);
    practiceBar->setScaleY(0.86f);
    practiceBar->setColor(cocos2d::ccc3(0, 255, 255));
    practiceBarBg->addChild(practiceBar, 1);
    practiceBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    practiceBar->setPosition(normalBar->getPosition());
    float maxPracticeWidth = practiceBar->getContentSize().width;
    float practiceFill = maxPracticeWidth * (static_cast<float>(m_level->m_practicePercent) / 100.0f);
    if (practiceFill > maxPracticeWidth) {
        practiceFill = maxPracticeWidth;
    }
    practiceBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, practiceFill, practiceBar->getContentSize().height));

    std::string normalText = cocos2d::CCString::createWithFormat("%i%%", GJGameLevel::getNormalPercent(m_level))->getCString();
    if (GameManager::sharedState()->getGameVariable("0099")) {
        int leaderboardPercent = m_level->m_newNormalPercent2.random - m_level->m_newNormalPercent2.seed;
        if (GJGameLevel::getNormalPercent(m_level) != leaderboardPercent) {
            normalText += cocos2d::CCString::createWithFormat(" (%i%%)", leaderboardPercent)->getCString();
        }
    }

    cocos2d::CCLabelBMFont* normalLabel = cocos2d::CCLabelBMFont::create(normalText.c_str(), "bigFont.fnt");
    this->addChild(normalLabel, 4);
    normalLabel->setPosition(normalBarBg->getPosition());
    normalLabel->setScale(0.4f);

    cocos2d::CCLabelBMFont* practiceLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i%%", m_level->m_practicePercent)->getCString(), "bigFont.fnt");
    this->addChild(practiceLabel, 4);
    practiceLabel->setPosition(practiceBarBg->getPosition());
    practiceLabel->setScale(0.4f);

    cocos2d::CCLabelBMFont* normalTitle = cocos2d::CCLabelBMFont::create("Normal", "bigFont.fnt");
    this->addChild(normalTitle, 4);
    normalTitle->setPosition(normalBarBg->getPosition() + cocos2d::CCPoint(0.0f, 15.0f));
    normalTitle->setScale(0.4f);

    cocos2d::CCLabelBMFont* practiceTitle = cocos2d::CCLabelBMFont::create("Practice", "bigFont.fnt");
    this->addChild(practiceTitle, 4);
    practiceTitle->setPosition(practiceBarBg->getPosition() + cocos2d::CCPoint(0.0f, 15.0f));
    practiceTitle->setScale(0.4f);
}

bool LevelInfoLayer::shouldDownloadLevel() {
    if (m_level->m_levelType == GJLevelType::SearchResult) {
        return true;
    }
    return m_level->m_levelString.empty();
}

void LevelInfoLayer::showSongWarning() {
    FLAlertLayer* alert = FLAlertLayer::create(
        this,
        "No Song",
        "This level uses a <cl>custom song</c> that has not been <cg>downloaded</c> yet.\n"
        "Do you want to play without music?\n"
        "<cy>Download by using the bar below</c>"
    );
    alert->setTag(9);
    alert->show();
}

void LevelInfoLayer::showUpdateAlert(UpdateResponse type) {
    std::string message;
    switch (type) {
        case UpdateResponse::GameVerOutOfDate:
            message = "Update your application to update this level.";
            break;
        case UpdateResponse::UpdateSuccess:
            message = "Level has been updated to the latest version.";
            break;
        case UpdateResponse::UpToDate:
            message = "Level is up to date.";
            break;
        default:
            return;
    }
    FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Update", message.c_str());
    alert->show();
}

void LevelInfoLayer::tryCloneLevel(cocos2d::CCObject* sender) {
    if (!m_isBusy) {
        if (m_level->m_password.random - m_level->m_password.seed == 1) {
            confirmClone(nullptr);
        } else if (m_level->m_failedPasswordAttempts <= 4) {
            NumberInputLayer* layer = NumberInputLayer::create();
            layer->m_delegate = this;
            layer->m_maximum = 6;
            layer->show();
        } else {
            FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Error", "Too many failed attempts!");
            alert->show();
        }
    }
}

void LevelInfoLayer::tryShowAd() {
    GameManager::sharedState()->tryShowInterstitial(120, 140, 0);
}

void LevelInfoLayer::updateLabelValues() {
    m_exactLengthLabel->setVisible(false);

    if (GJGameLevel::isPlatformer(m_level)) {
        int seconds = static_cast<int>(m_level->m_timestamp / 240.0f);
        std::string timeText = "<1min";
        if (seconds > 60) {
            timeText = GameToolbox::getTimeString(seconds, 1);
        } else if (seconds == 0) {
            timeText = "Dur.";
        }
        m_exactLengthLabel->setString(timeText.c_str());
        m_exactLengthLabel->setVisible(true);
    }

    m_lengthLabel->setString(GJGameLevel::lengthKeyToString(m_level->m_levelLength).c_str());
    m_downloadsLabel->setString(GameToolbox::pointsToString(m_level->m_downloads).c_str());

    int awardedCurrency = GameStatsManager::sharedState()->getAwardedCurrencyForLevel(m_level);
    int baseCurrency = static_cast<int>(GameStatsManager::sharedState()->getBaseCurrencyForLevel(m_level) * 1.25f);
    m_orbsLabel->setString(cocos2d::CCString::createWithFormat("%i/%i", awardedCurrency, baseCurrency)->getCString());

    int likeCount = m_level->m_likes - m_level->m_dislikes;
    m_likesLabel->setString(GameToolbox::pointsToString(likeCount).c_str());

    if (likeCount < 0) {
        m_likesIcon->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_dislikesIcon_001.png"));
    } else {
        m_likesIcon->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_likesIcon_001.png"));
    }

    GameManager* GM = GameManager::sharedState();
    if (GM->m_playerUserID.random - GM->m_playerUserID.seed == m_level->m_userID.random - m_level->m_userID.seed) {
        if (likeCount > 49) {
            GM->reportAchievementWithID("geometry.ach.creator03", 100, false);
            if (likeCount > 99) {
                GM->reportAchievementWithID("geometry.ach.creator01", 100, false);
                if (likeCount > 249) {
                    GM->reportAchievementWithID("geometry.ach.creator04", 100, false);
                    if (likeCount > 499) {
                        GM->reportAchievementWithID("geometry.ach.creator05", 100, false);
                        if (likeCount > 999) {
                            GM->reportAchievementWithID("geometry.ach.creator06", 100, false);
                        }
                    }
                }
            }
        }
        if (m_level->m_stars.random - m_level->m_stars.seed > 0) {
            GM->reportAchievementWithID("geometry.ach.creator02", 100, false);
        }
    }

    m_lengthLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);
    m_exactLengthLabel->limitLabelWidth(42.0f, 0.35f, 0.0f);
    m_downloadsLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);
    m_likesLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);
    m_orbsLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);

    int difficulty = GJGameLevel::getAverageDifficulty(m_level);
    cocos2d::CCPoint difficultyPos = m_basePosition;
    if (m_level->m_demon.random - m_level->m_demon.seed == 1) {
        difficulty = GJGameLevel::demonIconForDifficulty(static_cast<DemonDifficultyType>(m_level->m_demonDifficulty));
        difficultyPos.y += 5.0f;
    }
    if (m_level->m_autoLevel) {
        difficulty = -1;
    }
    m_difficultySprite->updateDifficultyFrame(difficulty, GJDifficultyName::Long);
    m_difficultySprite->setPosition(difficultyPos);
    m_difficultySprite->updateFeatureStateFromLevel(m_level);

    m_starsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_level->m_stars.random - m_level->m_stars.seed)->getCString());

    if (GameStatsManager::sharedState()->hasCompletedStarLevel(m_level)) {
        m_starsLabel->setColor(cocos2d::ccc3(255, 255, 50));
    }

    bool hasStars = m_level->m_stars.random - m_level->m_stars.seed > 0;
    m_starsLabel->setVisible(hasStars);
    m_starsIcon->setVisible(hasStars);

    float starsOffsetY = (m_level->m_demon.random - m_level->m_demon.seed == 1) ? -4.0f : 0.0f;
    m_starsLabel->setPosition(cocos2d::CCPoint(m_starsLabel->getPositionX(), starsOffsetY + m_baseStarsY));
    m_starsIcon->setPosition(cocos2d::CCPoint(m_starsIcon->getPositionX(), starsOffsetY + m_baseStarsY));

    bool orbsWereVisible = m_orbsLabel->isVisible();
    if (hasStars && !m_challenge) {
        m_orbsLabel->setVisible(true);
        m_orbsIcon->setVisible(true);
    } else {
        m_orbsLabel->setVisible(false);
        m_orbsIcon->setVisible(false);
    }

    if (orbsWereVisible != m_orbsLabel->isVisible()) {
        if (!m_orbsLabel->isVisible()) {
            m_iconOffset = -m_iconOffset;
        }
        for (unsigned int i = 0; i < m_icons->count(); ++i) {
            cocos2d::CCNode* icon = static_cast<cocos2d::CCNode*>(m_icons->objectAtIndex(i));
            icon->setPosition(icon->getPosition() + cocos2d::CCPoint(0.0f, m_iconOffset));
        }
    }

    for (unsigned int i = 0; i < m_coins->count(); ++i) {
        static_cast<cocos2d::CCNode*>(m_coins->objectAtIndex(i))->removeFromParent();
    }
    m_coins->removeAllObjects();

    int coinCount = m_level->m_coins;
    if (coinCount > 0) {
        float coinY;
        if (m_level->m_stars.random - m_level->m_stars.seed <= 0) {
            coinY = -31.5f;
        } else if (m_level->m_dailyID.random - m_level->m_dailyID.seed > 0 || m_level->m_gauntletLevel) {
            coinY = -60.0f;
        } else {
            coinY = -45.5f;
        }

        int verified = m_level->m_coinsVerified.random - m_level->m_coinsVerified.seed;
        for (int i = 0; i < coinCount && i <= 2; ++i) {
            float coinX;
            if (i == 1) {
                if (coinCount != 2) {
                    coinX = 0.0f;
                } else {
                    coinX = 5.0f;
                }
            } else if (i == 2) {
                coinX = 10.0f;
            } else {
                if (coinCount == 2) {
                    coinX = -5.0f;
                } else if (coinCount > 2) {
                    coinX = -10.0f;
                } else {
                    coinX = 0.0f;
                }
            }

            std::string coinKey = GJGameLevel::getCoinKey(m_level, i + 1);
            bool hasCoin = GameStatsManager::sharedState()->hasUserCoin(coinKey.c_str())
                || GameStatsManager::sharedState()->hasPendingUserCoin(coinKey.c_str());

            cocos2d::CCSprite* coin = cocos2d::CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
            this->addChild(coin);
            coin->setPosition(m_basePosition + cocos2d::CCPoint(coinX, coinY + starsOffsetY));
            m_coins->addObject(coin);

            if (verified == 1) {
                if (hasCoin) {
                    coin->setColor(cocos2d::ccc3(255, 255, 255));
                } else {
                    coin->setColor(cocos2d::ccc3(165, 165, 165));
                }
            } else {
                if (hasCoin) {
                    coin->setColor(cocos2d::ccc3(255, 75, 75));
                } else {
                    coin->setColor(cocos2d::ccc3(75, 48, 48));
                }
            }
        }
    }
}

void LevelInfoLayer::updateSideButtons() {
    if (GameLevelManager::sharedState()->hasLikedItemFullCheck(LikeItemType::Level, m_level->m_levelID.random - m_level->m_levelID.seed, false, reinterpret_cast<int>(m_level))) {
        m_likeBtn->setEnabled(false);
        m_likeBtn->getNormalImage()->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_like2Btn2_001.png"));
    }

    if (m_starRateBtn) {
        if (GameLevelManager::sharedState()->hasRatedLevelStars(m_level->m_levelID.random - m_level->m_levelID.seed)) {
            m_starRateBtn->setEnabled(false);
            m_starRateBtn->getNormalImage()->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_starBtn2_001.png"));
        }
    }

    if (m_demonRateBtn) {
        GameManager* GM = GameManager::sharedState();
        if ((GM->m_hasRP.random - GM->m_hasRP.seed - 1) > 1 && !GM->m_hasDRP) {
            if (GameLevelManager::sharedState()->hasRatedDemon(m_level->m_levelID.random - m_level->m_levelID.seed)) {
                m_demonRateBtn->setEnabled(false);
                m_demonRateBtn->getNormalImage()->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_rateDiffBtn2_001.png"));
            }
        }
    }
}

LevelInfoLayer::~LevelInfoLayer() {
}