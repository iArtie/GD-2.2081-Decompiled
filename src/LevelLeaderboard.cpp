#include "LevelLeaderboard.h"
#include "GJGameLevel.h"
#include "GameManager.h"
#include "GameLevelManager.h"
#include "GJAccountManager.h"
#include "GJLocalScore.h"
#include "FLAlertLayer.h"
#include "TextArea.h"
#include "TextAlertPopup.h"
#include "LoadingCircle.h"
#include "CCMenuItemSpriteExtra.h"
#include "GJCommentListLayer.h"
#include "CustomListView.h"
#include "InfoAlertButton.h"
#include "ButtonSprite.h"
#include "PlatformToolbox.h"
#include "cocos-ext.h"

void LevelLeaderboard::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (layer->getTag() == 991 && btn2) {
        deleteLocalScores();
    }
}

LevelLeaderboard* LevelLeaderboard::create(GJGameLevel* level, LevelLeaderboardType type, LevelLeaderboardMode mode) {
    LevelLeaderboard* layer = new LevelLeaderboard();
    if (layer && layer->init(level, type, mode)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool LevelLeaderboard::init(GJGameLevel* level, LevelLeaderboardType type, LevelLeaderboardMode mode) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }

    if (type == LevelLeaderboardType::Local && !GJGameLevel::isPlatformer(level)) {
        type = static_cast<LevelLeaderboardType>(1);
    }

    GJLevelType levelType = level->m_levelType;
    m_level = level;
    m_mode = mode;
    bool isEditor = (levelType == GJLevelType::Editor);
    if (levelType == GJLevelType::Editor) {
        type = LevelLeaderboardType::Local;
    }
    m_type = type;

    level->retain();
    bool isPlatformer = GJGameLevel::isPlatformer(level);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    std::string titleStr = level->m_levelName + " Leaderboard";
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(titleStr.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 18.0f));
    titleLabel->limitLabelWidth(300.0f, 0.8f, 0.0f);

    if (isPlatformer) {
        const char* iconFrame = (mode == LevelLeaderboardMode::Points) ? "GJ_pointsIcon_001.png" : "GJ_timeIcon_001.png";
        cocos2d::CCSprite* icon = cocos2d::CCSprite::createWithSpriteFrameName(iconFrame);
        float iconWidth = icon->getContentSize().width;
        m_mainLayer->addChild(icon, 20);

        cocos2d::CCPoint titlePos = titleLabel->getPosition();
        float titleWidth = titleLabel->getContentSize().width;
        float titleScale = titleLabel->getScale();
        cocos2d::CCPoint iconOffset(titleWidth * titleScale * 0.5f + 2.5f, -3.0f);
        icon->setPosition(titlePos + iconOffset);

        cocos2d::CCPoint labelOffset(-iconWidth * 0.5f - 2.5f, 0.0f);
        titleLabel->setPosition(titlePos + labelOffset);

        int timeTag = (mode == LevelLeaderboardMode::Time) ? 2 : 1;
        m_timeBtn = getSpriteButton("GJ_timeIcon_001.png",
            menu_selector(LevelLeaderboard::onChangeMode),
            cocos2d::CCPoint(0.0f, -0.5f), timeTag, m_buttonMenu, 0.7f);
        m_timeBtn->setEnabled(mode != LevelLeaderboardMode::Time);
        m_timeBtn->setTag(0);

        int pointsTag = (mode == LevelLeaderboardMode::Points) ? 2 : 1;
        m_pointsBtn = getSpriteButton("GJ_pointsIcon_001.png",
            menu_selector(LevelLeaderboard::onChangeMode),
            cocos2d::CCPoint(0.0f, -1.0f), pointsTag, m_buttonMenu, 0.7f);
        m_pointsBtn->setEnabled(mode != LevelLeaderboardMode::Points);
        m_pointsBtn->setTag(1);

        cocos2d::CCPoint worldPos(winSize.width * 0.5f - 220.0f + 25.0f, winSize.height * 0.5f + 40.0f);
        cocos2d::CCPoint worldSpace = m_mainLayer->convertToWorldSpace(worldPos);
        cocos2d::CCPoint menuPos = m_buttonMenu->convertToNodeSpace(worldSpace);

        m_timeBtn->setPosition(menuPos + cocos2d::CCPoint(0.0f, 20.0f));
        m_pointsBtn->setPosition(menuPos + cocos2d::CCPoint(0.0f, -20.0f));

        if (isEditor) {
            CCMenuItemSpriteExtra* deleteBtn = getSpriteButton("edit_delBtn_001.png",
                menu_selector(LevelLeaderboard::onDeleteLocalScores),
                cocos2d::CCPoint(0.0f, -0.5f), 4, m_buttonMenu, 0.7f);
            cocos2d::CCPoint btnWorldPos(winSize.width * 0.5f + 220.0f - 25.0f,
                winSize.height * 0.5f + 145.0f - 23.0f);
            cocos2d::CCPoint btnWorld = m_mainLayer->convertToWorldSpace(btnWorldPos);
            cocos2d::CCPoint btnMenuPos = m_buttonMenu->convertToNodeSpace(btnWorld);
            deleteBtn->setPosition(btnMenuPos);
        }
    }

    if (level->m_accountID.random == level->m_accountID.seed) {
        GameLevelManager::sharedState()->accountIDForUserID(level->m_userID.random - level->m_userID.seed);
    }

    cocos2d::CCSprite* closeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(closeSpr, nullptr, this,
        menu_selector(LevelLeaderboard::onClose));
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setSizeMult(1.6f);
    cocos2d::CCPoint closePos(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f);
    closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(closePos));

    cocos2d::CCSprite* refreshSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    refreshSpr->setScale(0.7f);
    m_refreshBtn = CCMenuItemSpriteExtra::create(refreshSpr, nullptr, this,
        menu_selector(LevelLeaderboard::onUpdate));
    m_refreshBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(m_refreshBtn);
    cocos2d::CCPoint refreshPos(winSize.width * 0.5f + 220.0f - 25.0f, winSize.height * 0.5f + 100.0f);
    m_refreshBtn->setPosition(m_buttonMenu->convertToNodeSpace(refreshPos));
    m_refreshBtn->setVisible(false);

    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = m_mainLayer;
    m_circle->m_fade = false;
    m_circle->retain();
    m_circle->show();
    m_circle->setVisible(false);

    m_noInternet = TextArea::create(
        "No <cg>Internet</c> connection!",
        "bigFont.fnt",
        1.0f,
        100.0f,
        cocos2d::CCPoint(0.5f, 1.0f),
        30.0f,
        0
    );
    m_noInternet->m_height = 30.0f;
    m_noInternet->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f));
    m_noInternet->setScale(0.7f);
    m_mainLayer->addChild(m_noInternet, 2);
    m_noInternet->setVisible(false);

    if (!isEditor) {
        cocos2d::CCMenu* typeMenu = cocos2d::CCMenu::create();
        m_mainLayer->addChild(typeMenu, 2);

        float yOffset = isPlatformer ? 15.0f : 0.0f;
        int count = isPlatformer ? 4 : 3;

        for (int i = 0; i < count; ++i) {
            const char* frame;
            int tag;
            switch (i) {
                case 2:
                    frame = "levelLeaderboard_globalWeeklyBtn_001.png";
                    tag = 2;
                    break;
                case 3:
                    frame = "levelLeaderboard_localBtn_001.png";
                    tag = 3;
                    break;
                case 1:
                    frame = "levelLeaderboard_globalBtn_001.png";
                    tag = 1;
                    break;
                default:
                    frame = "levelLeaderboard_friendsBtn_001.png";
                    tag = 0;
                    break;
            }

            cocos2d::CCSprite* spr = cocos2d::CCSprite::createWithSpriteFrameName(frame);
            CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, nullptr, this,
                menu_selector(LevelLeaderboard::onChangeType));
            btn->setTag(tag);
            typeMenu->addChild(btn);

            cocos2d::CCPoint pos(winSize.width * 0.5f + 220.0f - 25.0f,
                winSize.height * 0.5f + 42.0f - (i * 42.0f) + yOffset);
            btn->setPosition(typeMenu->convertToNodeSpace(pos));

            if (tag == static_cast<int>(m_type)) {
                btn->setEnabled(false);
                spr->setColor(cocos2d::ccc3(150, 150, 150));
            }
        }
    }

    if (GJGameLevel::getNormalPercent(m_level) > m_level->m_newNormalPercent2.random - m_level->m_newNormalPercent2.seed) {
        InfoAlertButton* infoBtn = InfoAlertButton::create(
            "Level Leaderboards",
            "<cy>Q: My score is wrong!?</c>\n"
            "A: Level leaderboards changed in update 2.11. Scores from before update 2.11 need to be replayed. "
            "You can see your leaderboard percentage by pressing the info button on the bottom left of the level page, "
            "or toggle the Leaderboard percent option.",
            1.0f
        );
        m_buttonMenu->addChild(infoBtn);
        cocos2d::CCPoint infoPos(winSize.width * 0.5f + 220.0f - 5.0f, winSize.height * 0.5f + 145.0f - 5.0f);
        infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(infoPos));
    }

    GameManager* GM = GameManager::sharedState();
    if (GM->m_scoreValid || m_type == LevelLeaderboardType::Local) {
        loadScores();
    } else {
        setupLeaderboard(nullptr);
        GameLevelManager::sharedState()->m_leaderboardManagerDelegate = this;
        GameLevelManager::sharedState()->updateUserScore();
    }

    return true;
}

