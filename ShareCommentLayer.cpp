#include "ShareCommentLayer.h"
#include "ShareCommentDelegate.h"
#include "UploadActionPopup.h"
#include "GameLevelManager.h"
#include "GJMultiplayerManager.h"
#include "CCTextInputNode.h"

void ShareCommentLayer::keyBackClicked() {
    onClose(nullptr);
}

void ShareCommentLayer::onClear(cocos2d::CCObject* sender) {
    updateDescText("");
    m_commentInput->setString("");
}

void ShareCommentLayer::onClose(cocos2d::CCObject* sender) {
    m_commentInput->onClickTrackNode(false);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ShareCommentLayer::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    if (m_uploadPopup == popup)
        m_uploadPopup = nullptr;
    if (m_uploadSuccess)
        onClose(nullptr);
}

void ShareCommentLayer::onPercent(cocos2d::CCObject* sender) {
    m_percentEnabled = !m_percentEnabled;
    updatePercentLabel();
}

void ShareCommentLayer::onShare(cocos2d::CCObject* sender) {
    if (m_commentType == CommentType::FriendRequest) {
        GameLevelManager::sharedState()->m_uploadActionDelegate = this;
        bool flag = GameLevelManager::sharedState()->uploadFriendRequest(m_itemID, m_descText);
        if (flag) {
            m_uploadPopup = UploadActionPopup::create(this, "Sending...");
            m_uploadPopup->show();
        }
    } else if (m_commentType == CommentType::Multiplayer) {
        GJMultiplayerManager::sharedState()->uploadComment(m_descText, m_itemID);
    } else if (m_commentType == CommentType::Account) {
        GameLevelManager::sharedState()->uploadAccountComment(m_descText);
    } else if (m_commentType == CommentType::Level) {
        int percent = 0;
        if (m_percentEnabled)
            percent = m_percent;
        GameLevelManager::sharedState()->uploadLevelComment(m_itemID, m_descText, percent);
    } else {
        if (m_delegate)
            m_delegate->shareCommentClosed(m_descText, this);
    }
    onClose(nullptr);
}

void ShareCommentLayer::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void ShareCommentLayer::textChanged(CCTextInputNode* node) {
    updateDescText(node->getString());
}

void ShareCommentLayer::textInputClosed(CCTextInputNode* node) {
    m_descText = node->getString();
    updateDescText(m_descText);
}

void ShareCommentLayer::textInputOpened(CCTextInputNode* node) {
}

void ShareCommentLayer::updateCharCountLabel() {
    int length = m_descText.length();
    if ((float)length < (float)((float)m_charLimit * 0.7f)) {
        m_charCountLabel->setOpacity(255);
        m_charCountLabel->setColor(cocos2d::ccc3(255, 255, 255));
    } else if ((float)length < (float)((float)m_charLimit * 0.9f)) {
        m_charCountLabel->setOpacity(255);
        m_charCountLabel->setColor(cocos2d::ccc3(255, 255, 255));
    } else {
        m_charCountLabel->setOpacity(255);
        m_charCountLabel->setColor(cocos2d::ccc3(255, 0, 0));
    }
    int remaining = m_charLimit - length;
    m_charCountLabel->setString(cocos2d::CCString::createWithFormat("%i", remaining)->getCString());
}

void ShareCommentLayer::updateDescText(std::string desc) {
    m_descText = desc;
    updateCharCountLabel();
}

void ShareCommentLayer::updatePercentLabel() {
    if (m_percentEnabled) {
        m_percentLabel->setOpacity(255);
        m_percentLabel->setColor(cocos2d::ccc3(255, 255, 255));
        m_percentLabel->setString(cocos2d::CCString::createWithFormat("%i%%", m_percent)->getCString());
    } else {
        m_percentLabel->setOpacity(125);
        m_percentLabel->setColor(cocos2d::ccc3(255, 255, 255));
        m_percentLabel->setString("%");
    }
}

void ShareCommentLayer::uploadActionFailed(int id, int response) {
    if (m_itemID == id) {
        if (m_uploadPopup) {
            m_uploadPopup->showFailMessage("Failed. Please try again later.");
        }
    }
}

void ShareCommentLayer::uploadActionFinished(int id, int response) {
    if (m_itemID == id) {
        if (m_uploadPopup) {
            m_uploadPopup->showSuccessMessage("Friend request sent!");
            m_uploadSuccess = true;
        }
    }
}

ShareCommentLayer::~ShareCommentLayer() {
}