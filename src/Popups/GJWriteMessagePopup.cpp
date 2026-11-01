#include "Popups/GJWriteMessagePopup.h"
#include "Cocos/CCTextInputNode.h"
#include "Popups/UploadActionPopup.h"
#include "Layers/FLAlertLayer.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "GameLevelManager.h"
#include "Models/GJUserMessage.h"
#include "Popups/GJMessagePopup.h"

GJWriteMessagePopup* GJWriteMessagePopup::create(int accountID, int messageID) {
    auto popup = new GJWriteMessagePopup();
    if (popup && popup->init(accountID, messageID)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool GJWriteMessagePopup::init(int accountID, int messageID) {
    if (!FLAlertLayer::initWithColor(cocos2d::ccc4(0, 0, 0, 150))) {
        return false;
    }
    
    FLAlertLayer::incrementForcePrio();
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    m_messageID = accountID;
    m_accountID = messageID;
    m_uploadPopup = nullptr;
    m_uploadSuccess = false;
    
    GJUserMessage* storedReply = nullptr;
    if (messageID > 0) {
        storedReply = GameLevelManager::sharedState()->getStoredUserMessageReply(m_accountID);
    }
    
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
    
    auto titleLabel = cocos2d::CCLabelBMFont::create("New Message", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 104.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    std::string recipientText = "To: " + GameLevelManager::sharedState()->tryGetUsername(m_messageID);
    auto recipientLabel = cocos2d::CCLabelBMFont::create(recipientText.c_str(), "bigFont.fnt");
    recipientLabel->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    m_mainLayer->addChild(recipientLabel);
    recipientLabel->setScale(0.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    const char* closeText = (m_accountID) ? "Back" : "Cancel";
    auto closeBtn = ButtonSprite::create(closeText, 0, 1, 0.55f, true, "bigFont.fnt", "GJ_button_05.png", nullptr, 26.0f);
    auto closeItem = CCMenuItemSpriteExtra::create(closeBtn, this, menu_selector(GJWriteMessagePopup::onClose));
    m_buttonMenu->addChild(closeItem);
    
    auto sendBtn = ButtonSprite::create("Send", 0, 1, 0.55f, true, "bigFont.fnt", "GJ_button_01.png", nullptr, 26.0f);
    auto sendItem = CCMenuItemSpriteExtra::create(sendBtn, this, menu_selector(GJWriteMessagePopup::onSend));
    m_buttonMenu->addChild(sendItem);
    
    cocos2d::CCPoint btnPos = m_mainLayer->getPosition() + cocos2d::CCPoint(centerX, centerY - 120.0f + 22.0f);
    cocos2d::CCArray* btnArray = cocos2d::CCArray::create();
    btnArray->addObject(closeItem);
    btnArray->addObject(sendItem);
    GameToolbox::alignItemsHorisontally(btnArray, 15.0f, m_buttonMenu->convertToNodeSpace(btnPos));
    
    cocos2d::CCPoint subjectPos(centerX, centerY + 53.0f);
    
    auto subjectBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    subjectBg->setOpacity(100);
    subjectBg->setColor(cocos2d::ccc3(100, 100, 100));
    subjectBg->setContentSize(cocos2d::CCSize(370.0f, 28.0f));
    m_mainLayer->addChild(subjectBg, 1);
    subjectBg->setPosition(subjectPos);
    subjectBg->setColor(cocos2d::ccc3(0, 0, 0));
    
    m_subjectInput = CCTextInputNode::create(370.0f, 50.0f, "Subject", "Thonburi");
    m_subjectInput->m_filterSwearWords = false;
    m_mainLayer->addChild(m_subjectInput, 20);
    m_subjectInput->m_delegate = this;
    m_subjectInput->setPosition(subjectPos);
    m_subjectInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");
    m_subjectInput->m_maxLabelLength = 35;
    m_subjectInput->m_textField->setVisible(false);
    m_subjectInput->setTag(0);
    
    auto subjectTextArea = TextArea::create("", "chatFont.fnt", 1.0f, 305.0f, cocos2d::CCPoint(20.0f, 0.0f), 0.5f, cocos2d::CCPoint(0.5f, 0.5f));
    m_subjectInput->addTextArea(subjectTextArea);
    
    m_subjectCountLabel = cocos2d::CCLabelBMFont::create("35", "bigFont.fnt");
    m_mainLayer->addChild(m_subjectCountLabel, 2);
    m_subjectCountLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_subjectCountLabel->setPosition(subjectPos + cocos2d::CCPoint(175.0f, 0.0f));
    updateSubject("");
    
    cocos2d::CCPoint messagePos(centerX, centerY - 25.0f);
    
    auto messageBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    messageBg->setOpacity(100);
    messageBg->setContentSize(cocos2d::CCSize(370.0f, 106.0f));
    m_mainLayer->addChild(messageBg, 1);
    messageBg->setPosition(messagePos);
    messageBg->setColor(cocos2d::ccc3(0, 0, 0));
    
    m_messageInput = CCTextInputNode::create(370.0f, 50.0f, "Message", "Thonburi");
    m_messageInput->m_filterSwearWords = false;
    m_mainLayer->addChild(m_messageInput, 20);
    m_messageInput->m_delegate = this;
    m_messageInput->setPosition(messagePos);
    m_messageInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");
    m_messageInput->m_maxLabelLength = 200;
    m_messageInput->m_textField->setVisible(false);
    m_messageInput->setTag(1);
    m_messageInput->setContentSize(cocos2d::CCSize(370.0f, 96.0f));
    m_messageInput->m_forceOffset = true;
    
    auto messageTextArea = TextArea::create("", "chatFont.fnt", 1.0f, 305.0f, cocos2d::CCPoint(20.0f, 0.0f), 0.5f, cocos2d::CCPoint(0.5f, 0.5f));
    m_messageInput->addTextArea(messageTextArea);
    
    m_messageCountLabel = cocos2d::CCLabelBMFont::create("200", "bigFont.fnt");
    m_mainLayer->addChild(m_messageCountLabel, 2);
    m_messageCountLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_messageCountLabel->setPosition(cocos2d::CCPoint(centerX + 210.0f - 10.0f, centerY + 120.0f - 14.0f));
    updateBody("");
    
    auto clearSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
    auto clearItem = CCMenuItemSpriteExtra::create(clearSprite, this, menu_selector(GJWriteMessagePopup::onClearBody));
    m_buttonMenu->addChild(clearItem);
    clearItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX + 210.0f - 18.0f, centerY - 120.0f + 20.0f)));
    
    if (storedReply) {
        m_subjectInput->setString(storedReply->m_title);
        m_messageInput->setString(storedReply->m_content);
    }
    
    return true;
}

void GJWriteMessagePopup::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2 && layer->getTag() == 1) {
        m_messageInput->setString("");
    }
}

