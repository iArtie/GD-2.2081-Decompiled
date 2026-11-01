#include "GJMessageCell.h"
#include "GJUserMessage.h"
#include "GameLevelManager.h"
#include "GJAccountManager.h"
#include "GJUserScore.h"
#include "GJMessagePopup.h"
#include "ProfilePage.h"
#include "UploadActionPopup.h"
#include "CCMenuItemToggler.h"
#include "ButtonSprite.h"
#include "FLAlertLayer.h"

GJMessageCell::GJMessageCell(const char* identifier, float width, float height)
    : TableViewCell(identifier, width, height) {
    init();
}

void GJMessageCell::draw() {
    cocos2d::CCSize size = cocos2d::CCSize(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, size.height), cocos2d::CCPoint(size.width - 1.0f, size.height));
    cocos2d::ccDrawLine(cocos2d::CCPoint(1.0f, 0.0f), cocos2d::CCPoint(size.width - 1.0f, 0.0f));
}

bool GJMessageCell::init() {
    m_message = nullptr;
    m_popup = nullptr;
    m_toggler = nullptr;
    return true;
}

void GJMessageCell::loadFromMessage(GJUserMessage* message) {
    m_mainLayer->removeAllChildrenWithCleanup(true);
    
    if (!message) {
        return;
    }
    
    if (m_message) {
        m_message->release();
    }
    m_message = message;
    m_message->retain();
    
    auto titleLabel = cocos2d::CCLabelBMFont::create(m_message->m_title.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    titleLabel->setScale(1.0f);
    titleLabel->setPosition(cocos2d::CCPoint(38.0f, m_height * 0.5f + 14.0f));
    titleLabel->limitLabelWidth(180.0f, 0.5f, 0.0f);
    
    auto viewBtn = ButtonSprite::create("View", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 25.0f);
    auto viewItem = CCMenuItemSpriteExtra::create(viewBtn, this, menu_selector(GJMessageCell::onViewMessage));
    viewItem->setSizeMult(1.1f);
    
    auto menu = cocos2d::CCMenu::create(viewItem, nullptr);
    m_mainLayer->addChild(menu);
    menu->setPosition(cocos2d::CCPoint(m_width - viewBtn->getContentSize().width * 0.5f - 45.0f, m_height * 0.5f));
    
    auto closeBtn = ButtonSprite::create("X", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_06.png", nullptr, 31.25f);
    closeBtn->setScale(0.8f);
    auto closeItem = CCMenuItemSpriteExtra::create(closeBtn, this, menu_selector(GJMessageCell::onDeleteMessage));
    closeItem->setSizeMult(1.1f);
    menu->addChild(closeItem);
    closeItem->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(m_width - 22.0f, m_height * 0.5f)));
    
    const char* prefix = m_message->m_outgoing ? "To:" : "From:";
    std::string senderText = std::string(prefix) + " " + m_message->m_username;
    
    auto senderLabel = cocos2d::CCLabelBMFont::create(senderText.c_str(), "bigFont.fnt");
    senderLabel->setScale(1.0f);
    senderLabel->limitLabelWidth(140.0f, 0.5f, 0.0f);
    
    auto senderItem = CCMenuItemSpriteExtra::create(senderLabel, this, menu_selector(GJMessageCell::onViewProfile));
    menu->addChild(senderItem);
    cocos2d::CCPoint titlePos = titleLabel->getPosition();
    cocos2d::CCPoint labelSize = cocos2d::CCPoint(senderLabel->getContentSize().width * senderLabel->getScale() * 0.5f + 1.0f, -15.0f);
    cocos2d::CCPoint senderPos = titlePos + labelSize;
    senderItem->setPosition(menu->convertToNodeSpace(senderPos));
    
    if (!m_message->m_uploadDate.empty()) {
        std::string timeText = m_message->m_uploadDate + " ago";
        auto timeLabel = cocos2d::CCLabelBMFont::create(timeText.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(timeLabel, 1);
        timeLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        timeLabel->setScale(0.5f);
        timeLabel->setColor(cocos2d::ccc3(125, 125, 125));
        timeLabel->setOpacity(125);
        cocos2d::CCPoint timePos = titleLabel->getPosition() + cocos2d::CCPoint(3.0f, -15.0f);
        timeLabel->setPosition(timePos);
    }
    
    auto onSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto offSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    onSprite->setScale(0.7f);
    offSprite->setScale(0.7f);
    
    m_toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(GJMessageCell::onToggle));
    m_toggler->toggle(m_message->m_toggled);
    menu->addChild(m_toggler);
    m_toggler->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(20.0f, m_height * 0.5f)));
    m_toggler->setSizeMult(1.2f);
    
    if (!m_message->m_read && !m_message->m_outgoing) {
        auto unreadLabel = cocos2d::CCLabelBMFont::create("*", "bigFont.fnt");
        unreadLabel->setColor(cocos2d::ccc3(255, 0, 0));
        unreadLabel->setScale(0.5f);
        unreadLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        m_mainLayer->addChild(unreadLabel, 10);
        
        cocos2d::CCPoint labelPos = titleLabel->getPosition();
        float labelWidth = titleLabel->getContentSize().width * titleLabel->getScale();
        unreadLabel->setPosition(labelPos + cocos2d::CCPoint(labelWidth + 5.0f, 0.0f));
        unreadLabel->setTag(100);
    }
}

void GJMessageCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2 || layer->getTag() != 1) {
        return;
    }
    
    if (!m_message) {
        return;
    }
    
    markAsRead();
    
    auto manager = GameLevelManager::sharedState();
    if (manager->deleteUserMessages(m_message, 0, m_message->m_outgoing)) {
        manager->m_uploadActionDelegate = this;
        
        if (m_popup) {
            m_popup->m_delegate = nullptr;
        }
        
        auto popup = UploadActionPopup::create(this, "Deleting message...");
        m_popup = popup;
        popup->show();
    }
}

void GJMessageCell::markAsRead() {
    if (m_message->m_read) {
        return;
    }
    
    m_message->m_read = true;
    
    auto unreadLabel = m_mainLayer->getChildByTag(100);
    if (unreadLabel) {
        unreadLabel->removeFromParentAndCleanup(true);
    }
    
    auto manager = GameLevelManager::sharedState();
    auto account = GJAccountManager::sharedState();
    auto userScore = manager->userInfoForAccountID(account->m_accountID - account->m_unkInt1);
    
    if (userScore) {
        int count = userScore->m_newMsgCount - 1;
        userScore->m_newMsgCount = count < 0 ? 0 : count;
    }
}

void GJMessageCell::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    m_popup = nullptr;
}

void GJMessageCell::onDeleteMessage(cocos2d::CCObject* sender) {
    if (!m_message || m_message->m_accountID <= 0) {
        return;
    }
    
    auto alert = FLAlertLayer::create(
        this,
        "Delete",
        "Are you sure you want to <cr>delete</c> this message?",
        "Back",
        "Delete",
        380.0f
    );
    alert->setTag(1);
    alert->show();
}

void GJMessageCell::onToggle(cocos2d::CCObject* sender) {
    if (m_message) {
        m_message->m_toggled = !m_message->m_toggled;
    }
}

void GJMessageCell::onViewMessage(cocos2d::CCObject* sender) {
    if (!m_message) {
        return;
    }
    
    markAsRead();
    
    auto manager = GameLevelManager::sharedState();
    auto storedMessage = manager->getStoredUserMessage(m_message->m_messageID);
    
    if (!storedMessage) {
        storedMessage = m_message;
    }
    
    auto popup = GJMessagePopup::create(storedMessage);
    popup->show();
}

void GJMessageCell::onViewProfile(cocos2d::CCObject* sender) {
    if (!m_message || m_message->m_accountID <= 0) {
        return;
    }
    
    auto profile = ProfilePage::create(m_message->m_accountID, 0);
    profile->show();
}

void GJMessageCell::updateBGColor(int index) {
    if (index & 1) {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    } else {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

void GJMessageCell::updateToggle() {
    if (m_message && m_toggler) {
        m_toggler->toggle(m_message->m_toggled);
    }
}

void GJMessageCell::uploadActionFailed(int id, int response) {
    if (m_message->m_messageID != id || !m_popup) {
        return;
    }
    
    m_popup->showFailMessage("Failed. Please try again later.");
    m_popup->m_delegate = nullptr;
    m_popup = nullptr;
}

void GJMessageCell::uploadActionFinished(int id, int response) {
    if (m_message->m_messageID != id || !m_popup) {
        return;
    }
    
    std::string message = "Unknown action finished";
    if (response == 39) {
        message = "Message removed!";
    }
    
    m_popup->showSuccessMessage(message);
    m_popup->m_delegate = nullptr;
    m_popup = nullptr;
}

GJMessageCell::~GJMessageCell() {
    if (m_message) {
        m_message->release();
    }
}