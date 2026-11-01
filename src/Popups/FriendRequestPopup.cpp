#include "Popups/FriendRequestPopup.h"
#include "Models/GJFriendRequest.h"
#include "GameLevelManager.h"
#include "UI/LoadingCircle.h"
#include "Popups/UploadActionPopup.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Sprites/ButtonSprite.h"
#include "UI/TextArea.h"
#include "Layers/FLAlertLayer.h"

void FriendRequestPopup::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2 && layer->getTag() == 1) {
        blockUser();
    }
}

void FriendRequestPopup::blockUser() {
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->blockUser(m_request->m_accountID)) {
        std::string msg = "Blocking user...";
        m_popup = UploadActionPopup::create(this, msg);
        m_popup->show();
    }
}

FriendRequestPopup* FriendRequestPopup::create(GJFriendRequest* request) {
    FriendRequestPopup* ret = new FriendRequestPopup();
    if (ret && ret->init(request)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FriendRequestPopup::init(GJFriendRequest* request) {
    if (!request) return false;
    
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccColor4B{0, 0, 0, 150})) {
        return false;
    }
    
    FLAlertLayer::incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square02.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(340, 190));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_joystickConnected = -1;
    
    // Title
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create();
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f - 16.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    // Get username
    GameLevelManager::sharedState()->tryGetUsername(request->m_accountID);
    std::string fromText = "From: " + request->m_username;
    
    // From label
    cocos2d::CCLabelBMFont* fromLabel = cocos2d::CCLabelBMFont::create();
    cocos2d::CCPoint titlePos = titleLabel->getPosition();
    fromLabel->setPosition(cocos2d::CCPoint(titlePos.x, titlePos.y - 20.0f));
    m_mainLayer->addChild(fromLabel);
    fromLabel->setScale(0.6f);
    
    // Loading circle
    m_circle = LoadingCircle::create();
    m_circle->m_parentLayer = m_mainLayer;
    m_circle->m_fade = false;
    m_circle->setScale(0.8f);
    m_circle->show();
    m_circle->m_sprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 4.0f));
    m_circle->setVisible(false);
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    m_closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, this, menu_selector(FriendRequestPopup::onClose));
    m_closeBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(m_closeBtn);
    m_closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 170.0f + 5.0f, winSize.height * 0.5f + 95.0f - 5.0f)));
    
    // Message background
    cocos2d::extension::CCScale9Sprite* msgBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", cocos2d::CCRect(0, 0, 80, 80));
    msgBg->setContentSize(cocos2d::CCSize(290, 80));
    m_mainLayer->addChild(msgBg, -1);
    msgBg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 4.0f));
    msgBg->setColor(cocos2d::ccColor3B{130, 130, 130});
    msgBg->setOpacity(75);
    
    loadFromGJFriendRequest(request);
    
    return true;
}

void FriendRequestPopup::keyBackClicked() {
    onClose(nullptr);
}

