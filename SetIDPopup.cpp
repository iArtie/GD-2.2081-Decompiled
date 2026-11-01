#include "SetIDPopup.h"
#include "SetIDPopupDelegate.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"

bool SetIDPopup::init(int value, int minimum, int maximum, std::string title, std::string button, bool allowArrows, bool allowDefault, float textWidth, bool noElasticity, bool cancelled) {
    if (!FLAlertLayer::init(150)) {
        return false;
    }

    return true;
}

void SetIDPopup::keyBackClicked() {
    onClose(nullptr);
}

void SetIDPopup::onCancel(cocos2d::CCObject* sender) {
    m_cancelled = true;
    onClose(nullptr);
}

void SetIDPopup::onClose(cocos2d::CCObject* sender) {
    m_inputNode->m_delegate = nullptr;
    m_inputNode->onClickTrackNode(false);

    if (m_value > m_maximum) {
        m_value = m_maximum;
    }
    if (m_value >= m_minimum) {
        m_minimum = m_value;
    }
    m_value = m_minimum;

    if (m_delegate && !m_cancelled) {
        m_delegate->setIDPopupClosed(this, m_value);
    }

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetIDPopup::onItemIDArrow(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int newValue = (tag == 1) ? m_value - 1 : m_value + 1;
    if (newValue <= m_maximum) {
        m_maximum = newValue;
    }
    if (m_maximum < m_minimum) {
        m_maximum = m_minimum;
    }
    m_value = m_maximum;
    updateTextInputLabel();
}

void SetIDPopup::onResetValue(cocos2d::CCObject* sender) {
    m_value = m_default;
    updateTextInputLabel();
}

void SetIDPopup::show() {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCScene* runningScene = director->m_pRunningScene;

    if (!m_scene) {
        int highestZ = cocos2d::CCScene::getHighestChildZ(runningScene);
        m_ZOrder = 105;
        if (highestZ > 104) {
            m_ZOrder = cocos2d::CCScene::getHighestChildZ(runningScene) + 1;
        }
        m_scene = runningScene;
    }

    if (!m_ZOrder) {
        m_ZOrder = 105;
    }

    m_scene->addChild(this);
}

void SetIDPopup::textChanged(CCTextInputNode* node) {
    if (m_disableDelegate) {
        return;
    }

    int val = atoi(node->getString().c_str());
    m_value = val;

    if (m_value > m_maximum) {
        m_value = m_maximum;
    }
    if (m_value < m_minimum) {
        m_value = m_minimum;
    }

    valueChanged();
}

void SetIDPopup::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void SetIDPopup::updateTextInputLabel() {
    if (m_value > m_maximum) {
        m_value = m_maximum;
    }
    m_disableDelegate = true;
    if (m_value < m_minimum) {
        m_value = m_minimum;
    }

    m_inputNode->setString(cocos2d::CCString::createWithFormat("%i", m_value)->getCString());
    valueChanged();
    m_disableDelegate = false;
}

void SetIDPopup::valueChanged() {
}

SetIDPopup::~SetIDPopup() {
}