#include "GJScoreCell.h"
#include "GJUserScore.h"
#include "SimplePlayer.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "ProfilePage.h"
#include "LevelBrowserLayer.h"
#include "GJSearchObject.h"
#include "FLAlertLayer.h"
#include "CCMenuItemSpriteExtra.h"

GJScoreCell::GJScoreCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void GJScoreCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

bool GJScoreCell::init() {
    m_score = nullptr;
    return true;
}

void GJScoreCell::loadFromScore(GJUserScore* score) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_score = score;
    
    bool hasCreatorPoints = score->m_creatorPoints > 0;
    float playerScale = (score->m_iconType == IconType::Ship) ? 0.75f : 1.0f;
    
    auto player = SimplePlayer::create(1);
    m_mainLayer->addChild(player, 1);
    player->setScale(playerScale);
    player->setPosition(cocos2d::CCPoint(30.0f, m_height * 0.5f + 7.0f));
    
    auto manager = GameManager::sharedState();
    player->setColor(manager->colorForIdx(score->m_color1));
    player->setSecondColor(manager->colorForIdx(score->m_color2));
    player->tryEnableCustomGlowColor(score->m_color3);
    
    bool hasGlow = (score->m_special == 2 || score->m_glowEnabled);
    player->m_hasGlowOutline = hasGlow;
    player->updatePlayerFrame(score->m_iconID, score->m_iconType);
    player->updateColors();
    
    auto menu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(menu);
    
    if (score->m_scoreType != 2) {
        auto rankLabel = cocos2d::CCLabelBMFont::create(std::to_string(score->m_playerRank).c_str(), "bigFont.fnt");
        m_mainLayer->addChild(rankLabel);
        rankLabel->setPosition(cocos2d::CCPoint(30.0f, m_height * 0.5f - 18.0f));
        rankLabel->limitLabelWidth(40.0f, 0.65f, 0.0f);
    }
    
    auto nameLabel = cocos2d::CCLabelBMFont::create(score->m_userName.c_str(), "bigFont.fnt");
    nameLabel->limitLabelWidth(120.0f, 0.8f, 0.0f);
    
    auto nameItem = CCMenuItemSpriteExtra::create(nameLabel, this, menu_selector(GJScoreCell::onViewProfile));
    menu->addChild(nameItem);
    
    cocos2d::CCPoint playerPos = player->getPosition();
    float labelWidth = nameLabel->getContentSize().width * nameLabel->getScale();
    nameItem->setPosition(menu->convertToNodeSpace(
        playerPos + cocos2d::CCPoint(30.0f + labelWidth * 0.5f, 7.0f)
    ));
    
    if (score->m_scoreType == 2) {
        player->setPosition(player->getPosition() + cocos2d::CCPoint(0.0f, -7.0f));
    }
    
    if (GJUserScore::isCurrentUser(score)) {
        m_backgroundLayer->setColor(cocos2d::ccc3(230, 150, 10));
    } else if (score->m_accountID <= 0 && score->m_scoreType != 1) {
        nameLabel->setColor(cocos2d::ccc3(100, 100, 255));
        nameItem->setEnabled(false);
    }
    
    float labelScale = hasCreatorPoints ? 0.45f : 0.5f;
    float labelY = m_height * 0.5f - 13.0f;
    float labelWidthMax = hasCreatorPoints ? 35.0f : 50.0f;
    
    int stars = score->m_stars;
    int moons = score->m_moons;
    int demons = score->m_demons;
    int userCoins = score->m_userCoins;
    
    const char* iconFrame1 = "GJ_starsIcon_001.png";
    const char* iconFrame2 = "GJ_moonsIcon_001.png";
    const char* iconFrame3 = "GJ_demonIcon_001.png";
    const char* iconFrame4 = "GJ_coinsIcon2_001.png";
    
    if (score->m_leaderboardStat != LeaderboardStat::None) {
        switch (score->m_leaderboardStat) {
            case LeaderboardStat::Demons:
                iconFrame1 = "GJ_demonIcon_001.png";
                std::swap(stars, demons);
                break;
            case LeaderboardStat::UserCoins:
                iconFrame1 = "GJ_coinsIcon2_001.png";
                std::swap(stars, userCoins);
                break;
            case LeaderboardStat::Moons:
                iconFrame1 = "GJ_moonsIcon_001.png";
                std::swap(stars, moons);
                break;
            default:
                break;
        }
    }
    
    auto starLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(stars).c_str(), "bigFont.fnt");
    m_mainLayer->addChild(starLabel, 2);
    starLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    starLabel->limitLabelWidth(60.0f, labelScale, 0.0f);
    starLabel->setPosition(playerPos + cocos2d::CCPoint(30.0f + labelWidth + 5.0f, -2.0f));
    
    auto starIcon = cocos2d::CCSprite::createWithSpriteFrameName(iconFrame1);
    m_mainLayer->addChild(starIcon, 2);
    starIcon->setPosition(starLabel->getPosition() + cocos2d::CCPoint(starLabel->getContentSize().width * starLabel->getScale() + 10.0f, 0.0f));
    starIcon->setScale(0.7f);
    
    auto moonLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(moons).c_str(), "bigFont.fnt");
    m_mainLayer->addChild(moonLabel, 2);
    moonLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    moonLabel->limitLabelWidth(60.0f, labelScale, 0.0f);
    moonLabel->setPosition(starIcon->getPosition() + cocos2d::CCPoint(starIcon->getContentSize().width * starIcon->getScale() * 0.5f + 8.0f, 0.0f));
    
    auto moonIcon = cocos2d::CCSprite::createWithSpriteFrameName(iconFrame2);
    m_mainLayer->addChild(moonIcon, 2);
    moonIcon->setPosition(moonLabel->getPosition() + cocos2d::CCPoint(moonLabel->getContentSize().width * moonLabel->getScale() + 10.0f, 0.0f));
    moonIcon->setScale(0.7f);
    
    auto diamondsLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(score->m_diamonds).c_str(), "bigFont.fnt");
    m_mainLayer->addChild(diamondsLabel, 2);
    diamondsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    diamondsLabel->setPosition(cocos2d::CCPoint(62.0f, labelY));
    diamondsLabel->limitLabelWidth(labelWidthMax, labelScale, 0.0f);
    
    auto diamondsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_diamondsIcon_001.png");
    m_mainLayer->addChild(diamondsIcon, 2);
    diamondsIcon->setPosition(diamondsLabel->getPosition() + cocos2d::CCPoint(diamondsLabel->getContentSize().width * diamondsLabel->getScale() + 10.0f, 0.0f));
    diamondsIcon->setScale(0.7f);
    
    auto coinsLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(score->m_secretCoins).c_str(), "bigFont.fnt");
    m_mainLayer->addChild(coinsLabel, 2);
    coinsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    coinsLabel->setPosition(diamondsIcon->getPosition() + cocos2d::CCPoint(diamondsIcon->getContentSize().width * diamondsIcon->getScale() * 0.5f + 8.0f, 0.0f));
    coinsLabel->limitLabelWidth(labelWidthMax, labelScale, 0.0f);
    
    auto coinsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
    m_mainLayer->addChild(coinsIcon, 2);
    coinsIcon->setPosition(coinsLabel->getPosition() + cocos2d::CCPoint(coinsLabel->getContentSize().width * coinsLabel->getScale() + 10.0f, 0.0f));
    coinsIcon->setScale(0.7f);
    
    auto userCoinsLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(userCoins).c_str(), "bigFont.fnt");
    m_mainLayer->addChild(userCoinsLabel, 2);
    userCoinsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    userCoinsLabel->setPosition(coinsIcon->getPosition() + cocos2d::CCPoint(coinsIcon->getContentSize().width * coinsIcon->getScale() * 0.5f + 8.0f, 0.0f));
    userCoinsLabel->limitLabelWidth(labelWidthMax, labelScale, 0.0f);
    
    auto userCoinsIcon = cocos2d::CCSprite::createWithSpriteFrameName(iconFrame4);
    m_mainLayer->addChild(userCoinsIcon, 2);
    userCoinsIcon->setPosition(userCoinsLabel->getPosition() + cocos2d::CCPoint(userCoinsLabel->getContentSize().width * userCoinsLabel->getScale() + 10.0f, 0.0f));
    userCoinsIcon->setScale(0.7f);
    
    auto demonsLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(demons).c_str(), "bigFont.fnt");
    m_mainLayer->addChild(demonsLabel, 2);
    demonsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    demonsLabel->setPosition(userCoinsIcon->getPosition() + cocos2d::CCPoint(userCoinsIcon->getContentSize().width * userCoinsIcon->getScale() * 0.5f + 8.0f, 0.0f));
    demonsLabel->limitLabelWidth(labelWidthMax, labelScale, 0.0f);
    
    auto demonsIcon = cocos2d::CCSprite::createWithSpriteFrameName(iconFrame3);
    m_mainLayer->addChild(demonsIcon, 2);
    demonsIcon->setPosition(demonsLabel->getPosition() + cocos2d::CCPoint(demonsLabel->getContentSize().width * demonsLabel->getScale() + 10.0f, 0.0f));
    demonsIcon->setScale(0.7f);
    
    if (hasCreatorPoints) {
        auto cpLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(score->m_creatorPoints).c_str(), "bigFont.fnt");
        m_mainLayer->addChild(cpLabel, 2);
        cpLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        cpLabel->setPosition(demonsIcon->getPosition() + cocos2d::CCPoint(demonsIcon->getContentSize().width * demonsIcon->getScale() * 0.5f + 8.0f, 0.0f));
        cpLabel->limitLabelWidth(labelWidthMax, labelScale, 0.0f);
        
        auto cpIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
        m_mainLayer->addChild(cpIcon, 2);
        cpIcon->setPosition(cpLabel->getPosition() + cocos2d::CCPoint(cpLabel->getContentSize().width * cpLabel->getScale() + 10.0f, 0.0f));
        cpIcon->setScale(0.7f);
    }
}

void GJScoreCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2 && layer->getTag() == 101 && m_score) {
        GameLevelManager::sharedState()->banUser(m_score->m_userID);
    }
}

void GJScoreCell::onBan(cocos2d::CCObject* sender) {
  //Empty func
}

void GJScoreCell::onCheck(cocos2d::CCObject* sender) {
  //Empty func
}

void GJScoreCell::onMoreLevels(cocos2d::CCObject* sender) {
    if (m_score && m_score->m_accountID > 0) {
        auto profile = ProfilePage::create(m_score->m_accountID, 0);
        profile->show();
    }
}

void GJScoreCell::onViewProfile(cocos2d::CCObject* sender) {
    if (!m_score) {
        return;
    }
    
    if (m_score->m_accountID > 0) {
        auto profile = ProfilePage::create(m_score->m_accountID, 0);
        profile->show();
        return;
    }
    
    if (m_score->m_scoreType == 1) {
        auto director = cocos2d::CCDirector::sharedDirector();
        if (director->m_bIsTransitioning) {
            return;
        }
        
        auto runningScene = director->getRunningScene();
        if (runningScene->m_eObjType != MenuLayerObjectType) {
            if (director->m_pSceneReference) {
                director->popToSceneInStack(director->m_pSceneReference);
            } else {
                director->m_pSceneReference = runningScene;
            }
        }
        
        auto searchObj = GJSearchObject::create(SearchType::UsersLevels);
        auto levelBrowser = LevelBrowserLayer::create(searchObj);
        auto transition = cocos2d::CCTransitionFade::create(0.5f, levelBrowser);
        director->pushScene(transition);
    }
}

void GJScoreCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

GJScoreCell::~GJScoreCell() {
}