CCMenuItemSpriteExtra* LevelLeaderboard::getSpriteButton(const char* frame, cocos2d::SEL_MenuHandler callback, cocos2d::CCPoint offset, int buttonType, cocos2d::CCMenu* menu, float scale) {
    std::string buttonFrame = cocos2d::CCString::createWithFormat("GJ_button_%02d.png", buttonType)->getCString();
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frame);
    ButtonSprite* buttonSprite = ButtonSprite::create(sprite, 0, 0, 32.0f, 1.0f, true, buttonFrame.c_str(), nullptr, false);
    buttonSprite->updateSpriteOffset(offset);

    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(buttonSprite, nullptr, this, callback);
    item->setScale(scale);
    item->m_baseScale = scale;

    if (menu) {
        menu->addChild(item);
    }

    return item;
}

void LevelLeaderboard::deleteLocalScores() {
    m_level->m_localBestTimes.clear();
    m_level->m_localBestPoints.clear();

    cocos2d::CCNode* parent = getParent();
    onClose(nullptr);
    if (parent) {
        TextAlertPopup* popup = TextAlertPopup::create("Local scores reset.", 1.0f, 0.6f, 150, "");
        parent->addChild(popup, 100);
    }
}

cocos2d::CCArray* LevelLeaderboard::getLocalScores() {
    cocos2d::CCArray* result = cocos2d::CCArray::create();

    std::string* localScores = (m_mode == LevelLeaderboardMode::Points) ? &m_level->m_localBestPoints : &m_level->m_localBestTimes;
    if (localScores->empty()) {
        return result;
    }

    std::vector<int> scores;
    GJGameLevel::scoreStringToVector(localScores, &scores);

    for (size_t i = 0; i < scores.size(); ++i) {
        int scoreValue = scores[i];
        GJLocalScore* localScore = GJLocalScore::create(scoreValue, m_mode);

        if (m_level->m_M_ID == GameManager::sharedState()->m_leaderboardLevelID) {
            if (m_mode == LevelLeaderboardMode::Time) {
                if (GameManager::sharedState()->m_leaderboardLevelTime == scoreValue) {
                    localScore->m_currentScore = 1;
                }
            } else {
                if (GameManager::sharedState()->m_leaderboardLevelPoints == scoreValue) {
                    localScore->m_currentScore = 1;
                }
            }
        }

        result->addObject(localScore);
    }

    return result;
}

