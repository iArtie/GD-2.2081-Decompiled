#include "LevelCell.h"
#include "GameLevelManager.h"
#include "GameStatsManager.h"
#include "MusicDownloadManager.h"
#include "GameToolbox.h"
#include "ProfilePage.h"
#include "FLAlertLayer.h"
#include "GJGameLevel.h"
#include "SongInfoObject.h"
#include "CCString.h"
#include "CCSpriteFrameCache.h"
#include "CCDirector.h"

LevelCell* LevelCell::create(float width, float height) {
    auto cell = new LevelCell(" ", width, height);
    if (cell && cell->init()) {
        cell->autorelease();
        return cell;
    }
    delete cell;
    return nullptr;
}

bool LevelCell::init() {
    m_level = nullptr;
    m_button = nullptr;
    m_mainMenu = nullptr;
    m_cellMode = 0;
    m_cellDrawn = false;
    m_compactView = false;
    return true;
}

void LevelCell::draw() {
    if (!m_cellDrawn) {
        cocos2d::CCSize size(m_width, m_height);
        cocos2d::ccDrawColor4B(0, 0, 0, 75);
        glLineWidth(1.0f);
        cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
        cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
    }
}

void LevelCell::loadFromLevel(GJGameLevel* level) {
    m_mainLayer->removeAllChildrenWithCleanup(true);

    auto menu = cocos2d::CCMenu::create();
    m_mainMenu = menu;
    m_mainLayer->addChild(menu, 3);

    m_levelType = level->m_levelType;
    m_level = level;

    if (m_levelType == LevelType::Editor) {
        loadLocalLevelCell();
    } else {
        loadCustomLevelCell();
    }

    if (m_level->m_localOrSaved && !m_compactView) {
        auto checkOn = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto checkOff = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        checkOn->setScale(0.6f);
        checkOff->setScale(0.6f);

        auto toggler = CCMenuItemToggler::create(
            checkOff,
            checkOn,
            this,
            menu_selector(LevelCell::onToggle)
        );
        m_toggler = toggler;
        toggler->toggle(level->m_selected);
        toggler->setSizeMult(1.2f);
        m_mainMenu->addChild(toggler);

        auto pos = m_point + cocos2d::CCPoint(-50.0f, 0.0f);
        toggler->setPosition(m_mainMenu->convertToNodeSpace(pos));
    }
}

