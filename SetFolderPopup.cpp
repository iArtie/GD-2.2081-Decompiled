#include "SetFolderPopup.h"
#include "SetTextPopup.h"
#include "GameLevelManager.h"
#include "CCMenuItemSpriteExtra.h"

SetFolderPopup* SetFolderPopup::create(int value, bool isCreated, std::string title) {
    SetFolderPopup* popup = new SetFolderPopup();
    if (popup && popup->init(value, isCreated, title)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetFolderPopup::init(int value, bool isCreated, std::string title) {
    if (!SetIDPopup::init(value, 0, 999, title, "OK", true, false, 60.0f, false, false)) {
        return false;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_isCreated = isCreated;

    m_titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_titleLabel->setScale(0.4f);
    m_mainLayer->addChild(m_titleLabel, 1);
    m_titleLabel->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f + 35.0f + 60.0f));

    cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optionsSprite->setScale(0.5f);
    CCMenuItemSpriteExtra* optionsButton = CCMenuItemSpriteExtra::create(
        optionsSprite, nullptr, menu_selector(SetFolderPopup::onSetFolderName));
    optionsButton->setSizeMult(1.1f);
    m_buttonMenu->addChild(optionsButton);
    cocos2d::CCPoint optionsPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(
            winSize.width * 0.5f + 90.0f,
            winSize.height * 0.5f + 20.0f + 60.0f));
    optionsButton->setPosition(optionsPos);

    valueChanged();

    return true;
}

void SetFolderPopup::onSetFolderName(cocos2d::CCObject* sender) {
    if (m_value <= 0) {
        return;
    }

    GameLevelManager* glm = GameLevelManager::sharedState();
    std::string currentName = glm->getFolderName(m_value, m_isCreated);

    SetTextPopup* textPopup = SetTextPopup::create(
        currentName,
        "Folder Name",
        20,
        "Set Folder Name",
        "OK",
        true,
        60.0f);
    textPopup->m_delegate = this;
    textPopup->show();
}

void SetFolderPopup::setTextPopupClosed(SetTextPopup* popup, std::string text) {
    GameLevelManager::sharedState()->setFolderName(m_value, text, m_isCreated);
    valueChanged();
}

void SetFolderPopup::valueChanged() {
    if (!m_titleLabel) {
        return;
    }

    std::string folderName = GameLevelManager::sharedState()->getFolderName(m_value, m_isCreated);
    m_titleLabel->setString(folderName.c_str());
}

SetFolderPopup::~SetFolderPopup() {
}