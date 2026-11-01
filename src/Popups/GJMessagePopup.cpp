#include "Popups/GJMessagePopup.h"
#include "Models/GJUserMessage.h"
#include "GameLevelManager.h"
#include "UI/LoadingCircle.h"
#include "Popups/UploadActionPopup.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/FLAlertLayer.h"
#include "Popups/GJWriteMessagePopup.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"

GJMessagePopup* GJMessagePopup::create(GJUserMessage* message) {
    auto popup = new GJMessagePopup();
    if (popup && popup->init(message)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool GJMessagePopup::init(GJUserMessage* message) {
    if (!message || !FLAlertLayer::initWithColor(cocos2d::ccc4(0, 0, 0, 150))) {
        return false;
    }
    
    FLAlertLayer::incrementForcePrio();
    
    m_message = message;
    m_message->retain();
    
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png");
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(420.0f, 240.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    auto titleLabel = cocos2d::CCLabelBMFont::create(m_message->m_title.c_str(), "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 104.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    const char* prefix = m_message->m_outgoing ? "To: " : "From: ";
    std::string senderText = std::string(prefix) + m_message->m_username;
    auto senderLabel = cocos2d::CCLabelBMFont::create(senderText.c_str(), "bigFont.fnt");
    senderLabel->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    m_mainLayer->addChild(senderLabel);
    senderLabel->setScale(0.6f);
    
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->m_parentLayer = m_mainLayer;
    m_loadingCircle->m_fade = false;
    m_loadingCircle->setScale(0.8f);
    m_loadingCircle->show();
    m_loadingCircle->m_sprite->setPosition(cocos2d::CCPoint(centerX, centerY - 4.0f));
    m_loadingCircle->setVisible(false);
    
    m_errorLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_errorLabel, 10);
    m_errorLabel->setPosition(cocos2d::CCPoint(centerX, centerY));
    m_errorLabel->setScale(0.6f);
    m_errorLabel->setVisible(false);
    
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    m_closeButton = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(GJMessagePopup::onClose));
    m_closeButton->setSizeMult(1.6f);
    m_buttonMenu->addChild(m_closeButton);
    m_closeButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 205.0f, centerY + 115.0f)));
    
    auto textBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    textBg->setContentSize(cocos2d::CCSize(370.0f, 130.0f));
    m_mainLayer->addChild(textBg, -1);
    textBg->setPosition(cocos2d::CCPoint(centerX, centerY));
    textBg->setColor(cocos2d::ccc3(165, 33, 33));
    textBg->setOpacity(75);
    
    if (!m_message->m_uploadDate.empty()) {
        std::string timeText = m_message->m_uploadDate + " ago";
        auto timeLabel = cocos2d::CCLabelBMFont::create(timeText.c_str(), "bigFont.fnt");
        m_mainLayer->addChild(timeLabel, 1);
        timeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        timeLabel->setScale(0.6f);
        timeLabel->setColor(cocos2d::ccc3(125, 125, 125));
        timeLabel->setOpacity(225);
        timeLabel->setPosition(cocos2d::CCPoint(centerX + 202.0f, centerY + 108.0f));
    }
    
    if (m_message->m_content.empty()) {
        m_loadingCircle->setVisible(true);
        GameLevelManager::sharedState()->m_downloadMessageDelegate = this;
        GameLevelManager::sharedState()->downloadUserMessage(m_message->m_messageID, m_message->m_outgoing);
    } else {
        loadFromGJMessage(m_message);
    }
    
    return true;
}

void GJMessagePopup::loadFromGJMessage(GJUserMessage* message) {
    if (!message) {
        return;
    }
    
    if (m_message) {
        m_message->release();
    }
    m_message = message;
    m_message->retain();
    
    GameLevelManager::sharedState()->m_uploadActionDelegate = this;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    std::string content = m_message->m_content.empty() ? "No message" : m_message->m_content;
    
    auto textArea = TextArea::create(content, "chatFont.fnt", 1.0f, 290.0f, cocos2d::CCPoint(20.0f, 0.0f), 1.0f, cocos2d::CCPoint(0.0f, 0.5f));
    m_mainLayer->addChild(textArea, 6);
    textArea->setPosition(cocos2d::CCPoint(centerX, centerY - 4.0f));
    textArea->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    textArea->setPosition(cocos2d::CCPoint(centerX - 185.0f + (370.0f - textArea->m_maxWidth) * 0.5f, centerY + 4.0f));
    textArea->setScale(1.0f);
    
    cocos2d::CCPoint buttonPos(centerX, centerY - 98.0f);
    
    if (!m_message->m_outgoing) {
        auto replyBtn = ButtonSprite::create("Reply", 0, 1, 0.55f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 26.0f);
        auto replyItem = CCMenuItemSpriteExtra::create(replyBtn, this, menu_selector(GJMessagePopup::onReply));
        m_buttonMenu->addChild(replyItem);
        replyItem->setPosition(m_buttonMenu->convertToNodeSpace(buttonPos + cocos2d::CCPoint(-140.0f, 0.0f)));
        
        auto blockBtn = ButtonSprite::create("Block", 0, 1, 0.55f, true, "bigFont.fnt", "GJ_button_06.png", nullptr, 26.0f);
        auto blockItem = CCMenuItemSpriteExtra::create(blockBtn, this, menu_selector(GJMessagePopup::onBlock));
        m_buttonMenu->addChild(blockItem);
        blockItem->setPosition(m_buttonMenu->convertToNodeSpace(buttonPos + cocos2d::CCPoint(140.0f, 0.0f)));
    }
    
    auto removeBtn = ButtonSprite::create("Remove", 0, 1, 0.55f, true, "bigFont.fnt", "GJ_button_04.png", nullptr, 26.0f);
    auto removeItem = CCMenuItemSpriteExtra::create(removeBtn, this, menu_selector(GJMessagePopup::onRemove));
    m_buttonMenu->addChild(removeItem);
    removeItem->setPosition(m_buttonMenu->convertToNodeSpace(buttonPos));
}