void LevelCell::loadCustomLevelCell() {
    bool isDaily = (m_level->m_dailyID.random - m_level->m_dailyID.seed) > 0;
    int levelLength = m_level->m_levelLength;

    float offsetX = m_compactView ? 15.0f : 0.0f;

    // Level name
    auto nameLabel = cocos2d::CCLabelBMFont::create();
    nameLabel->setString(m_level->m_levelName.c_str());
    m_mainLayer->addChild(nameLabel);
    nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    nameLabel->setScale(1.0f);

    if (m_compactView) {
        nameLabel->setPosition(offsetX + 50.0f, m_height * 0.5f + 16.0f);
        nameLabel->limitLabelWidth(120.0f, 0.5f, 0.0f);
    } else {
        nameLabel->setPosition(50.0f, m_height * 0.5f + 30.0f);
        nameLabel->limitLabelWidth(190.0f, 0.8f, 0.0f);
    }

    // Check if level is downloaded/up to date
    GJGameLevel* savedLevel = nullptr;
    if (isDaily) {
        savedLevel = GameLevelManager::sharedState()->getSavedDailyLevel(
            m_level->m_dailyID.random - m_level->m_dailyID.seed
        );
    } else {
        savedLevel = GameLevelManager::sharedState()->getSavedLevel(
            m_level->m_levelID.random - m_level->m_levelID.seed
        );
    }

    bool isUpToDate = true;
    if (m_level->m_levelType != LevelType::Editor) {
        std::string levelString = m_level->m_levelString;
        if (levelString.empty() || !savedLevel || savedLevel->m_levelNotDownloaded || 
            savedLevel->m_levelVersion < m_level->m_levelVersion) {
            isUpToDate = false;
        }
    }

    const char* buttonText = "View";
    const char* buttonBg = "GJ_button_01.png";

    if (!isUpToDate) {
        if (m_level->m_gameVersion > 22) {
            buttonText = "NA";
            buttonBg = "GJ_button_04.png";
        } else if (!savedLevel || savedLevel->m_levelNotDownloaded || 
                   savedLevel->m_levelVersion >= m_level->m_levelVersion) {
            buttonText = "Get It";
            buttonBg = "GJ_button_02.png";
        } else {
            buttonText = "Update";
            buttonBg = "GJ_button_03.png";
        }
    }

    cocos2d::CCNode* buttonSprite = nullptr;
    float extraOffset = 0.0f;
    bool hasCompleted = GameStatsManager::sharedState()->hasCompletedLevel(m_level);

    if (isDaily) {
        auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        sprite->setScale(0.7f);
        buttonSprite = sprite;
        extraOffset = 18.0f;
    } else {
        float scale = m_compactView ? 0.6f : 0.8f;
        auto sprite = ButtonSprite::create(buttonText, 0, 1, scale, "bigFont.fnt", buttonBg, nullptr, 30.0f);
        buttonSprite = sprite;
        extraOffset = 0.0f;
    }

    auto menuItem = CCMenuItemSpriteExtra::create(
        buttonSprite,
        nullptr,
        this,
        menu_selector(LevelCell::onClick)
    );
    m_button = menuItem;
    menuItem->setSizeMult((isDaily || m_level->m_localOrSaved) ? 1.0f : 1.4f);
    menuItem->setTag(0);
    m_mainMenu->addChild(menuItem);

    auto spriteSize = buttonSprite->getContentSize();
    m_point = cocos2d::CCPoint(
        m_width - (spriteSize.width * 0.5f) - 9.0f + extraOffset,
        m_height * 0.5f
    );
    menuItem->setPosition(m_mainMenu->convertToNodeSpace(m_point));

    // Move buttons for Level4 list type
    if (m_listType == 4) {
        auto moveUpBtn = ButtonSprite::create("", 0, 1, 0.6f, "bigFont.fnt", "GJ_button_01.png", nullptr, 30.0f);
        moveUpBtn->setScale(0.6f);
        auto upBtn = CCMenuItemSpriteExtra::create(
            moveUpBtn,
            nullptr,
            this,
            menu_selector(LevelCell::onClick)
        );
        upBtn->setTag(1);
        m_mainMenu->addChild(upBtn);
        upBtn->setPosition(m_button->getPosition() + cocos2d::CCPoint(-40.0f, -8.0f));

        auto moveDownBtn = ButtonSprite::create("", 0, 1, 0.6f, "bigFont.fnt", "GJ_button_06.png", nullptr, 30.0f);
        moveDownBtn->setScale(0.6f);
        auto downBtn = CCMenuItemSpriteExtra::create(
            moveDownBtn,
            nullptr,
            this,
            menu_selector(LevelCell::onClick)
        );
        downBtn->setTag(2);
        m_mainMenu->addChild(downBtn);
        downBtn->setPosition(m_button->getPosition() + cocos2d::CCPoint(-12.0f, -8.0f));

        auto flipBtn = ButtonSprite::create("", 0, 1, 0.6f, "bigFont.fnt", "GJ_button_05.png", nullptr, 30.0f);
        flipBtn->setScale(0.6f);
        auto flipBtnItem = CCMenuItemSpriteExtra::create(
            flipBtn,
            nullptr,
            this,
            menu_selector(LevelCell::onClick)
        );
        flipBtnItem->setTag(3);
        m_mainMenu->addChild(flipBtnItem);
        flipBtnItem->setPosition(m_button->getPosition() + cocos2d::CCPoint(16.0f, -8.0f));

        auto deleteBtn = ButtonSprite::create("Delete", 0, 1, 0.6f, "bigFont.fnt", "GJ_button_06.png", nullptr, 30.0f);
        deleteBtn->setScale(0.6f);
        auto deleteBtnItem = CCMenuItemSpriteExtra::create(
            deleteBtn,
            nullptr,
            this,
            menu_selector(LevelCell::onClick)
        );
        deleteBtnItem->setTag(4);
        m_mainMenu->addChild(deleteBtnItem);
        deleteBtnItem->setPosition(m_mainMenu->convertToNodeSpace(m_point + cocos2d::CCPoint(-extraOffset, 0.0f)));

        updateCellMode(m_cellMode);
    }

    // Creator name
    int accountID = m_level->m_accountID.random - m_level->m_accountID.seed;
    if (accountID <= 0) {
        accountID = GameLevelManager::sharedState()->accountIDForUserID(
            m_level->m_userID.random - m_level->m_userID.seed
        );
    }

    std::string creatorText = m_compactView ? "" : "By ";
    creatorText += m_level->m_creatorName;

    auto creatorLabel = cocos2d::CCLabelBMFont::create();
    creatorLabel->setString(creatorText.c_str());
    m_mainMenu->addChild(creatorLabel);

    if (accountID <= 0) {
        creatorLabel->setColor(cocos2d::ccc3(90, 255, 90));
        creatorLabel->setEnabled(false);
    }

    auto creatorBtn = CCMenuItemSpriteExtra::create(
        creatorLabel,
        nullptr,
        this,
        menu_selector(LevelCell::onViewProfile)
    );
    m_mainMenu->addChild(creatorBtn);

    if (m_compactView) {
        creatorLabel->limitLabelWidth(80.0f, 0.45f, 0.0f);
        auto namePos = nameLabel->getPosition();
        auto labelWidth = nameLabel->getContentSize().width * nameLabel->getScale();
        auto creatorWidth = creatorLabel->getContentSize().width * creatorLabel->getScale();
        creatorBtn->setPosition(m_mainMenu->convertToNodeSpace(
            namePos + cocos2d::CCPoint(labelWidth + creatorWidth + 5.0f, 0.0f)
        ));
    } else {
        creatorLabel->limitLabelWidth(140.0f, 0.7f, 0.0f);
        auto namePos = nameLabel->getPosition();
        auto creatorWidth = creatorLabel->getContentSize().width * creatorLabel->getScale();
        creatorBtn->setPosition(m_mainMenu->convertToNodeSpace(
            namePos + cocos2d::CCPoint(creatorWidth + 2.0f, -22.0f)
        ));
    }

    // Song name
    cocos2d::CCPoint songPos;
    float songWidth = m_compactView ? 150.0f : 195.0f;
    float songScale = m_compactView ? 0.36f : 0.45f;

    if (m_compactView) {
        songPos = cocos2d::CCPoint(nameLabel->getPosition().x + 1.0f, m_height * 0.5f - 1.0f);
    } else {
        songPos = cocos2d::CCPoint(nameLabel->getPosition().x + 2.0f, m_height * 0.5f - 12.0f);
    }

    auto songInfo = MusicDownloadManager::sharedState()->getSongInfoObject(m_level->m_songID);
    if (songInfo && songInfo->m_nongType > 0) {
        const char* iconName = (songInfo->m_nongType == 2) ? "chompo_small_001.png" : "ncs_small_001.png";
        auto icon = cocos2d::CCSprite::createWithSpriteFrameName(iconName);
        m_mainLayer->addChild(icon);

        if (songInfo->m_nongType == 2) {
            icon->setPosition(songPos + cocos2d::CCPoint(14.0f, -0.5f));
            songPos.x += 40.0f;
            songWidth -= 20.0f;
        } else {
            icon->setPosition(songPos + cocos2d::CCPoint(14.0f, -1.0f));
            songPos.x += 34.0f;
            songWidth -= 15.0f;
        }
    }

    auto songLabel = cocos2d::CCLabelBMFont::create();
    songLabel->setString(m_level->getSongName().c_str());
    songLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    songLabel->setPosition(songPos);
    songLabel->limitLabelWidth(songWidth, songScale, 0.0f);
    m_mainLayer->addChild(songLabel);

    if (m_level->m_songID <= 0) {
        songLabel->setColor(cocos2d::ccc3(210, 210, 210));
    } else {
        songLabel->setColor(cocos2d::ccc3(80, 80, 255));
    }

    // Difficulty sprite
    auto difficultyNode = cocos2d::CCNode::create();
    m_mainLayer->addChild(difficultyNode, 2);
    difficultyNode->setContentSize(cocos2d::CCSize(40.0f, 40.0f));
    difficultyNode->setPosition(26.0f, m_height * 0.5f);

    if (m_compactView) {
        difficultyNode->setScale(0.7f);
        difficultyNode->setPosition(difficultyNode->getPosition() + cocos2d::CCPoint(18.0f, 1.0f));
    }

    int difficulty = m_level->getAverageDifficulty();
    if (m_level->m_demon.random - m_level->m_demon.seed == 1) {
        difficulty = m_level->demonIconForDifficulty((DemonDifficultyType)m_level->m_demonDifficulty);
    }
    if (m_level->m_autoLevel) {
        difficulty = -1;
    }

    cocos2d::CCPoint diffOffset(0.0f, 0.0f);
    if (m_level->m_coins > 0 && !m_compactView && m_level->m_stars.random - m_level->m_stars.seed > 0) {
        diffOffset.y += 14.0f;
    } else if (m_level->m_coins > 0 && !m_compactView) {
        diffOffset.y += 5.0f;
    }
    if (isDaily) {
        diffOffset.y += 10.0f;
    }

    auto diffSprite = GJDifficultySprite::create(difficulty, DifficultySize::Short);
    diffSprite->updateFeatureStateFromLevel(m_level);
    difficultyNode->addChild(diffSprite, 3);
    diffSprite->setPosition(diffOffset);

    // Stars/Diamonds
    bool hasCompletedStar = GameStatsManager::sharedState()->hasCompletedStarLevel(m_level);
    int stars = m_level->m_stars.random - m_level->m_stars.seed;

    if (stars > 0) {
        const char* iconName = (levelLength == 5) ? "moon_small01_001.png" : "star_small01_001.png";
        auto starIcon = cocos2d::CCSprite::createWithSpriteFrameName(iconName);
        difficultyNode->addChild(starIcon);
        starIcon->setPosition(diffOffset + cocos2d::CCPoint(8.0f, -30.0f));

        auto starLabel = cocos2d::CCLabelBMFont::create();
        starLabel->setString(std::to_string(stars).c_str());
        starLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        starLabel->setScale(0.4f);
        difficultyNode->addChild(starLabel);
        starLabel->setPosition(starIcon->getPosition() + cocos2d::CCPoint(-8.0f, 0.0f));

        if (hasCompletedStar) {
            starLabel->setColor(cocos2d::ccc3(255, 255, 50));
        }
    }

    // Diamonds for daily levels
    if (isDaily || hasCompletedStar) {
        auto diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
        difficultyNode->addChild(diamondIcon);

        int baseDiamonds = GameStatsManager::sharedState()->getBaseDiamonds(stars);
        int awardedDiamonds = GameStatsManager::sharedState()->getAwardedDiamondsForLevel(m_level);

        auto diamondLabel = cocos2d::CCLabelBMFont::create();
        diamondLabel->setString(std::to_string(awardedDiamonds) + "/" + std::to_string(baseDiamonds));
        diamondLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        diamondLabel->setScale(0.35f);
        difficultyNode->addChild(diamondLabel);

        float labelWidth = diamondLabel->getContentSize().width * diamondLabel->getScale();
        diamondIcon->setPosition(diffOffset + cocos2d::CCPoint(labelWidth * 0.5f + 2.0f, -44.5f));
        diamondLabel->setPosition(diamondIcon->getPosition() + cocos2d::CCPoint(-8.0f, 0.0f));

        if (baseDiamonds == awardedDiamonds) {
            diamondLabel->setColor(cocos2d::ccc3(100, 255, 100));
        }
    }

    // Coins
    int coins = m_level->m_coins;
    if (coins > 0) {
        float yOffset = (stars > 0) ? -60.0f : -45.5f;
        for (int i = 1; i <= coins && i <= 3; ++i) {
            float xOffset = 0.0f;
            if (coins == 2) {
                xOffset = (i == 1) ? -5.0f : 5.0f;
            } else if (coins > 2) {
                xOffset = (i == 1) ? -10.0f : (i == 2) ? 0.0f : 10.0f;
            }

            std::string coinKey = GJGameLevel::getCoinKey(m_level, i);
            bool hasCoin = GameStatsManager::sharedState()->hasUserCoin(coinKey) ||
                           GameStatsManager::sharedState()->hasPendingUserCoin(coinKey);

            auto coinSprite = cocos2d::CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");

            if (m_compactView) {
                m_mainLayer->addChild(coinSprite);
                coinSprite->setScale(0.7f);
                auto namePos = nameLabel->getPosition();
                auto nameWidth = nameLabel->getContentSize().width * nameLabel->getScale();
                coinSprite->setPosition(namePos + cocos2d::CCPoint(nameWidth + 18.0f + xOffset * coinSprite->getScale(), 0.0f));
            } else {
                difficultyNode->addChild(coinSprite);
                coinSprite->setPosition(diffOffset + cocos2d::CCPoint(xOffset, yOffset));
            }

            if (hasCoin) {
                coinSprite->setColor(cocos2d::ccc3(255, 255, 75));
            } else {
                coinSprite->setColor(cocos2d::ccc3(165, 165, 165));
            }
        }
    }

    // Stats icons (time, downloads, likes)
    float iconScale = m_compactView ? 0.4f : 0.6f;
    float iconSpacing = m_compactView ? 60.0f : 80.0f;
    float yPos = m_compactView ? 8.5f : 14.0f;
    float xStart = nameLabel->getPosition().x + (m_compactView ? 6.0f : 9.0f);

    auto timeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
    m_mainLayer->addChild(timeIcon);
    timeIcon->setPosition(xStart, yPos);
    timeIcon->setScale(iconScale);

    auto dlIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
    m_mainLayer->addChild(dlIcon);
    dlIcon->setPosition(timeIcon->getPosition() + cocos2d::CCPoint(iconSpacing, 0.0f));
    dlIcon->setScale(iconScale);

    auto likeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_likesIcon_001.png");
    m_mainLayer->addChild(likeIcon);
    likeIcon->setPosition(dlIcon->getPosition() + cocos2d::CCPoint(iconSpacing, 0.0f));
    likeIcon->setScale(iconScale);

    // Length label
    float labelScale = m_compactView ? 0.3f : 0.4f;
    float labelOffset = m_compactView ? 20.0f : 25.0f;

    if (m_compactView && stars <= 0) {
        labelOffset = 20.0f;
    }

    auto lengthLabel = cocos2d::CCLabelBMFont::create();
    lengthLabel->setString(GJGameLevel::lengthKeyToString(levelLength));
    lengthLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    lengthLabel->setScale(labelScale);
    m_mainLayer->addChild(lengthLabel);

    auto timePos = timeIcon->getPosition();
    lengthLabel->setPosition(timePos + cocos2d::CCPoint(10.0f, 0.0f));
    lengthLabel->limitLabelWidth(50.0f, labelScale, 0.0f);

    // Downloads label
    auto dlLabel = cocos2d::CCLabelBMFont::create();
    dlLabel->setString(GameToolbox::intToShortString(m_level->m_downloads).c_str());
    dlLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    dlLabel->setScale(labelScale);
    m_mainLayer->addChild(dlLabel);

    auto dlPos = dlIcon->getPosition();
    dlLabel->setPosition(dlPos + cocos2d::CCPoint(9.0f, 0.0f));
    dlLabel->limitLabelWidth(45.0f, labelScale, 0.0f);

    // Likes label
    int likeCount = m_level->m_likes - m_level->m_dislikes;
    const char* likeIconName = (likeCount < 0) ? "GJ_dislikesIcon_001.png" : "GJ_likesIcon_001.png";
    likeIcon->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(likeIconName)
    );

    auto likeLabel = cocos2d::CCLabelBMFont::create();
    likeLabel->setString(GameToolbox::intToShortString(likeCount).c_str());
    likeLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    likeLabel->setScale(labelScale);
    m_mainLayer->addChild(likeLabel);

    auto likePos = likeIcon->getPosition();
    likeLabel->setPosition(likePos + cocos2d::CCPoint(10.0f, 0.0f));
    likeLabel->limitLabelWidth(45.0f, labelScale, 0.0f);

    // Orbs
    if (stars > 1) {
        auto orbIcon = cocos2d::CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
        m_mainLayer->addChild(orbIcon);
        orbIcon->setPosition(likeIcon->getPosition() + cocos2d::CCPoint(iconSpacing, 0.0f));
        orbIcon->setScale(iconScale);

        int baseCurrency = GameStatsManager::sharedState()->getBaseCurrencyForLevel(m_level);
        int maxCurrency = baseCurrency + (int)(baseCurrency * 0.25f);
        int awardedCurrency = GameStatsManager::sharedState()->getAwardedCurrencyForLevel(m_level);

        std::string orbText;
        if (maxCurrency <= awardedCurrency) {
            orbText = std::to_string(maxCurrency);
        } else {
            orbText = std::to_string(awardedCurrency) + "/" + std::to_string(maxCurrency);
        }

        auto orbLabel = cocos2d::CCLabelBMFont::create();
        orbLabel->setString(orbText.c_str());
        orbLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        orbLabel->setScale(labelScale);
        m_mainLayer->addChild(orbLabel);

        auto orbPos = orbIcon->getPosition();
        orbLabel->setPosition(orbPos + cocos2d::CCPoint(10.0f, 0.0f));
        orbLabel->limitLabelWidth(45.0f, labelScale, 0.0f);

        if (maxCurrency == awardedCurrency) {
            orbLabel->setColor(cocos2d::ccc3(100, 255, 100));
        }

        likeIcon->setPosition(likeIcon->getPosition() + cocos2d::CCPoint(0.0f, 1.0f));
    }

    // Completion/Progress icon
    cocos2d::CCPoint progressPos;
    float nameEnd = nameLabel->getPosition().x + nameLabel->getContentSize().width * nameLabel->getScale() + 10.0f;
    progressPos = cocos2d::CCPoint(nameEnd, nameLabel->getPosition().y - 1.0f);

    if (m_compactView) {
        progressPos = cocos2d::CCPoint(16.0f, 10.0f);
    }

    if (hasCompleted) {
        auto completeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        if (!m_compactView) {
            completeIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        }
        m_mainLayer->addChild(completeIcon);
        completeIcon->setPosition(progressPos);
        completeIcon->setScale(m_compactView ? 0.5f : 0.8f);
    } else if (savedLevel) {
        int normalPercent = savedLevel->getNormalPercent();
        if (normalPercent > 0) {
            auto progressLabel = cocos2d::CCLabelBMFont::create();
            progressLabel->setString(std::to_string(normalPercent) + "%");
            progressLabel->setScale(m_compactView ? 0.5f : 0.8f);
            m_mainLayer->addChild(progressLabel, 10);

            if (!m_compactView) {
                progressLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
            }
            progressLabel->setPosition(progressPos + cocos2d::CCPoint(m_compactView ? 1.0f : 0.0f, 0.0f));
        }
    }

    // Collaboration icon
    if (m_level->m_originalLevel.random != m_level->m_originalLevel.seed) {
        float iconSize = m_compactView ? 0.8f : 1.0f;
        auto collabIcon = cocos2d::CCSprite::createWithSpriteFrameName("collaborationIcon_001.png");
        collabIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        m_mainLayer->addChild(collabIcon);

        float xPos = creatorLabel->getPosition().x + 
                     (creatorLabel->getContentSize().width * 0.5f * creatorLabel->getScale()) + 
                     (iconSize * 6.0f);
        collabIcon->setPosition(xPos, creatorLabel->getPosition().y - 1.0f);
        collabIcon->setScale(iconSize);
    }

    // High object icon
    if (m_level->m_objectCount.random - m_level->m_objectCount.seed > 40000) {
        float iconSize = m_compactView ? 0.8f : 1.0f;
        float xOffset = m_compactView ? 14.4f : 18.0f;
        auto highObjIcon = cocos2d::CCSprite::createWithSpriteFrameName("highObjectIcon_001.png");
        highObjIcon->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        m_mainLayer->addChild(highObjIcon, 10);

        float xPos = creatorLabel->getPosition().x + 
                     (creatorLabel->getContentSize().width * 0.5f * creatorLabel->getScale()) + 
                     (iconSize * 6.0f);
        if (m_level->m_originalLevel.random != m_level->m_originalLevel.seed) {
            xPos += 15.0f;
        }
        highObjIcon->setPosition(xPos, creatorLabel->getPosition().y - 1.0f);
        highObjIcon->setScale(iconSize);
    }
}