bool LevelLeaderboard::isCorrect(const char* key) {
    std::string expectedKey = GameLevelManager::sharedState()->getLevelLeaderboardKey(
        m_level->m_levelID.random - m_level->m_levelID.seed,
        m_type,
        m_mode
    );
    return expectedKey == key;
}

void LevelLeaderboard::keyBackClicked() {
    onClose(nullptr);
}

void LevelLeaderboard::loadLeaderboardFailed(const char* key) {
    if (!isCorrect(key)) {
        return;
    }

    m_circle->setVisible(false);

    if (PlatformToolbox::isNetworkAvailable()) {
        GJAccountManager* AM = GJAccountManager::sharedState();
        if (AM->m_accountID - AM->m_unkInt1 > 0) {
            m_refreshBtn->setVisible(m_type != LevelLeaderboardType::Local);
            return;
        }
        m_noInternet->setString("Create an <cg>account</c> to\ncompare scores\nwith <cl>other players</c>!");
    } else {
        m_noInternet->setString("No <cg>Internet</c> connection!");
    }

    m_noInternet->setVisible(true);
    m_refreshBtn->setVisible(m_type != LevelLeaderboardType::Local);
}

void LevelLeaderboard::loadLeaderboardFinished(cocos2d::CCArray* scores, const char* key) {
    if (!isCorrect(key)) {
        return;
    }

    if (m_scores) {
        m_scores->release();
    }
    m_scores = scores;
    m_scores->retain();

    setupLeaderboard(m_scores);
    m_circle->setVisible(false);
    m_noInternet->setVisible(false);
    m_refreshBtn->setVisible(m_type != LevelLeaderboardType::Local);
}

