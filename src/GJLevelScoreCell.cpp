#include "GJLevelScoreCell.h"
#include "GJUserScore.h"
#include "SimplePlayer.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "ProfilePage.h"
#include "CCMenuItemSpriteExtra.h"

GJLevelScoreCell::GJLevelScoreCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void GJLevelScoreCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

bool GJLevelScoreCell::init() {
    m_userScore = nullptr;
    return true;
}

void GJLevelScoreCell::loadFromScore(GJUserScore* score) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_userScore = score;
    
    auto player = SimplePlayer::create(1);
    m_mainLayer->addChild(player, 1);
    player->setScale(0.8f);
    player->setPosition(cocos2d::CCPoint(60.0f, m_height * 0.5f));
    
    auto gameManager = GameManager::sharedState();
    player->setColor(gameManager->colorForIdx(m_userScore->m_color1));
    player->setSecondColor(gameManager->colorForIdx(m_userScore->m_color2));
    player->tryEnableCustomGlowColor(m_userScore->m_color3);
    
    bool hasGlow = (m_userScore->m_special == 2 || m_userScore->m_glowEnabled);
    player->m_hasGlowOutline = hasGlow;
    player->updatePlayerFrame(m_userScore->m_iconID, m_userScore->m_iconType);
    player->updateColors();
    
    auto menu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(menu);
    
    cocos2d::CCPoint rankPos(22.0f, m_height * 0.5f);
    int rank = m_userScore->m_playerRank;
    
    if (rank > 3) {
        auto rankLabel = cocos2d::CCLabelBMFont::create(std::to_string(rank).c_str(), "bigFont.fnt");
        m_mainLayer->addChild(rankLabel);
        rankLabel->setPosition(rankPos);
        rankLabel->limitLabelWidth(20.0f, 0.75f, 0.0f);
    } else {
        const char* rankFrame = nullptr;
        if (rank == 1) {
            rankFrame = "rankIcon_top10_001.png";
        } else if (rank == 2) {
            rankFrame = "rankIcon_top50_001.png";
        } else {
            rankFrame = "rankIcon_top100_001.png";
        }
        
        auto rankSprite = cocos2d::CCSprite::createWithSpriteFrameName(rankFrame);
        m_mainLayer->addChild(rankSprite);
        rankSprite->setPosition(rankPos);
        rankSprite->setScale(0.8f);
    }
    
    auto nameLabel = cocos2d::CCLabelBMFont::create(m_userScore->m_userName.c_str(), "bigFont.fnt");
    if (m_userScore->m_levelMode == 1) {
        nameLabel->limitLabelWidth(100.0f, 0.7f, 0.0f);
    } else {
        nameLabel->limitLabelWidth(130.0f, 0.8f, 0.0f);
    }
    
    auto menuItem = CCMenuItemSpriteExtra::create(nameLabel, this, menu_selector(GJLevelScoreCell::onViewProfile));
    menu->addChild(menuItem);
    
    float labelWidth = nameLabel->getContentSize().width;
    float labelScale = nameLabel->getScale();
    cocos2d::CCPoint playerPos = player->getPosition();
    cocos2d::CCPoint offset(30.0f, 0.0f);
    cocos2d::CCPoint pos = playerPos + offset;
    pos.x += labelWidth * labelScale * 0.5f;
    menuItem->setPosition(menu->convertToNodeSpace(pos));
    
    if (GJUserScore::isCurrentUser(m_userScore)) {
        m_backgroundLayer->setColor(cocos2d::ccc3(80, 150, 10));
    }
    
    int stars = m_userScore->m_stars;
    std::string scoreText;
    
    if (m_userScore->m_levelMode != 1) {
        scoreText = std::to_string(stars) + "%";
    } else {
        if (m_userScore->m_leaderboardMode) {
            scoreText = GameToolbox::pointsToString(stars);
        } else {
            scoreText = GameToolbox::msToTimeString(stars, 1);
        }
    }
    
    auto scoreLabel = cocos2d::CCLabelBMFont::create(scoreText.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(scoreLabel, 2);
    
    if (m_userScore->m_levelMode == 1) {
        scoreLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        scoreLabel->limitLabelWidth(85.0f, 0.5f, 0.0f);
        scoreLabel->setPosition(cocos2d::CCPoint(m_width - 60.0f, m_height * 0.5f));
    } else {
        scoreLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        scoreLabel->limitLabelWidth(200.0f, 0.6f, 0.0f);
        scoreLabel->setPosition(cocos2d::CCPoint(labelWidth * labelScale + 6.0f, -1.0f));
        
        int coins = m_userScore->m_secretCoins;
        if (coins > 0) {
            int xOffset = 65;
            for (int i = 0; i < coins; ++i) {
                auto coin = cocos2d::CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
                m_mainLayer->addChild(coin);
                coin->setScale(0.9f);
                cocos2d::CCPoint coinPos = scoreLabel->getPosition() + cocos2d::CCPoint(static_cast<float>(xOffset), 5.0f);
                coin->setPosition(coinPos);
                xOffset += 8;
            }
        }
    }
    
    std::string timeText = m_userScore->m_unkString + " ago";
    auto timeLabel = cocos2d::CCLabelBMFont::create(timeText.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(timeLabel, 1);
    timeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    timeLabel->setScale(0.5f);
    timeLabel->setColor(cocos2d::ccc3(125, 125, 125));
    timeLabel->setOpacity(125);
    timeLabel->setPosition(cocos2d::CCPoint(m_width - 11.0f, 10.0f));
    
    if (m_userScore->m_levelMode == 1) {
        timeLabel->limitLabelWidth(35.0f, 0.5f, 0.0f);
    }
}

void GJLevelScoreCell::onViewProfile(cocos2d::CCObject* sender) {
    if (m_userScore) {
        int accountId = m_userScore->m_accountID;
        if (accountId > 0) {
            auto profile = ProfilePage::create(accountId, 0);
            profile->show();
        }
    }
}

void GJLevelScoreCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

GJLevelScoreCell::~GJLevelScoreCell() {
}