void LevelCell::loadLocalLevelCell() {
    // Level name
    auto nameLabel = cocos2d::CCLabelBMFont::create();
    nameLabel->setString(m_level->m_levelName.c_str());
    m_mainLayer->addChild(nameLabel);
    nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    nameLabel->setScale(1.0f);
    nameLabel->setPosition(12.0f, m_height * 0.5f + 14.0f);

    float nameWidth = nameLabel->getContentSize().width;
    if (nameWidth > 200.0f) {
        nameLabel->setScale(200.0f / nameWidth);
    }
    if (nameLabel->getScale() > 0.8f) {
        nameLabel->setScale(0.8f);
    }

    // Revision label
    if (m_level->m_levelRev > 0) {
        auto revLabel = cocos2d::CCLabelBMFont::create();
        revLabel->setString(("Rev. " + std::to_string(m_level->m_levelRev)).c_str());
        revLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
        revLabel->setScale(0.4f);
        m_mainLayer->addChild(revLabel);

        auto namePos = nameLabel->getPosition();
        auto nameSize = nameLabel->getContentSize();
        revLabel->setPosition(
            namePos.x + nameLabel->getScale() * nameSize.width + 10.0f,
            namePos.y + nameLabel->getScale() * (nameSize.height * -0.5f)
        );
    }

    // View button
    auto viewBtn = ButtonSprite::create("View", 0, 0, 0.6f, "bigFont.fnt", "GJ_button_01.png", nullptr, 30.0f);
    auto menuItem = CCMenuItemSpriteExtra::create(
        viewBtn,
        nullptr,
        this,
        menu_selector(LevelCell::onClick)
    );
    menuItem->setSizeMult(m_level->m_localOrSaved ? 1.0f : 1.5f);
    m_mainMenu->addChild(menuItem);

    auto btnSize = viewBtn->getContentSize();
    m_point = cocos2d::CCPoint(
        m_width - (btnSize.width * 0.5f) - 9.0f,
        m_height * 0.5f
    );
    menuItem->setPosition(m_mainMenu->convertToNodeSpace(m_point));

    // Time icon
    float xPos = nameLabel->getPosition().x + 9.0f;
    auto timeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
    m_mainLayer->addChild(timeIcon);
    timeIcon->setPosition(xPos, 20.0f);
    timeIcon->setScale(0.7f);

    // Length label
    auto lengthLabel = cocos2d::CCLabelBMFont::create();
    lengthLabel->setString(GJGameLevel::lengthKeyToString(m_level->m_levelLength));
    lengthLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(lengthLabel);

    auto timePos = timeIcon->getPosition();
    lengthLabel->setPosition(timePos + cocos2d::CCPoint(9.0f, 0.0f));
    float lengthScale = 100.0f / lengthLabel->getContentSize().width;
    if (lengthScale > 0.4f) {
        lengthLabel->setScale(0.4f);
    } else {
        lengthLabel->setScale(lengthScale);
    }

    // Note icon
    auto noteIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_noteIcon_001.png");
    m_mainLayer->addChild(noteIcon);
    float noteX = lengthLabel->getPosition().x + lengthLabel->getContentSize().width * lengthLabel->getScale() + 17.0f;
    noteIcon->setPosition(noteX, lengthLabel->getPosition().y);
    noteIcon->setScale(0.7f);

    // Song label
    auto songLabel = cocos2d::CCLabelBMFont::create();
    songLabel->setString(m_level->getSongName().c_str());
    songLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(songLabel);

    auto notePos = noteIcon->getPosition();
    songLabel->setPosition(notePos + cocos2d::CCPoint(9.0f, 0.0f));
    float songScale = 100.0f / songLabel->getContentSize().width;
    if (songScale > 0.4f) {
        songLabel->setScale(0.4f);
    } else {
        songLabel->setScale(songScale);
    }

    // Info icon
    auto infoIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    m_mainLayer->addChild(infoIcon);
    float infoX = songLabel->getPosition().x + songLabel->getContentSize().width * songLabel->getScale() + 17.0f;
    infoIcon->setPosition(infoX, songLabel->getPosition().y);
    infoIcon->setScale(0.7f);

    // Info label
    auto infoLabel = cocos2d::CCLabelBMFont::create();
    infoLabel->setString("i");
    infoLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(infoLabel);

    auto infoPos = infoIcon->getPosition();
    infoLabel->setPosition(infoPos + cocos2d::CCPoint(9.0f, 0.0f));
    float infoScale = 60.0f / infoLabel->getContentSize().width;
    if (infoScale > 0.4f) {
        infoLabel->setScale(0.4f);
    } else {
        infoLabel->setScale(infoScale);
    }
}

