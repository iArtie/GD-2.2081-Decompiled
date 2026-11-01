#include "Cells/CommentCell.h"
#include "Models/GJComment.h"
#include "Models/GJUserScore.h"
#include "Managers/GameManager.h"
#include "GameLevelManager.h"
#include "Managers/GJAccountManager.h"
#include "Sprites/SimplePlayer.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/FLAlertLayer.h"
#include "Layers/LikeItemLayer.h"
#include "Pages/ProfilePage.h"
#include "Layers/LevelBrowserLayer.h"
#include "Models/GJSearchObject.h"
#include "Platform/cocos2d.h"

CommentCell::CommentCell(char* charPtr, float value, float value2)
    : TableViewCell(charPtr, value, value2) {
    init();
}

void CommentCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (layer->getTag() == 0 && btn2) {
        onDelete();
    }
}

void CommentCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);

    cocos2d::CCPoint p1 = cocos2d::CCPoint(1.0f, size.height);
    cocos2d::CCPoint p2 = cocos2d::CCPoint(size.width - 1.0f, size.height);
    cocos2d::ccDrawLine(p1, p2);

    cocos2d::CCPoint p3 = cocos2d::CCPoint(1.0f, 0.0f);
    cocos2d::CCPoint p4 = cocos2d::CCPoint(size.width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(p3, p4);
}

void CommentCell::incrementDislikes() {
    if (m_comment) {
        m_comment->m_likeCount--;
        updateLabelValues();
    }
}

void CommentCell::incrementLikes() {
    if (m_comment) {
        m_comment->m_likeCount++;
        updateLabelValues();
    }
}

bool CommentCell::init() {
    m_iconSprite = nullptr;
    m_likeLabel = nullptr;
    m_comment = nullptr;
    m_compactMode = false;
    m_accountComment = false;
    return true;
}

void CommentCell::likedItem(LikeItemType type, int id, bool liked) {
    if (m_comment && m_comment->m_commentID == id) {
        if (liked) {
            incrementLikes();
        } else {
            incrementDislikes();
        }
    }
}