void GJMessagePopup::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2) {
        return;
    }
    
    int tag = layer->getTag();
    
    if (tag == 1) {
        blockUser();
    } else if (tag == 2) {
        auto manager = GameLevelManager::sharedState();
        if (manager->deleteUserMessages(m_message, 0, m_message->m_outgoing)) {
            manager->m_uploadActionDelegate = this;
            
            if (m_uploadPopup) {
                m_uploadPopup->m_delegate = nullptr;
            }
            
            auto popup = UploadActionPopup::create(this, "Deleting message...");
            m_uploadPopup = popup;
            popup->show();
        }
    }
}

void GJMessagePopup::blockUser() {
    auto manager = GameLevelManager::sharedState();
    if (manager->blockUser(m_message->m_accountID)) {
        auto popup = UploadActionPopup::create(this, "Blocking user...");
        m_uploadPopup = popup;
        popup->show();
    }
}

void GJMessagePopup::downloadMessageFailed(int id) {
    if (m_message->m_messageID == id) {
        m_loadingCircle->setVisible(false);
        m_errorLabel->setVisible(true);
    }
}

void GJMessagePopup::downloadMessageFinished(GJUserMessage* message) {
    if (message && m_message->m_messageID == message->m_messageID) {
        loadFromGJMessage(message);
        m_loadingCircle->setVisible(false);
        m_errorLabel->setVisible(false);
    }
}

void GJMessagePopup::keyBackClicked() {
    onClose(nullptr);
}

void GJMessagePopup::onBlock(cocos2d::CCObject* sender) {
    std::string msg = "Are you sure you want to block <cg>" + m_message->m_username + "</c>?\n";
    msg += "<cg>" + m_message->m_username + "</c> will no longer be able to:\n";
    msg += "- <cy>View your profile</c>\n";
    msg += "- <cl>Send messages</c>\n";
    msg += "- <cp>Send friend requests</c>\n";
    msg += "- <cr>Messages from this user will be removed</c>";
    
    auto alert = FLAlertLayer::create(this, "Confirm Block", msg.c_str(), "Back", "Block", 380.0f);
    alert->setTag(1);
    alert->show();
}

void GJMessagePopup::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJMessagePopup::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    
    if (m_uploadPopup == popup) {
        m_uploadPopup = nullptr;
        if (m_actionSuccess) {
            onClose(nullptr);
        }
    }
}

void GJMessagePopup::onRemove(cocos2d::CCObject* sender) {
    auto alert = FLAlertLayer::create(this, "Delete", "Are you sure you want to <cr>delete</c> this message?", "Back", "Delete", 380.0f);
    alert->setTag(2);
    alert->show();
}

void GJMessagePopup::onReply(cocos2d::CCObject* sender) {
    auto manager = GameLevelManager::sharedState();
    if (!manager->getStoredUserMessageReply(m_message->m_messageID)) {
        auto reply = GJUserMessage::create(0);
        
        std::string title = m_message->m_title;
        if (title.find("Re: ") == std::string::npos) {
            title = "Re: " + title;
        }
        reply->m_title = title;
        
        manager->storeUserMessageReply(m_message->m_messageID, reply);
    }
    
    auto writePopup = GJWriteMessagePopup::create(m_message->m_accountID, m_message->m_messageID);
    writePopup->m_noElasticity = true;
    writePopup->show();
    onClose(nullptr);
}

void GJMessagePopup::uploadActionFailed(int id, int response) {
    if (m_message->m_messageID == id || (response == 34 && id == m_message->m_accountID)) {
        if (m_uploadPopup) {
            m_uploadPopup->showFailMessage("Failed. Please try again later.");
        }
    }
}

void GJMessagePopup::uploadActionFinished(int id, int response) {
    if (m_message->m_messageID == id) {
        if (response == 39) {
            m_actionSuccess = true;
            if (m_uploadPopup) {
                m_uploadPopup->showSuccessMessage("Message removed");
            }
        } else if (response == 34) {
            m_actionSuccess = true;
            if (m_uploadPopup) {
                m_uploadPopup->showSuccessMessage("User blocked!");
            }
        }
    } else if (response == 34 && id == m_message->m_accountID) {
        m_actionSuccess = true;
        if (m_uploadPopup) {
            m_uploadPopup->showSuccessMessage("User blocked!");
        }
    }
}

GJMessagePopup::~GJMessagePopup() {
    if (m_message) {
        m_message->release();
    }
}