void LevelCell::onClick(cocos2d::CCObject* sender) {
    if (m_listType == 4 && sender->getTag() != 0) {
        int action = sender->getTag() - 1;
        if (action >= 0 && action <= 3) {
            if (m_tableView && m_tableView->m_cellDelegate) {
                m_tableView->m_cellDelegate->cellPerformedAction(this, action);
            }
        }
        return;
    }

    if (m_level->m_gameVersion <= 22) {
        GameLevelManager::sharedState()->gotoLevelPage(m_level);
    } else {
        auto alert = FLAlertLayer::create(
            nullptr,
            "Unavailable",
            "Please update your application to download this level.",
            "Close",
            nullptr,
            nullptr
        );
        alert->show();
    }
}

void LevelCell::onToggle(cocos2d::CCObject* sender) {
    if (m_level) {
        m_level->m_selected = !m_level->m_selected;
    }
}

void LevelCell::onViewProfile(cocos2d::CCObject* sender) {
    int accountID = m_level->m_accountID.random - m_level->m_accountID.seed;
    if (accountID > 0) {
        auto profile = ProfilePage::create(accountID, false);
        profile->show();
    }
}

void LevelCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

void LevelCell::updateCellMode(int mode) {
    m_cellMode = mode;

    for (int i = 0; i < 5; ++i) {
        auto node = m_mainMenu->getChildByTag(i);
        if (node) {
            bool visible = false;
            if (i == 0) {
                visible = (mode == 0);
            } else if (i == 4) {
                visible = (mode == 2);
            } else {
                visible = (mode == 1);
            }
            node->setVisible(visible);
        }
    }
}

void LevelCell::updateToggle() {
    if (m_level && m_toggler) {
        m_toggler->toggle(m_level->m_selected);
    }
}

LevelCell::~LevelCell() {
    // Destructor
}