void CommentCell::loadFromComment(GJComment* comment) {
    m_backgroundLayer->setOpacity(255);
    comment->retain();

    if (m_comment) {
        m_comment->release();
    }

    m_comment = comment;
    bool isAccountComment = (comment->m_levelID == 0 && comment->m_accountID > 0);
    m_accountComment = isAccountComment;

    m_mainLayer->removeAllChildrenWithCleanup(true);

    if (m_comment->m_commentDeleted) {
        return;
    }

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(menu, 2);

    // Load username
    if (!comment->m_unkMultiplayerBool) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        std::string username = glm->userNameForUserID(comment->m_userID);

        if (m_accountComment) {
            glm->tryGetUsername(comment->m_accountID);
        }

        comment->m_userName = username;
    }

    int accountId = comment->m_accountID;
    bool isRobTop = (accountId == 71);

    // Username label
    cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create();
    nameLabel->setString(comment->m_userName.c_str());

    if (m_compactMode) {
        nameLabel->limitLabelWidth(150.0f, 0.5f, 0.0f);
    } else {
        float maxWidth = comment->m_hasLevelID ? 100.0f : 150.0f;
        nameLabel->limitLabelWidth(maxWidth, 0.7f, 0.0f);
    }

    // Player icon
    float iconOffset = 0.0f;
    if (!m_accountComment && comment->m_userScore) {
        SimplePlayer* player = SimplePlayer::create(1);
        m_mainLayer->addChild(player, 1);

        float scale = m_compactMode ? 0.7f : 1.15f;
        player->setScale(scale);
        float yPos = m_compactMode ? 11.0f : 19.5f;
        player->setPosition(cocos2d::CCPoint(20.0f, m_height - yPos));

        GJUserScore* score = comment->m_userScore;
        player->setColor(GameManager::sharedState()->colorForIdx(score->m_color1));
        player->setSecondColor(GameManager::sharedState()->colorForIdx(score->m_color2));
        player->tryEnableCustomGlowColor(score->m_color3);

        bool glowEnabled = (score->m_special == 2 || score->m_glowEnabled);
        player->m_hasGlowOutline = glowEnabled;
        player->updatePlayerFrame(score->m_iconID, score->m_iconType);
        player->updateColors();

        // Adjust position for specific icon types
        if (score->m_iconType == IconType::Wave) {
            player->setScale(player->getScale() * 1.15f);
            cocos2d::CCPoint pos = player->getPosition();
            pos.x += 1.0f;
            player->setPosition(pos);
        } else if (score->m_iconType == IconType::Ship || score->m_iconType == IconType::Spider) {
            player->setScale(player->getScale() * 0.9f);
        } else if (score->m_iconType == IconType::Ufo) {
            player->setScale(player->getScale() * 0.95f);
        }

        iconOffset = 26.0f;
    }

    // Mod badge
    float modOffset = 0.0f;
    if (comment->m_modBadge > 0) {
        float labelWidth = nameLabel->getContentSize().width * nameLabel->getScale();
        float xPos = iconOffset + 10.0f + labelWidth;
        float yPos = m_compactMode ? 11.5f : 19.5f;

        if (m_compactMode) {
            xPos += 8.0f;
        } else {
            xPos += 12.0f;
        }

        const char* badgeFile = (comment->m_modBadge == 2) ? "modBadge_02_001.png" : "modBadge_01_001.png";
        cocos2d::CCSprite* badge = cocos2d::CCSprite::createWithSpriteFrameName(badgeFile);
        badge->setScale(m_compactMode ? 0.8f : 0.75f);
        m_mainLayer->addChild(badge, 10);
        badge->setPosition(cocos2d::CCPoint(xPos, m_height - yPos));

        modOffset = m_compactMode ? 15.0f : 22.0f;
    }

    // Percentage label
    if (comment->m_percentage > 0) {
        cocos2d::CCLabelBMFont* percentLabel = cocos2d::CCLabelBMFont::create();
        percentLabel->setString(CCString::createWithFormat("%i%%", comment->m_percentage)->getCString());
        m_mainLayer->addChild(percentLabel);
        percentLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        percentLabel->setScale(m_compactMode ? 0.5f : 0.7f);
        percentLabel->setColor(cocos2d::ccc3(180, 200, 230));
        percentLabel->setOpacity(255);

        float xPos = iconOffset + 10.0f + modOffset +
                     (nameLabel->getContentSize().width * nameLabel->getScale()) + 4.0f;
        float yPos = m_compactMode ? 11.5f : 19.5f;
        percentLabel->setPosition(cocos2d::CCPoint(xPos, m_height - yPos));
    }

    // Name label position
    float yOffset = m_compactMode ? 10.0f : 18.0f;
    if (!isAccountComment && comment->m_hasLevelID) {
        m_mainLayer->addChild(nameLabel);
        nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        nameLabel->setPosition(cocos2d::CCPoint(iconOffset + 10.0f, m_height - yOffset));

        if (accountId != 71 && !m_accountComment) {
            nameLabel->setColor(cocos2d::ccc3(90, 180, 255));
        }
    } else {
        // Make username clickable for profile
        CCMenuItemSpriteExtra* nameItem = CCMenuItemSpriteExtra::create(
            nameLabel, nullptr, this, menu_selector(CommentCell::onViewProfile)
        );
        menu->addChild(nameItem);

        float xPos = iconOffset + 10.0f + (nameLabel->getContentSize().width * nameLabel->getScale() * 0.5f);
        float yPos = m_height - yOffset;
        nameItem->setPosition(xPos, yPos);
    }

    // Comment text container
    float textWidth = m_width - (m_compactMode ? 8.0f : 10.0f);
    float textHeight = m_height - (m_compactMode ? 8.0f : 12.0f);
    cocos2d::CCPoint center = cocos2d::CCPoint(m_width * 0.5f, m_height * 0.5f);

    if (!m_compactMode) {
        cocos2d::extension::CCScale9Sprite* textBg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_001.png",
            cocos2d::CCRect(0, 0, 80, 80)
        );
        textBg->setContentSize(cocos2d::CCSize(textWidth, m_height - textHeight));
        m_mainLayer->addChild(textBg, -1);
        textBg->setPosition(center);
        textBg->setColor(cocos2d::ccc3(64, 90, 40));
        textBg->setTag(100);
    }

    // Comment text
    std::string commentText = comment->m_commentString;

    if (comment->m_isSpam) {
        if (m_compactMode) {
            commentText = "Comment flagged as spam";
        } else {
            commentText = "<cy>Comment flagged as spam</c>";
        }
    }

    if (m_compactMode) {
        cocos2d::CCLabelBMFont* compactLabel = cocos2d::CCLabelBMFont::create();
        compactLabel->setString(commentText.c_str());
        m_mainLayer->addChild(compactLabel);
        compactLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        compactLabel->setPosition(cocos2d::CCPoint(10.0f, (m_height * 0.5f) - 9.0f));
        compactLabel->limitLabelWidth(270.0f, 0.7f, 0.0f);

        if (isRobTop) {
            compactLabel->setColor(cocos2d::ccc3(50, 180, 255));
        } else {
            GameLevelManager* glm = GameLevelManager::sharedState();
            GJGameLevel* level = glm->getSavedLevel(comment->m_levelID);
            if (!level) {
                level = glm->getSavedDailyLevelFromLevelID(comment->m_levelID);
            }
            if (level && comment->m_userID == level->m_userID) {
                compactLabel->setColor(cocos2d::ccc3(255, 75, 75));
            } else if (comment->m_modBadge > 0 && !m_accountComment) {
                compactLabel->setColor(comment->m_color);
            }
        }
    } else {
        // Use TextArea for full comments
        float maxWidth = (textWidth - 45.0f) / 0.7f;
        cocos2d::CCPoint textPos = cocos2d::CCPoint(20.0f, (m_height * 0.5f) - 0.0f);

        TextArea* textArea = TextArea::create(
            commentText,
            "chatFont.fnt",
            1.0f,
            maxWidth,
            textPos,
            0.0f,
            cocos2d::CCPoint(0.0f, 1.0f)
        );
        m_mainLayer->addChild(textArea);
        textArea->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        textArea->setPosition(cocos2d::CCPoint(10.0f, m_height * 0.5f));
        textArea->setScale(0.7f);

        if (isRobTop) {
            textArea->colorAllCharactersTo(cocos2d::ccc3(50, 180, 255));
        } else {
            GameLevelManager* glm = GameLevelManager::sharedState();
            GJGameLevel* level = glm->getSavedLevel(comment->m_levelID);
            if (!level) {
                level = glm->getSavedDailyLevelFromLevelID(comment->m_levelID);
            }
            if (level && comment->m_userID == level->m_userID) {
                textArea->colorAllCharactersTo(cocos2d::ccc3(255, 75, 75));
            } else if (comment->m_modBadge > 0 && !m_accountComment) {
                textArea->colorAllCharactersTo(comment->m_color);
            }
        }
    }

    // Level ID button
    if (comment->m_hasLevelID) {
        int levelId = comment->m_levelID;
        bool isDaily = false;
        if (levelId < 0) {
            levelId = -levelId;
            isDaily = true;
        }

        const char* buttonFile = isDaily ? "GJ_button_03.png" : "GJ_button_01.png";
        cocos2d::CCString* levelStr = CCString::createWithFormat("%i", levelId);
        cocos2d::CCNode* levelBtn = ButtonSprite::create(
            levelStr->getCString(),
            "bigFont.fnt",
            buttonFile,
            30.0f
        );
        levelBtn->setScale(m_compactMode ? 0.7f : 1.15f);

        CCMenuItemSpriteExtra* levelItem = CCMenuItemSpriteExtra::create(
            levelBtn, nullptr, this, menu_selector(CommentCell::onGoToLevel)
        );
        levelItem->setSizeMult(1.2f);
        menu->addChild(levelItem);

        float xPos = m_width - (m_compactMode ? 95.0f : 130.0f);
        float yPos = m_compactMode ? 11.5f : 19.5f;
        levelItem->setPosition(xPos, m_height - yPos);
    }

    // Show button for spam comments
    if (comment->m_isSpam) {
        cocos2d::CCNode* showBtn = ButtonSprite::create(
            "Show",
            "goldFont.fnt",
            "GJ_button_04.png",
            30.0f
        );
        if (m_compactMode) {
            showBtn->setScale(0.8f);
        }

        CCMenuItemSpriteExtra* showItem = CCMenuItemSpriteExtra::create(
            showBtn, nullptr, this, menu_selector(CommentCell::onUnhide)
        );
        showItem->setSizeMult(1.2f);
        menu->addChild(showItem);

        float xPos = m_width - (showBtn->getContentSize().width * 0.5f) - 9.0f;
        showItem->setPosition(xPos, m_height * 0.5f);
        return;
    }

    // Like/Dislike
    if (!comment->m_unkMultiplayerBool) {
        float yPos = m_compactMode ? (m_height - 14.0f) : (m_height - 20.0f);
        float scale = m_compactMode ? 0.6f : 0.8f;
        float xOffset = m_compactMode ? 12.0f : 13.0f;

        cocos2d::CCSprite* likeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_likesIcon_001.png");
        m_iconSprite = likeIcon;

        float xPos = m_width - (m_compactMode ? 32.0f : 35.0f);
        if (!m_compactMode) {
            xPos -= 14.0f;
        }
        likeIcon->setPosition(cocos2d::CCPoint(xPos, yPos));
        likeIcon->setScale(scale);

        int likeCount = comment->m_likeCount;
        if (likeCount < 0) {
            likeIcon->setDisplayFrame(
                cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_dislikesIcon_001.png")
            );
        }

        m_likeLabel = cocos2d::CCLabelBMFont::create();
        m_likeLabel->setString(CCString::createWithFormat("%i", abs(likeCount))->getCString());
        m_mainLayer->addChild(m_likeLabel);
        m_likeLabel->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));

        float labelX = xPos + (m_compactMode ? 8.0f + 1.0f : 14.0f + 0.5f);
        float labelY = yPos + (m_compactMode ? 1.0f : 0.5f);
        m_likeLabel->setPosition(cocos2d::CCPoint(labelX, labelY));
        m_likeLabel->setScale(m_compactMode ? 0.3f : 0.4f);
        m_likeLabel->limitLabelWidth(m_compactMode ? 15.0f : 28.0f, m_compactMode ? 0.3f : 0.4f, 0.0f);

        // Like button
        CCMenuItemSpriteExtra* likeItem = CCMenuItemSpriteExtra::create(
            likeIcon, nullptr, this, menu_selector(CommentCell::onLike)
        );
        likeItem->setSizeMult(2.0f);
        menu->addChild(likeItem);
        likeItem->setPosition(likeIcon->getPosition());

        // Delete button
        bool canDelete = false;
        if (m_accountComment) {
            if (comment->m_accountID == GJAccountManager::sharedState()->m_accountID) {
                canDelete = true;
            }
        } else if (comment->m_levelID > 0) {
            GJGameLevel* level = GameLevelManager::sharedState()->getSavedLevel(comment->m_levelID);
            if (level) {
                if (GameManager::sharedState()->m_playerUserID == level->m_userID) {
                    canDelete = true;
                }
            }
        } else if (comment->m_userID == GameManager::sharedState()->m_playerUserID) {
            canDelete = true;
        }

        if (GameManager::sharedState()->m_hasRP == 2) {
            canDelete = true;
        }

        if (canDelete) {
            cocos2d::CCSprite* deleteIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
            deleteIcon->setScale(scale * 0.95f);

            CCMenuItemSpriteExtra* deleteItem = CCMenuItemSpriteExtra::create(
                deleteIcon, nullptr, this, menu_selector(CommentCell::onConfirmDelete)
            );
            deleteItem->setSizeMult(2.0f);
            menu->addChild(deleteItem);

            float dx = m_compactMode ? -20.0f : -24.0f;
            deleteItem->setPosition(likeItem->getPosition() + cocos2d::CCPoint(dx, -1.0f));
        }
    }

    // Date label
    if (!comment->m_uploadDate.empty()) {
        cocos2d::CCLabelBMFont* dateLabel = cocos2d::CCLabelBMFont::create();
        dateLabel->setString(CCString::createWithFormat("%s ago", comment->m_uploadDate.c_str())->getCString());
        m_mainLayer->addChild(dateLabel, 1);
        dateLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        dateLabel->setScale(m_compactMode ? 0.35f : 0.5f);
        dateLabel->setColor(cocos2d::ccc3(180, 200, 230));
        dateLabel->setOpacity(125);

        float xPos = m_width - (m_compactMode ? 8.0f : 11.0f);
        float yPos = m_compactMode ? 7.0f : 14.0f;
        dateLabel->setPosition(cocos2d::CCPoint(xPos, yPos));
    }
}