void GJWriteMessagePopup::closeMessagePopup(bool fullExit) {
    m_messageInput->onClickTrackNode(false);
    m_subjectInput->onClickTrackNode(false);
    
    if (!fullExit && m_accountID) {
        std::string subject = m_subjectText;
        std::string body = m_messageText;
        
        if (!subject.empty() || !body.empty()) {
            auto reply = GJUserMessage::create();
            reply->m_title = subject;
            reply->m_content = body;
            GameLevelManager::sharedState()->storeUserMessageReply(m_accountID, reply);
        }
        
        auto stored = GameLevelManager::sharedState()->getStoredUserMessage(m_accountID);
        if (stored) {
            auto popup = GJMessagePopup::create(stored);
            popup->m_noElasticity = true;
            popup->show();
        }
    }
    
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJWriteMessagePopup::keyBackClicked() {
    onClose(nullptr);
}

void GJWriteMessagePopup::onClearBody(cocos2d::CCObject* sender) {
    auto alert = FLAlertLayer::create(
        this,
        "Reset Message",
        "Are you sure you want to reset this message?",
        "Cancel",
        "Reset",
        380.0f
    );
    alert->setTag(1);
    alert->show();
}

void GJWriteMessagePopup::onClose(cocos2d::CCObject* sender) {
    closeMessagePopup(false);
}

void GJWriteMessagePopup::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    if (m_uploadPopup == popup) {
        m_uploadPopup = nullptr;
    }
    if (m_uploadSuccess) {
        closeMessagePopup(true);
    }
}