void LevelLeaderboard::loadScores() {
    std::string key = GameLevelManager::sharedState()->getLevelLeaderboardKey(
        m_level->m_levelID.random - m_level->m_levelID.seed,
        m_type,
        m_mode
    );

    if (m_type == LevelLeaderboardType::Local) {
        cocos2d::CCArray* localScores = getLocalScores();
        loadLeaderboardFinished(localScores, key.c_str());
        return;
    }

    cocos2d::CCArray* storedLevels = GameLevelManager::sharedState()->getStoredOnlineLevels(key.c_str());
    if (storedLevels) {
        loadLeaderboardFinished(storedLevels, key.c_str());
    } else {
        GameLevelManager::sharedState()->makeTimeStamp("upd_ll");
        m_circle->setVisible(true);
        m_noInternet->setVisible(false);
        setupLeaderboard(nullptr);
        GameLevelManager::sharedState()->m_leaderboardManagerDelegate = this;
        GameLevelManager::sharedState()->getLevelLeaderboard(m_level, m_type, m_mode);
    }
}

void LevelLeaderboard::onChangeMode(cocos2d::CCObject* sender) {
    LevelLeaderboardMode mode = static_cast<LevelLeaderboardMode>(sender->getTag());
    GameManager::sharedState()->setIntGameVariable("0164", static_cast<int>(mode));
    reloadLeaderboard(m_type, mode);
}

void LevelLeaderboard::onChangeType(cocos2d::CCObject* sender) {
    GJAccountManager* AM = GJAccountManager::sharedState();
    if (AM->m_accountID - AM->m_unkInt1 > 0) {
        LevelLeaderboardType type = static_cast<LevelLeaderboardType>(sender->getTag());
        GameManager::sharedState()->setIntGameVariable("0098", static_cast<int>(type));
        reloadLeaderboard(type, m_mode);
    }
}

