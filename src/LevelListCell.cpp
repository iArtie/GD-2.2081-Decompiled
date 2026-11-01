#include "LevelListCell.h"
#include "GJLevelList.h"
#include "GameLevelManager.h"
#include "GameStatsManager.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "ProfilePage.h"
#include "LevelListLayer.h"
#include "GJEnums.h"

LevelListCell::LevelListCell(const char* identifier, float width, float height) : TableViewCell(identifier, width, height) {
    m_width = width;
    m_height = height;
    m_levelList = nullptr;
    m_addingLevel = false;
    init();
}

LevelListCell* LevelListCell::create(float width, float height) {
    LevelListCell* cell = new LevelListCell(" ", width, height);
    if (cell && cell->init()) {
        cell->autorelease();
        return cell;
    }
    CC_SAFE_DELETE(cell);
    return nullptr;
}

void LevelListCell::draw() {
    cocos2d::CCSize size(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::CCPoint p1(1.0f, size.height);
    cocos2d::CCPoint p2(size.width - 1.0f, size.height);
    cocos2d::ccDrawLine(p1, p2);
    cocos2d::CCPoint p3(1.0f, 0.0f);
    cocos2d::CCPoint p4(size.width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(p3, p4);
}

bool LevelListCell::init() {
    return true;
}

void LevelListCell::loadFromList(GJLevelList* list) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    if (m_levelList) {
        m_levelList->release();
    }
    m_levelList = list;
    m_levelList->retain();

    std::string difficultyFrame = GJLevelList::frameForListDifficulty(m_levelList->m_difficulty, DifficultyIconType::ShortText);
    cocos2d::CCSprite* difficultySprite = cocos2d::CCSprite::createWithSpriteFrameName(difficultyFrame.c_str());
    difficultySprite->setPosition(cocos2d::CCPoint(26.0f, m_height * 0.5f));
    m_mainLayer->addChild(difficultySprite, 2);

    int completedLevels = GJLevelList::completedLevels(m_levelList);
    int totalLevels = GJLevelList::totalLevels(m_levelList);
    bool hasClaimedReward = false;
    int levelsToClaim = 0;

    if (list->m_levelsToClaim > 0 && list->m_diamonds > 0) {
        hasClaimedReward = GameStatsManager::sharedState()->hasClaimedListReward(m_levelList);

        difficultySprite->setPosition(difficultySprite->getPosition() + cocos2d::CCPoint(0.0f, 5.0f));

        cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
        m_mainLayer->addChild(diamondIcon);
        diamondIcon->setPosition(difficultySprite->getPosition() + cocos2d::CCPoint(8.0f, -30.0f));

        std::string diamondStr = GameToolbox::intToString(m_levelList->m_diamonds);
        cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create(diamondStr.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(diamondLabel);
        diamondLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        diamondLabel->setPosition(diamondIcon->getPosition() + cocos2d::CCPoint(-8.0f, 0.0f));
        diamondLabel->setScale(0.4f);

        if (hasClaimedReward) {
            diamondLabel->setColor(cocos2d::ccc3(255, 255, 50));
        } else {
            levelsToClaim = m_levelList->m_levelsToClaim;
        }
    }

    if (m_levelList->m_featured) {
        cocos2d::CCSprite* featuredIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
        m_mainLayer->addChild(featuredIcon, 1);
        featuredIcon->setPosition(difficultySprite->getPosition());
    }

    std::string listName = m_levelList->m_listName;
    cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create(listName.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(nameLabel);
    nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    nameLabel->setScale(1.0f);
    nameLabel->setPosition(cocos2d::CCPoint(50.0f, m_height * 0.5f + 30.0f));
    nameLabel->limitLabelWidth(190.0f, 0.7f, 0.0f);

    if (m_levelList->m_listRevision > 0) {
        std::string revStr = cocos2d::CCString::createWithFormat("Rev. %i", m_levelList->m_listRevision)->getCString();
        cocos2d::CCLabelBMFont* revLabel = cocos2d::CCLabelBMFont::create(revStr.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(revLabel);
        revLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        revLabel->setScale(0.4f);
        revLabel->setPosition(nameLabel->getPosition() + cocos2d::CCPoint(
            nameLabel->getContentSize().width * nameLabel->getScale() + 30.0f, -1.0f));
    }

    const char* btnText = m_addingLevel ? "Add" : "View";
    const char* btnFrame = m_addingLevel ? "GJ_button_02.png" : "GJ_button_01.png";
    ButtonSprite* buttonSprite = ButtonSprite::create(btnText, "bigFont.fnt", btnFrame, 30.0f);
    CCMenuItemSpriteExtra* actionBtn = CCMenuItemSpriteExtra::create(buttonSprite, nullptr, this,
        menu_selector(LevelListCell::onClick));
    cocos2d::CCMenu* actionMenu = cocos2d::CCMenu::create(actionBtn, nullptr);
    m_mainLayer->addChild(actionMenu, 3);
    actionMenu->setPosition(cocos2d::CCPoint(
        m_width - buttonSprite->getContentSize().width * 0.5f - 9.0f,
        m_height * 0.5f));

    std::string creatorStr = cocos2d::CCString::createWithFormat("By %s", m_levelList->m_creatorName.c_str())->getCString();
    cocos2d::CCLabelBMFont* creatorLabel = cocos2d::CCLabelBMFont::create(creatorStr.c_str(), "bigFont.fnt");
    creatorLabel->limitLabelWidth(140.0f, 0.7f, 0.0f);
    CCMenuItemSpriteExtra* profileBtn = CCMenuItemSpriteExtra::create(creatorLabel, nullptr, this,
        menu_selector(LevelListCell::onViewProfile));
    actionMenu->addChild(profileBtn);
    cocos2d::CCPoint creatorWorldPos = nameLabel->getPosition() + cocos2d::CCPoint(
        creatorLabel->getContentSize().width * creatorLabel->getScale() * 0.5f + 2.0f, -21.0f);
    profileBtn->setPosition(actionMenu->convertToNodeSpace(creatorWorldPos));

    cocos2d::CCSprite* progressBarBg = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    progressBarBg->setColor(cocos2d::ccc3(0, 0, 0));
    progressBarBg->setOpacity(125);
    progressBarBg->setScaleY(0.64f);
    progressBarBg->setScaleX(0.48f);
    m_mainLayer->addChild(progressBarBg, 3);
    progressBarBg->setPosition(cocos2d::CCPoint(m_width * 0.5f - 45.0f, m_height * 0.5f - 12.0f));

    cocos2d::CCSprite* progressBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    progressBar->setScaleX(0.978f);
    progressBar->setScaleY(0.76f);
    if (levelsToClaim > hasClaimedReward) {
        progressBar->setColor(cocos2d::ccc3(255, 255, 50));
    } else {
        progressBar->setColor(cocos2d::ccc3(50, 255, 50));
    }
    progressBarBg->addChild(progressBar, 1);
    progressBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    float barOffset = progressBar->getContentSize().width - progressBar->getContentSize().width * 0.978f;
    progressBar->setPosition(cocos2d::CCPoint(
        barOffset * 0.5f,
        progressBarBg->getContentSize().height * 0.5f));

    float fillPercent = (levelsToClaim <= 0) ? 0.0f : static_cast<float>(completedLevels) / static_cast<float>(levelsToClaim);
    float fillWidth = fillPercent * progressBar->getContentSize().width;
    if (fillWidth > progressBar->getContentSize().width) {
        fillWidth = progressBar->getContentSize().width;
    }
    progressBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, fillWidth, progressBar->getContentSize().height));

    std::string progressStr = cocos2d::CCString::createWithFormat("%i/%i", completedLevels, levelsToClaim)->getCString();
    cocos2d::CCLabelBMFont* progressLabel = cocos2d::CCLabelBMFont::create(progressStr.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(progressLabel, 4);
    progressLabel->setPosition(progressBarBg->getPosition() + cocos2d::CCPoint(0.0f, 0.5f));
    progressLabel->setScale(0.4f);

    if (m_levelList->m_listType == GJLevelType::Editor) {
        cocos2d::CCSprite* infoIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        m_mainLayer->addChild(infoIcon);
        infoIcon->setPosition(cocos2d::CCPoint(nameLabel->getPosition().x + 8.0f, 14.0f));
        infoIcon->setScale(0.6f);

        cocos2d::CCLabelBMFont* editorLabel = cocos2d::CCLabelBMFont::create("Editor", "bigFont.fnt");
        m_mainLayer->addChild(editorLabel);
        editorLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        editorLabel->setPosition(infoIcon->getPosition() + cocos2d::CCPoint(13.0f, 0.0f));
        editorLabel->setScale(0.4f);
    } else {
        float downloadsX = nameLabel->getPosition().x + 9.0f;

        cocos2d::CCSprite* downloadsIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
        m_mainLayer->addChild(downloadsIcon);
        downloadsIcon->setPosition(cocos2d::CCPoint(downloadsX, 14.0f));
        downloadsIcon->setScale(0.6f);

        cocos2d::CCSprite* likesIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_likesIcon_001.png");
        m_mainLayer->addChild(likesIcon);
        likesIcon->setPosition(downloadsIcon->getPosition() + cocos2d::CCPoint(80.0f, 0.0f));
        likesIcon->setScale(0.6f);

        std::string downloadsStr = GameToolbox::intToShortString(m_levelList->m_downloads);
        cocos2d::CCLabelBMFont* downloadsLabel = cocos2d::CCLabelBMFont::create(downloadsStr.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(downloadsLabel);
        downloadsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        downloadsLabel->setPosition(downloadsIcon->getPosition() + cocos2d::CCPoint(9.0f, 0.0f));
        downloadsLabel->setScale(0.4f);
        downloadsLabel->limitLabelWidth(45.0f, 0.4f, 0.0f);

        int likes = m_levelList->m_likes;
        if (likes < 0) {
            likesIcon->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_dislikesIcon_001.png"));
        } else {
            likesIcon->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_likesIcon_001.png"));
        }

        float likesX = downloadsLabel->getPosition().x +
            downloadsLabel->getContentSize().width * downloadsLabel->getScale() + 25.0f;
        likesIcon->setPosition(cocos2d::CCPoint(likesX, likesIcon->getPosition().y));

        std::string likesStr = GameToolbox::intToShortString(likes);
        cocos2d::CCLabelBMFont* likesLabel = cocos2d::CCLabelBMFont::create(likesStr.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(likesLabel);
        likesLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        likesLabel->setPosition(likesIcon->getPosition() + cocos2d::CCPoint(10.0f, 0.0f));
        likesLabel->setScale(0.4f);
        likesLabel->limitLabelWidth(45.0f, 0.4f, 0.0f);

        likesIcon->setPosition(likesIcon->getPosition() + cocos2d::CCPoint(0.0f, 1.0f));

        if (levelsToClaim > hasClaimedReward) {
            cocos2d::CCSprite* diamondIcon = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
            m_mainLayer->addChild(diamondIcon, 10);
            diamondIcon->setPosition(progressBarBg->getPosition() + cocos2d::CCPoint(73.0f, 0.0f));

            int total = GJLevelList::totalLevels(m_levelList);

            cocos2d::CCSprite* completesIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            m_mainLayer->addChild(completesIcon);
            completesIcon->setPosition(downloadsIcon->getPosition() + cocos2d::CCPoint(80.0f, 0.0f));
            completesIcon->setScale(0.6f);

            float completesX = likesLabel->getPosition().x +
                likesLabel->getContentSize().width * likesLabel->getScale() + 25.0f;
            completesIcon->setPosition(cocos2d::CCPoint(completesX, completesIcon->getPosition().y));

            std::string completesStr = cocos2d::CCString::createWithFormat("%i/%i", completedLevels, total)->getCString();
            cocos2d::CCLabelBMFont* completesLabel = cocos2d::CCLabelBMFont::create(completesStr.c_str(), "bigFont.fnt");
            m_mainLayer->addChild(completesLabel);
            completesLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
            completesLabel->setPosition(completesIcon->getPosition() + cocos2d::CCPoint(10.0f, 0.0f));
            completesLabel->setScale(0.4f);
            completesLabel->limitLabelWidth(45.0f, 0.4f, 0.0f);
        }
    }

    cocos2d::CCSprite* infoIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoIcon->setScale(0.65f);
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoIcon, nullptr, this,
        menu_selector(LevelListCell::onListInfo));
    infoBtn->setSizeMult(1.2f);
    actionMenu->addChild(infoBtn);

    float infoX = nameLabel->getPosition().x +
        nameLabel->getContentSize().width * nameLabel->getScale() + 15.0f;
    cocos2d::CCPoint infoWorldPos(infoX, -1.0f);
    infoBtn->setPosition(actionMenu->convertToNodeSpace(infoWorldPos));
}

void LevelListCell::onClick(cocos2d::CCObject* sender) {
    if (m_addingLevel) {
        TableViewCellDelegate* delegate = m_tableView->m_cellDelegate;
        if (delegate) {
            delegate->cellPerformedAction(this, 30, CellAction::Click, nullptr);
        }
    } else if (m_levelList->m_listType == GJLevelType::Editor) {
        cocos2d::CCScene* scene = LevelListLayer::scene(m_levelList);
        cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
    } else {
        GameLevelManager::sharedState()->updateSavedLevelList(m_levelList);
        cocos2d::CCScene* scene = LevelListLayer::scene(m_levelList);
        cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->pushScene(fade);
    }
}

void LevelListCell::onListInfo(cocos2d::CCObject* sender) {
    if (m_levelList) {
        m_levelList->showListInfo();
    }
}

void LevelListCell::onViewProfile(cocos2d::CCObject* sender) {
    int accountID = m_levelList->m_accountID;
    if (accountID > 0) {
        ProfilePage* page = ProfilePage::create(accountID, false);
        page->show();
    }
}

void LevelListCell::updateBGColor(int index) {
    if ((index & 1) != 0) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

LevelListCell::~LevelListCell() {
}