void GJWriteMessagePopup::onSend(cocos2d::CCObject* sender) {
    m_messageInput->onClickTrackNode(false);
    m_subjectInput->onClickTrackNode(false);
    
    if (m_subjectText.empty()) {
        auto alert = FLAlertLayer::create(nullptr, "No subject", "You must enter a <cg>subject</c> for the message", "Ok", nullptr, 380.0f);
        alert->show();
        return;
    }
    
    if (m_messageText.empty()) {
        auto alert = FLAlertLayer::create(nullptr, "No message", "You can't send an <cr>empty</c> message.", "Ok", nullptr, 380.0f);
        alert->show();
        return;
    }
    
    GameLevelManager::sharedState()->m_uploadMessageDelegate = this;
    GameLevelManager::sharedState()->uploadUserMessage(m_messageID, m_subjectText, m_messageText);
    
    auto popup = UploadActionPopup::create(this, "Sending...");
    m_uploadPopup = popup;
    popup->show();
}

void GJWriteMessagePopup::registerWithTouchDispatcher() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void GJWriteMessagePopup::textChanged(CCTextInputNode* node) {
    const char* text = node->getString();
    int tag = node->getTag();
    
    if (tag == 0) {
        updateSubject(text);
    } else if (tag == 1) {
        updateBody(text);
    }
}

void GJWriteMessagePopup::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void GJWriteMessagePopup::textInputOpened(CCTextInputNode* node) {
}

void GJWriteMessagePopup::textInputReturn(CCTextInputNode* node) {
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0.0f, 0.0f)));
}

void GJWriteMessagePopup::textInputShouldOffset(CCTextInputNode* node, float yOffset) {
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0.0f, yOffset)));
}

void GJWriteMessagePopup::updateBody(const std::string& body) {
    updateText(body, 1);
}

void GJWriteMessagePopup::updateSubject(const std::string& subject) {
    updateText(subject, 0);
}

void GJWriteMessagePopup::updateText(const std::string& text, int type) {
    TextArea* textArea = nullptr;
    int maxLength = 0;
    float width = 0.0f;
    
    if (type == 1) {
        m_messageText = text;
        textArea = m_messageInput->m_textArea;
        maxLength = 200;
        width = 290.0f;
    } else {
        m_subjectText = text;
        textArea = m_subjectInput->m_textArea;
        maxLength = 35;
    }
    
    if (text.empty()) {
        textArea->setString(type == 1 ? "Message" : "Subject");
        textArea->colorAllCharactersTo(cocos2d::ccc3(200, 100, 100));
    } else {
        textArea->setString(text);
        textArea->colorAllCharactersTo(cocos2d::ccc3(255, 255, 255));
        if (type == 1) {
            textArea->setScale(1.0f);
            textArea->m_width = 290.0f;
        }
    }
    
    updateCharCountLabel(type);
}

void GJWriteMessagePopup::updateCharCountLabel(int type) {
    const std::string* text = nullptr;
    cocos2d::CCLabelBMFont* label = nullptr;
    int maxLength = 0;
    float warnThreshold = 0.0f;
    
    if (type == 1) {
        text = &m_messageText;
        label = m_messageCountLabel;
        maxLength = 200;
        warnThreshold = 140.0f;
    } else {
        text = &m_subjectText;
        label = m_subjectCountLabel;
        maxLength = 35;
        warnThreshold = 24.5f;
    }
    
    int length = text->length();
    int remaining = maxLength - length;
    
    if (length < warnThreshold) {
        label->setOpacity(100);
        label->setColor(cocos2d::ccc3(125, 125, 125));
    } else {
        label->setOpacity(255);
        label->setColor(cocos2d::ccc3(255, 0, 0));
    }
    
    label->setString(std::to_string(remaining).c_str());
}

void GJWriteMessagePopup::uploadMessageFailed(int accountID) {
    if (m_messageID == accountID && m_uploadPopup) {
        m_uploadPopup->showFailMessage("Failed. Please try again later.");
    }
}

void GJWriteMessagePopup::uploadMessageFinished(int accountID) {
    if (m_messageID == accountID && m_uploadPopup) {
        m_uploadPopup->showSuccessMessage("Message sent!");
        m_uploadSuccess = true;
    }
}

GJWriteMessagePopup::~GJWriteMessagePopup() {
}