void LevelLeaderboard::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void LevelLeaderboard::onDeleteLocalScores(cocos2d::CCObject* sender) {
    FLAlertLayer* alert = FLAlertLayer::create(this, "Reset Scores", "Do you want to <cr>delete</c> the local scores?");
    alert->setTag(991);
    alert->show();
}

void LevelLeaderboard::onUpdate(cocos2d::CCObject* sender) {
    GJAccountManager* AM = GJAccountManager::sharedState();
    if (AM->m_accountID - AM->m_unkInt1 > 0) {
        if ((unsigned int)(GameLevelManager::sharedState()->getTimeLeft("upd_ll", 5.0) - 1) > 4) {
            GameLevelManager::sharedState()->makeTimeStamp("upd_ll");
            m_noInternet->setVisible(false);
            m_circle->setVisible(true);
            m_refreshBtn->setVisible(false);
            setupLeaderboard(nullptr);
            GameLevelManager::sharedState()->m_leaderboardManagerDelegate = this;
            GameLevelManager::sharedState()->getLevelLeaderboard(m_level, m_type, m_mode);
        }
    }
}

void LevelLeaderboard::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void LevelLeaderboard::reloadLeaderboard(LevelLeaderboardType type, LevelLeaderboardMode mode) {
    if (m_list) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_list);
    }
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);

    LevelLeaderboard* leaderboard = LevelLeaderboard::create(m_level, type, mode);
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    int zOrder = 105;
    if (cocos2d::CCScene::getHighestChildZ(runningScene) > 104) {
        zOrder = cocos2d::CCScene::getHighestChildZ(runningScene) + 1;
    }
    runningScene->addChild(leaderboard, zOrder);
    onClose(nullptr);
}

void LevelLeaderboard::setupLeaderboard(cocos2d::CCArray* scores) {
    if (m_list) {
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(m_list);
        m_list->removeMeAndCleanup();
        m_list = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    CustomListView* listView = nullptr;
    if (scores) {
        if (m_level) {
            if (GJGameLevel::isPlatformer(m_level) && m_type != LevelLeaderboardType::Local) {
                for (unsigned int i = 0; i < scores->count(); ++i) {
                    GJLocalScore* score = static_cast<GJLocalScore*>(scores->objectAtIndex(i));
                    score->m_currentScore = 1;
                    score->m_mode = m_mode;
                }
            }
        }
        listView = CustomListView::create(scores, BoomListType::Score, 220.0f, 340.0f);
    }

    cocos2d::ccColor4B color = { 60, 60, 60, 255 };
    GJCommentListLayer* listLayer = GJCommentListLayer::create(listView, "Scores", color, 340.0f, 220.0f, true);
    m_list = listLayer;
    m_mainLayer->addChild(listLayer);

    m_list->setPosition(cocos2d::CCPoint((winSize.width - 340.0f) * 0.5f, (winSize.height - 220.0f) * 0.5f - 10.0f));
}

void LevelLeaderboard::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint originalPos = m_mainLayer->getPosition();
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, originalPos.y));
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(cocos2d::CCMoveTo::create(0.5f, originalPos), 0.6f));

        cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
        if (!m_scene) {
            int zOrder = 105;
            if (cocos2d::CCScene::getHighestChildZ(runningScene) > 104) {
                zOrder = cocos2d::CCScene::getHighestChildZ(runningScene) + 1;
            }
            m_ZOrder = zOrder;
            m_scene = runningScene;
        } else if (!m_ZOrder) {
            m_ZOrder = 105;
        }

        m_scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void LevelLeaderboard::updateUserScoreFailed() {
    m_circle->setVisible(false);
    if (!PlatformToolbox::isNetworkAvailable()) {
        m_noInternet->setVisible(true);
    }
}

LevelLeaderboard::~LevelLeaderboard() {
}