void CommentCell::onConfirmDelete(cocos2d::CCObject* sender) {
    if (m_comment) {
        FLAlertLayer* alert = FLAlertLayer::create(
            this,
            "Delete Comment",
            "Are you sure you want to <cr>delete</c> this comment?"
        );
        alert->setTag(0);
        alert->show();
    }
}

void CommentCell::onDelete() {
    if (!m_comment) {
        return;
    }

    if (m_accountComment) {
        GameLevelManager::sharedState()->deleteAccountComment(m_comment->m_accountID, m_comment->m_commentID);
    } else {
        GameLevelManager::sharedState()->deleteLevelComment(m_comment->m_levelID, m_comment->m_commentID);
    }

    m_comment->m_commentDeleted = true;
    loadFromComment(m_comment);
}

void CommentCell::onGoToLevel(cocos2d::CCObject* sender) {
    if (!m_comment || m_comment->m_levelID == 0) {
        return;
    }

    int levelId = m_comment->m_levelID;
    int searchMode = 0;
    if (levelId < 0) {
        levelId = -levelId;
        searchMode = 1;
    }

    GJSearchObject* searchObj = GJSearchObject::create(SearchType::Type0);
    searchObj->m_searchMode = searchMode;

    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void CommentCell::onLike(cocos2d::CCObject* sender) {
    if (!m_comment) {
        return;
    }

    LikeItemType type;
    int itemId;
    int accountId;

    if (m_accountComment) {
        type = LikeItemType::AccountComment;
        itemId = m_comment->m_commentID;
        accountId = m_comment->m_accountID;

        if (GameLevelManager::sharedState()->hasLikedItemFullCheck(type, itemId, accountId)) {
            return;
        }
    } else {
        type = LikeItemType::LevelComment;
        itemId = m_comment->m_commentID;
        accountId = m_comment->m_levelID;

        if (GameLevelManager::sharedState()->hasLikedItemFullCheck(type, itemId, accountId)) {
            return;
        }
    }

    if (m_comment->m_userID == GameManager::sharedState()->m_playerUserID) {
        return;
    }

    if (m_accountComment) {
        if (m_comment->m_accountID == GJAccountManager::sharedState()->m_accountID) {
            return;
        }
    }

    LikeItemLayer* likeLayer = LikeItemLayer::create(type, itemId, accountId);
    likeLayer->m_likeDelegate = this;
    likeLayer->show();
}

void CommentCell::onUndelete() {
    if (m_comment) {
        m_comment->m_commentDeleted = false;
        loadFromComment(m_comment);
    }
}

void CommentCell::onUnhide(cocos2d::CCObject* sender) {
    if (m_comment) {
        m_comment->m_isSpam = false;
        loadFromComment(m_comment);
    }
}

void CommentCell::onViewProfile(cocos2d::CCObject* sender) {
    if (m_comment) {
        ProfilePage* profile = ProfilePage::create(m_comment->m_accountID, false);
        profile->show();
    }
}

void CommentCell::updateBGColor(int index) {
    if (m_compactMode) {
        if (index & 1) {
            m_backgroundLayer->setColor(cocos2d::ccc3(144, 79, 39));
        } else {
            m_backgroundLayer->setColor(cocos2d::ccc3(156, 85, 42));
        }
    } else {
        if (index & 1) {
            m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
        } else {
            m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
        }
    }
}

void CommentCell::updateLabelValues() {
    if (!m_likeLabel || !m_comment) {
        return;
    }

    int likeCount = m_comment->m_likeCount;
    m_likeLabel->setString(CCString::createWithFormat("%i", abs(likeCount))->getCString());

    if (likeCount < 0) {
        m_iconSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_dislikesIcon_001.png")
        );
    } else {
        m_iconSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_likesIcon_001.png")
        );
    }
}

CommentCell::~CommentCell() {
    CC_SAFE_RELEASE_NULL(m_comment);
}