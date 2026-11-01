#include "GJUserCell.h"
#include "GJUserScore.h"
#include "SimplePlayer.h"
#include "GameManager.h"
#include "GameLevelManager.h"
#include "GJAccountManager.h"
#include "GJFriendRequest.h"
#include "FriendRequestPopup.h"
#include "ProfilePage.h"
#include "GJWriteMessagePopup.h"
#include "UploadActionPopup.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "FLAlertLayer.h"

GJUserCell::GJUserCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void GJUserCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

bool GJUserCell::init() {
    m_userScore = nullptr;
    m_uploadPopup = nullptr;
    return true;
}

void GJUserCell::loadFromScore(GJUserScore* score) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    
    if (m_userScore) {
        m_userScore->release();
        m_userScore = nullptr;
    }
    
    if (!score) {
        return;
    }
    
    m_userScore = score;
    m_userScore->retain();
    
    bool isBlocked = score->m_friendReqStatus == 2;
    bool hasSentRequest = score->m_friendReqStatus == 4;
    
    auto player = SimplePlayer::create(1);
    m_mainLayer->addChild(player, 1);
    player->setScale(1.0f);
    player->setPosition(cocos2d::CCPoint(30.0f, m_height * 0.5f));
    
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
    
    std::string displayName = score->m_userName;
    if (displayName.empty()) {
        displayName = "Unknown";
        score->m_userName = displayName;
    }
    
    auto nameLabel = cocos2d::CCLabelBMFont::create(displayName.c_str(), "bigFont.fnt");
    nameLabel->limitLabelWidth(150.0f, 0.8f, 0.0f);
    
    auto nameItem = CCMenuItemSpriteExtra::create(nameLabel, this, menu_selector(GJUserCell::onViewProfile));
    menu->addChild(nameItem);
    
    cocos2d::CCPoint playerPos = player->getPosition();
    float labelOffset = nameLabel->getContentSize().width * nameLabel->getScale() * 0.5f + 35.0f;
    nameItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(playerPos.x + labelOffset, playerPos.y)));
    
    if (isBlocked) {
        nameItem->setEnabled(false);
        
        auto unblockBtn = ButtonSprite::create("Unblock", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_05.png", nullptr, 28.0f);
        auto unblockItem = CCMenuItemSpriteExtra::create(unblockBtn, this, menu_selector(GJUserCell::onUnblockUser));
        unblockItem->setSizeMult(1.5f);
        menu->addChild(unblockItem);
        unblockItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - unblockBtn->getContentSize().width * 0.5f - 9.0f, m_height * 0.5f)));
    } else if (hasSentRequest) {
        auto cancelBtn = ButtonSprite::create("Cancel", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_05.png", nullptr, 28.0f);
        auto cancelItem = CCMenuItemSpriteExtra::create(cancelBtn, this, menu_selector(GJUserCell::onCancelFriendRequest));
        cancelItem->setSizeMult(1.5f);
        menu->addChild(cancelItem);
        cancelItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - cancelBtn->getContentSize().width * 0.5f - 9.0f, m_height * 0.5f)));
    }
    
    if (score->m_friendReqStatus == 1) {
        if (score->m_messageState != 2) {
            auto msgSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_messages_001.png");
            msgSprite->setScale(0.7f);
            auto msgItem = CCMenuItemSpriteExtra::create(msgSprite, this, menu_selector(GJUserCell::onSendMessage));
            msgItem->setSizeMult(1.2f);
            menu->addChild(msgItem);
            msgItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - 60.0f, m_height * 0.5f)));
        }
        
        auto removeSprite = cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_removeFriend_001.png");
        removeSprite->setScale(0.7f);
        auto removeItem = CCMenuItemSpriteExtra::create(removeSprite, this, menu_selector(GJUserCell::onRemoveFriend));
        removeItem->setSizeMult(1.2f);
        menu->addChild(removeItem);
        removeItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - 24.0f, m_height * 0.5f)));
    }
    
    if (score->m_newFriendRequest && !isBlocked && !hasSentRequest) {
        auto unreadLabel = cocos2d::CCLabelBMFont::create("*", "bigFont.fnt");
        unreadLabel->setScale(0.5f);
        unreadLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        m_mainLayer->addChild(unreadLabel, 10);
        unreadLabel->setTag(100);
        
        cocos2d::CCPoint labelPos = nameLabel->getPosition();
        float labelWidth = nameLabel->getContentSize().width * nameLabel->getScale() * 0.5f;
        unreadLabel->setPosition(cocos2d::CCPoint(labelPos.x + labelWidth + 10.0f, labelPos.y));
    }
}

void GJUserCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2 || !m_userScore) {
        return;
    }
    
    int tag = layer->getTag();
    auto manager = GameLevelManager::sharedState();
    
    if (tag == 1) {
        if (manager->unblockUser(m_userScore->m_accountID)) {
            manager->m_uploadActionDelegate = this;
            
            if (m_uploadPopup) {
                m_uploadPopup->m_delegate = nullptr;
            }
            
            auto popup = UploadActionPopup::create(this, "Unblocking user...");
            m_uploadPopup = popup;
            popup->show();
        }
    } else if (tag == 2) {
        if (manager->removeFriend(m_userScore->m_accountID)) {
            manager->m_uploadActionDelegate = this;
            
            if (m_uploadPopup) {
                m_uploadPopup->m_delegate = nullptr;
            }
            
            auto popup = UploadActionPopup::create(this, "Removing Friend...");
            m_uploadPopup = popup;
            popup->show();
        }
    } else if (tag == 3) {
        if (manager->deleteSentFriendRequest(m_userScore->m_accountID)) {
            manager->m_uploadActionDelegate = this;
            
            auto popup = UploadActionPopup::create(this, "Removing Friend Request...");
            m_uploadPopup = popup;
            popup->show();
        }
    }
}

void GJUserCell::onCancelFriendRequest(cocos2d::CCObject* sender) {
    if (!m_userScore || m_userScore->m_accountID <= 0) {
        return;
    }
    
    auto alert = FLAlertLayer::create(
        this,
        "Cancel friend request",
        "Are you sure you want to remove this friend request?",
        "Back",
        "Remove",
        380.0f
    );
    alert->setTag(3);
    alert->show();
}

void GJUserCell::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    m_uploadPopup = nullptr;
}

void GJUserCell::onRemoveFriend(cocos2d::CCObject* sender) {
    if (!m_userScore || m_userScore->m_accountID <= 0) {
        return;
    }
    
    auto alert = FLAlertLayer::create(
        this,
        "Unfriend",
        cocos2d::CCString::createWithFormat("Are you sure you want to <cr>remove</c> <cy>%s</c> from your friends list?", m_userScore->m_userName.c_str())->getCString(),
        "Back",
        "Unfriend",
        380.0f
    );
    alert->setTag(2);
    alert->show();
}

void GJUserCell::onSendMessage(cocos2d::CCObject* sender) {
    if (!m_userScore || m_userScore->m_accountID <= 0) {
        return;
    }
    
    auto popup = GJWriteMessagePopup::create(m_userScore->m_accountID, 0);
    popup->show();
}

void GJUserCell::onUnblockUser(cocos2d::CCObject* sender) {
    if (!m_userScore || m_userScore->m_accountID <= 0) {
        return;
    }
    
    auto alert = FLAlertLayer::create(
        this,
        "Unblock user",
        cocos2d::CCString::createWithFormat("Are you sure you want to <cl>unblock</c> <cy>%s</c>?", m_userScore->m_userName.c_str())->getCString(),
        "Back",
        "Unblock",
        380.0f
    );
    alert->setTag(1);
    alert->show();
}

void GJUserCell::onViewFriendRequest(cocos2d::CCObject* sender) {
    if (!m_userScore || m_userScore->m_accountID <= 0) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    auto request = manager->friendRequestFromAccountID(m_userScore->m_accountID);
    
    if (request) {
        if (m_userScore->m_newFriendRequest) {
            auto node = m_mainLayer->getChildByTag(100);
            if (node) {
                node->setVisible(false);
            }
            m_userScore->m_newFriendRequest = false;
        }
        
        auto popup = FriendRequestPopup::create(request);
        popup->show();
    }
}

void GJUserCell::onViewProfile(cocos2d::CCObject* sender) {
    if (!m_userScore || m_userScore->m_accountID <= 0) {
        return;
    }
    
    auto profile = ProfilePage::create(m_userScore->m_accountID, 0);
    profile->show();
}

void GJUserCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

void GJUserCell::uploadActionFailed(int id, int response) {
    if (m_userScore && m_userScore->m_accountID == id && m_uploadPopup) {
        m_uploadPopup->showFailMessage("Failed. Please try again later.");
        m_uploadPopup->m_delegate = nullptr;
        m_uploadPopup = nullptr;
    }
}

void GJUserCell::uploadActionFinished(int id, int response) {
    if (m_userScore && m_userScore->m_accountID == id && m_uploadPopup) {
        std::string message = "Unknown action finished";
        if (response == 30) {
            message = "Request removed!";
        } else if (response == 33) {
            message = "Friend removed!";
        } else if (response == 35) {
            message = "User unblocked!";
        }
        
        m_uploadPopup->showSuccessMessage(message);
        m_uploadPopup->m_delegate = nullptr;
        m_uploadPopup = nullptr;
    }
}

GJUserCell::~GJUserCell() {
    if (m_userScore) {
        m_userScore->release();
    }
}