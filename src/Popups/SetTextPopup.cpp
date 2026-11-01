#include "Popups/SetTextPopup.h"
#include "Delegates/SetTextPopupDelegate.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"

SetTextPopup* SetTextPopup::create(std::string value, std::string title, int maxLength, std::string button1, std::string button2, bool allowDefault, float textWidth) {
    SetTextPopup* popup = new SetTextPopup();
    if (popup && popup->init(value, title, maxLength, button1, button2, allowDefault, textWidth)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetTextPopup::init(std::string value, std::string title, int maxLength, std::string button1, std::string button2, bool allowDefault, float textWidth) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }

    m_value = value;

    return true;
}

void SetTextPopup::keyBackClicked() {
    onClose(nullptr);
}

void SetTextPopup::onCancel(cocos2d::CCObject* sender) {
    m_cancelled = true;
    onClose(nullptr);
}

void SetTextPopup::onClose(cocos2d::CCObject* sender) {
    m_input->m_delegate = nullptr;
    m_input->onClickTrackNode(false);

    if (m_delegate && !m_cancelled) {
        m_delegate->setTextPopupClosed(this, m_value);
    }

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetTextPopup::onResetValue(cocos2d::CCObject* sender) {
    m_value = "";
    updateTextInputLabel();
}

void SetTextPopup::show() {
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;

    if (!m_scene) {
        m_scene = runningScene;
    }

    if (!m_ZOrder) {
        m_ZOrder = 105;
    }

    m_scene->addChild(this, (int)m_ZOrder);
}

void SetTextPopup::textChanged(CCTextInputNode* node) {
    if (m_disableDelegate) {
        return;
    }

    m_value = node->getString();
}

void SetTextPopup::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void SetTextPopup::updateTextInputLabel() {
    m_disableDelegate = true;
    m_input->setString(m_value);
    m_disableDelegate = false;
}

SetTextPopup::~SetTextPopup() {
}