void FriendRequestPopup::loadFromGJFriendRequest(GJFriendRequest* request) {
    if (!request || m_request) return;
    
    m_request = request;
    m_request->retain();
    
    GameLevelManager::sharedState()->m_uploadActionDelegate = this;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    std::string message = request->m_message.empty() ? "No message" : request->m_message;
    
    // Replace \n with actual newlines
    size_t pos = 0;
    while ((pos = message.find("\\n", pos)) != std::string::npos) {
        message.replace(pos, 2, "\n");
        pos += 1;
    }
    
    // Calculate scale based on message length
    float scale = 1.0f;
    if (message.length() > 180) {
        scale = 0.7f;
    } else if (message.length() > 70) {
        scale = ((180 - (int)message.length()) / 180.0f) * 0.3f + 0.7f;
    }
    
    cocos2d::CCPoint msgPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 4.0f);
    
    TextArea* textArea = TextArea::create(message, "chatFont.fnt", 1.0f, 210.0f / scale, 
                                          cocos2d::CCPoint(20.0f, 0.0f), 0.0f, true);
    m_mainLayer->addChild(textArea);
    textArea->setPosition(cocos2d::CCPoint(msgPos.x, msgPos.y + 1.0f));
    textArea->setScale(scale);
    
    // Date
    if (!request->m_uploadDate.empty()) {
        std::string dateStr = request->m_uploadDate + " ago";
        cocos2d::CCLabelBMFont* dateLabel = cocos2d::CCLabelBMFont::create();
        m_mainLayer->addChild(dateLabel, 1);
        dateLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        dateLabel->setScale(0.6f);
        dateLabel->setColor(cocos2d::ccColor3B{150, 150, 150});
        dateLabel->setOpacity(175);
        dateLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + 170.0f - 8.0f, 
                                                 winSize.height * 0.5f + 95.0f - 12.0f));
    }
    
    // Buttons
    cocos2d::CCPoint btnPos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 95.0f + 22.0f);
    
    // Accept button
    cocos2d::CCNode* acceptBtn = ButtonSprite::create("Accept", 26.0f, true, "GJ_button_01.png", false);
    CCMenuItemSpriteExtra* acceptItem = CCMenuItemSpriteExtra::create(acceptBtn, nullptr, this, 
        menu_selector(FriendRequestPopup::onAccept));
    m_buttonMenu->addChild(acceptItem);
    acceptItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(btnPos.x - 120.0f, btnPos.y)));
    
    // Remove button
    cocos2d::CCNode* removeBtn = ButtonSprite::create("Remove", 26.0f, true, "GJ_button_04.png", false);
    CCMenuItemSpriteExtra* removeItem = CCMenuItemSpriteExtra::create(removeBtn, nullptr, this,
        menu_selector(FriendRequestPopup::onRemove));
    m_buttonMenu->addChild(removeItem);
    removeItem->setPosition(m_buttonMenu->convertToNodeSpace(btnPos));
    
    // Block button
    cocos2d::CCNode* blockBtn = ButtonSprite::create("Block", 26.0f, true, "GJ_button_06.png", false);
    CCMenuItemSpriteExtra* blockItem = CCMenuItemSpriteExtra::create(blockBtn, nullptr, this,
        menu_selector(FriendRequestPopup::onBlock));
    m_buttonMenu->addChild(blockItem);
    blockItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(btnPos.x + 120.0f, btnPos.y)));
}

void FriendRequestPopup::onAccept(cocos2d::CCObject* sender) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->acceptFriendRequest(m_request->m_accountID, m_request->m_requestID)) {
        std::string msg = "Accepting Friend Request...";
        m_popup = UploadActionPopup::create(this, msg);
        m_popup->show();
    }
}

void FriendRequestPopup::onBlock(cocos2d::CCObject* sender) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->tryGetUsername(m_request->m_accountID);
    
    std::string msg = "Are you sure you want to block <cg>" + m_request->m_username + 
        "</c>?\nThey will no longer be able to:\n"
        "-View your <cy>profile</c>.\n"
        "-Send you a <cl>friend request</c>.\n"
        "-Send you <cp>messages</c>, and all older messages from them will be <cr>deleted</c>.";
    
    FLAlertLayer* alert = FLAlertLayer::create(this, "Confirm Block", msg, "Back", "Block", false, 380.0f, true, 0.0f, 1.0f);
    alert->m_button2->updateBGImage("GJ_button_06.png");
    alert->setTag(1);
    alert->show();
}

void FriendRequestPopup::onClose(cocos2d::CCObject* sender) {
    if (m_request && !m_request->m_isRead) {
        GameLevelManager::sharedState()->readFriendRequest(m_request->m_requestID);
        m_request->m_isRead = true;
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void FriendRequestPopup::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    if (m_popup == popup) {
        bool shouldClose = m_unk;
        m_popup = nullptr;
        if (shouldClose) {
            onClose(nullptr);
        }
    }
}

void FriendRequestPopup::onRemove(cocos2d::CCObject* sender) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->deleteFriendRequests(m_request->m_accountID, nullptr, false)) {
        std::string msg = "Removing Friend Request...";
        m_popup = UploadActionPopup::create(this, msg);
        m_popup->show();
    }
}

void FriendRequestPopup::uploadActionFailed(int id, int response) {
    if (m_request && (m_request->m_requestID == id || id == m_request->m_accountID)) {
        if (m_popup) {
            std::string msg = "Failed. Please try again later.";
            m_popup->showFailMessage(msg);
        }
    }
}

void FriendRequestPopup::uploadActionFinished(int id, int response) {
    if (m_request && (m_request->m_requestID == id || id == m_request->m_accountID)) {
        m_unk = true;
        if (m_popup) {
            std::string msg = "Unknown action";
            switch (response) {
                case 30: msg = "Request removed"; break;
                case 31: msg = "Friend added!"; break;
                case 34: msg = "User blocked!"; break;
                default: break;
            }
            m_popup->showSuccessMessage(msg);
        }
    }
}

FriendRequestPopup::~FriendRequestPopup() {
    if (m_request) {
        m_request->release();
        m_request = nullptr;
    }
    if (m_circle) {
        m_circle->release();
        m_circle = nullptr;
    }
}