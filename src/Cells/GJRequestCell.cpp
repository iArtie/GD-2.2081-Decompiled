#include "Cells/GJRequestCell.h"
#include "Models/GJUserScore.h"
#include "GameLevelManager.h"
#include "Managers/GJAccountManager.h"
#include "Models/GJFriendRequest.h"
#include "Popups/FriendRequestPopup.h"
#include "Pages/ProfilePage.h"
#include "Popups/UploadActionPopup.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Sprites/ButtonSprite.h"
#include "Layers/FLAlertLayer.h"
#include "Sprites/SimplePlayer.h"

GJRequestCell::GJRequestCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void GJRequestCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

bool GJRequestCell::init() {
    m_score = nullptr;
    m_popup = nullptr;
    m_toggler = nullptr;
    return true;
}

void GJRequestCell::loadFromScore(GJUserScore* score) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    
    if (!score) {
        return;
    }
    
    if (m_score) {
        m_score->release();
    }
    m_score = score;
    m_score->retain();
    
    float playerScale = (score->m_iconType == IconType::Ship) ? 0.65f : 0.75f;
    
    auto player = SimplePlayer::create(1);
    m_mainLayer->addChild(player, 1);
    player->setScale(playerScale);
    player->setPosition(cocos2d::CCPoint(60.0f, m_height * 0.5f));
    
    auto manager = GameManager::sharedState();
    player->setColor(manager->colorForIdx(m_score->m_color1));
    player->setSecondColor(manager->colorForIdx(m_score->m_color2));
    player->tryEnableCustomGlowColor(m_score->m_color3);
    
    bool hasGlow = (m_score->m_special == 2 || m_score->m_glowEnabled);
    player->m_hasGlowOutline = hasGlow;
    player->updatePlayerFrame(m_score->m_iconID, m_score->m_iconType);
    player->updateColors();
    
    auto menu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(menu);
    
    std::string displayName = m_score->m_userName;
    if (displayName.empty()) {
        displayName = "Unknown";
        m_score->m_userName = displayName;
    }
    
    auto nameLabel = cocos2d::CCLabelBMFont::create(displayName.c_str(), "bigFont.fnt");
    nameLabel->limitLabelWidth(120.0f, 0.8f, 0.0f);
    
    auto nameItem = CCMenuItemSpriteExtra::create(nameLabel, this, menu_selector(GJRequestCell::onViewProfile));
    menu->addChild(nameItem);
    
    cocos2d::CCPoint playerPos = player->getPosition();
    float labelOffset = nameLabel->getContentSize().width * nameLabel->getScale() * 0.5f + 25.0f;
    nameItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(playerPos.x + labelOffset, playerPos.y)));
    
    auto viewBtn = ButtonSprite::create("View", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 25.0f);
    auto viewItem = CCMenuItemSpriteExtra::create(viewBtn, this, menu_selector(GJRequestCell::onViewFriendRequest));
    viewItem->setSizeMult(1.5f);
    menu->addChild(viewItem);
    viewItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - viewBtn->getContentSize().width * 0.5f - 45.0f, m_height * 0.5f)));
    
    auto closeBtn = ButtonSprite::create("X", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_06.png", nullptr, 31.25f);
    closeBtn->setScale(0.8f);
    auto closeItem = CCMenuItemSpriteExtra::create(closeBtn, this, menu_selector(GJRequestCell::onDeleteRequest));
    closeItem->setSizeMult(1.5f);
    menu->addChild(closeItem);
    closeItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - 22.0f, m_height * 0.5f)));
    
    auto onSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto offSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    onSprite->setScale(0.7f);
    offSprite->setScale(0.7f);
    
    m_toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(GJRequestCell::onToggle));
    m_toggler->toggle(m_score->m_toggled);
    menu->addChild(m_toggler);
    m_toggler->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(20.0f, m_height * 0.5f)));
    m_toggler->setSizeMult(1.2f);
    
    if (m_score->m_newFriendRequest) {
        auto unreadLabel = cocos2d::CCLabelBMFont::create("*", "bigFont.fnt");
        unreadLabel->setColor(cocos2d::ccc3(255, 0, 0));
        unreadLabel->setScale(0.5f);
        unreadLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        m_mainLayer->addChild(unreadLabel, 10);
        unreadLabel->setTag(100);
        
        cocos2d::CCPoint labelPos = nameLabel->getPosition();
        float labelWidth = nameLabel->getContentSize().width * nameLabel->getScale() * 0.5f;
        unreadLabel->setPosition(cocos2d::CCPoint(labelPos.x + labelWidth + 5.0f, labelPos.y));
    }
}

void GJRequestCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2 || layer->getTag() != 1 || !m_score) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    auto request = manager->friendRequestFromAccountID(m_score->m_accountID);
    
    if (request) {
        markAsRead();
        manager->m_uploadActionDelegate = this;
        
        if (manager->deleteFriendRequests(request->m_accountID, 0, 0)) {
            auto popup = UploadActionPopup::create(this, "Removing Friend Request...");
            m_popup = popup;
            popup->show();
        }
    }
}

void GJRequestCell::markAsRead() {
    if (!m_score->m_newFriendRequest) {
        return;
    }
    
    auto node = m_mainLayer->getChildByTag(100);
    if (node) {
        node->setVisible(false);
    }
    
    m_score->m_newFriendRequest = false;
    
    auto manager = GameLevelManager::sharedState();
    auto account = GJAccountManager::sharedState();
    auto userScore = manager->userInfoForAccountID(account->m_accountID - account->m_unkInt1);
    
    if (userScore) {
        int count = userScore->m_friendReqCount - 1;
        userScore->m_friendReqCount = count < 0 ? 0 : count;
    }
}

void GJRequestCell::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    m_popup = nullptr;
}

void GJRequestCell::onDeleteRequest(cocos2d::CCObject* sender) {
    if (!m_score || m_score->m_accountID <= 0) {
        return;
    }
    
    auto alert = FLAlertLayer::create(
        this,
        "Delete",
        "Are you sure you want to <cr>delete</c> this friend request?",
        "Back",
        "Delete",
        380.0f
    );
    alert->setTag(1);
    alert->show();
}

void GJRequestCell::onToggle(cocos2d::CCObject* sender) {
    if (m_score) {
        m_score->m_toggled = !m_score->m_toggled;
    }
}

void GJRequestCell::onViewFriendRequest(cocos2d::CCObject* sender) {
    if (!m_score || m_score->m_accountID <= 0) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    auto request = manager->friendRequestFromAccountID(m_score->m_accountID);
    
    if (request) {
        markAsRead();
        auto popup = FriendRequestPopup::create(request);
        popup->show();
    }
}

void GJRequestCell::onViewProfile(cocos2d::CCObject* sender) {
    if (!m_score || m_score->m_accountID <= 0) {
        return;
    }
    
    auto profile = ProfilePage::create(m_score->m_accountID, 0);
    profile->show();
}

void GJRequestCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

void GJRequestCell::updateToggle() {
    if (m_score && m_toggler) {
        m_toggler->toggle(m_score->m_toggled);
    }
}

void GJRequestCell::uploadActionFailed(int id, int response) {
    if (!m_score) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    auto request = manager->friendRequestFromAccountID(m_score->m_accountID);
    
    if (request && request->m_accountID != id) {
        return;
    }
    
    if (m_popup) {
        m_popup->showFailMessage("Failed. Please try again later.");
        m_popup->m_delegate = nullptr;
        m_popup = nullptr;
    }
}

void GJRequestCell::uploadActionFinished(int id, int response) {
    if (!m_score) {
        return;
    }
    
    auto manager = GameLevelManager::sharedState();
    auto request = manager->friendRequestFromAccountID(m_score->m_accountID);
    
    if (request && request->m_accountID != id) {
        return;
    }
    
    if (m_popup) {
        std::string message = "Unknown action finished";
        if (response == 30) {
            message = "Request removed!";
        }
        
        m_popup->showSuccessMessage(message);
        m_popup->m_delegate = nullptr;
        m_popup = nullptr;
    }
}

GJRequestCell::~GJRequestCell() {
    if (m_score) {
        m_score->release();
    }
}