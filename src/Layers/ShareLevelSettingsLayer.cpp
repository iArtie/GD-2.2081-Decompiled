#include "Layers/ShareLevelSettingsLayer.h"
#include "Models/GJGameLevel.h"
#include "Managers/GameManager.h"
#include "UI/InfoAlertButton.h"
#include "UI/TextArea.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"

ShareLevelSettingsLayer* ShareLevelSettingsLayer::create(GJGameLevel* level) {
    ShareLevelSettingsLayer* returnVal = new ShareLevelSettingsLayer();
    if (returnVal->init(level)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool ShareLevelSettingsLayer::init(GJGameLevel* level) {
    if (!FLAlertLayer::init(75))
        return false;

    m_level = level;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(360.0f, 220.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -1);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Level Settings", "bigFont.fnt");
    titleLabel->setPosition(centerX, centerY + 110.0f - 20.0f);
    m_mainLayer->addChild(titleLabel);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    InfoAlertButton* infoBtn = InfoAlertButton::create(
        "Help",
        "<cg>Unlisted</c> makes it so that your level can only be found by <cy>searching for the exact ID</c>. It will <cr>"
        "not</c> be visible on your profile, from search, or anywhere else. Unlisted levels do <cr>not</c> have to be <cp>v"
        "erified</c> to be uploaded.\n"
        "<co>Friends Only</c> makes it so that only <cl>friends</c> who search the exact ID can find it.",
        1.0f);
    m_buttonMenu->addChild(infoBtn);
    cocos2d::CCPoint infoPos(centerX + 180.0f - 20.0f, centerY + 110.0f - 20.0f);
    infoBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &infoPos));

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(ShareLevelSettingsLayer::onClose));
    closeBtn->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeBtn);
    cocos2d::CCPoint closePos(centerX - 180.0f + 5.0f, centerY + 110.0f - 5.0f);
    closeBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &closePos));

    m_passwordLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_passwordLabel);
    m_passwordLabel->setPosition(centerX, centerY - 110.0f + 80.0f);
    m_passwordLabel->setScale(0.6f);

    cocos2d::CCPoint unlistedPos(centerX - 140.0f, centerY + 45.0f);

    cocos2d::CCSprite* checkOn1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    cocos2d::CCSprite* checkOff1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    checkOn1->setScale(0.8f);
    checkOff1->setScale(0.8f);

    CCMenuItemToggler* unlistedToggler = CCMenuItemToggler::create(
        checkOn1, checkOff1, this, menu_selector(ShareLevelSettingsLayer::onUnlisted));
    unlistedToggler->toggle(!m_level->m_unlisted);
    m_buttonMenu->addChild(unlistedToggler);
    unlistedToggler->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &unlistedPos));
    unlistedToggler->setSizeMult(1.5f);

    cocos2d::CCLabelBMFont* unlistedLabel = cocos2d::CCLabelBMFont::create("Unlisted", "bigFont.fnt");
    m_mainLayer->addChild(unlistedLabel);
    unlistedLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    unlistedLabel->setPosition(
        unlistedPos + cocos2d::CCPoint((checkOn1->getContentSize().width * 0.5f * 0.8f) + 8.0f, 0.0f));
    unlistedLabel->limitLabelWidth(110.0f, 0.35f, 0.0f);

    if (m_level->m_isUploaded || m_level->m_levelVersion > 1) {
        unlistedToggler->setEnabled(false);
        checkOn1->setColor(cocos2d::ccc3(128, 128, 128));
        checkOff1->setColor(cocos2d::ccc3(128, 128, 128));
        unlistedLabel->setColor(cocos2d::ccc3(128, 128, 128));
    }

    cocos2d::CCPoint friendsOnlyPos(centerX + 40.0f, unlistedPos.y);

    cocos2d::CCSprite* checkOn2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    cocos2d::CCSprite* checkOff2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    checkOn2->setScale(0.8f);
    checkOff2->setScale(0.8f);

    m_friendsOnlyToggler = CCMenuItemToggler::create(
        checkOff2, checkOn2, this, menu_selector(ShareLevelSettingsLayer::onUnlistedFriendsOnly));
    m_friendsOnlyToggler->toggle(m_level->m_friendsOnly);
    m_buttonMenu->addChild(m_friendsOnlyToggler);
    m_friendsOnlyToggler->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &friendsOnlyPos));
    m_friendsOnlyToggler->setSizeMult(1.5f);

    m_friendsOnlyLabel = cocos2d::CCLabelBMFont::create("Friends Only", "bigFont.fnt");
    m_mainLayer->addChild(m_friendsOnlyLabel);
    m_friendsOnlyLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_friendsOnlyLabel->setPosition(
        friendsOnlyPos + cocos2d::CCPoint((checkOn2->getContentSize().width * 0.5f * 0.8f) + 8.0f, 0.0f));
    m_friendsOnlyLabel->limitLabelWidth(110.0f, 0.35f, 0.0f);

    updateSettingsState();

    TextArea* infoText = TextArea::create(
        "Starting in 2.2, all new levels are free to copy.\n(Older levels retain any copy settings.)",
        "chatFont.fnt", 1.0f, 320.0f, cocos2d::CCPoint(20.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(infoText);
    infoText->setPosition(centerX, centerY - 110.0f + 35.0f);
    infoText->setScale(0.65f);

    return true;
}

void ShareLevelSettingsLayer::keyBackClicked() {
    onClose(nullptr);
}

void ShareLevelSettingsLayer::onClose(cocos2d::CCObject* sender) {
    if (m_level->m_password.random - m_level->m_password.seed == -1) {
        int password = (int)(float)((float)((float)rand() * 4.6566e-10) * 1000.0f);
        m_level->m_password.seed = password;
        m_level->m_password.random = password + 1;
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ShareLevelSettingsLayer::onUnlisted(cocos2d::CCObject* sender) {
    m_level->m_unlisted = !m_level->m_unlisted;

    GameManager* gameManager = GameManager::sharedState();
    if (!GameManager::getGameVariable(gameManager, "0070")) {
        GameManager::sharedState()->setGameVariable("0070", true);
        FLAlertLayer::create(
            nullptr,
            "Unlisted",
            "An unlisted level can only be found by entering its levelID. "
            "Once a level is unlisted, it cannot change back to listed.")->show();
    }

    if (!m_level->m_unlisted)
        m_level->m_friendsOnly = false;

    updateSettingsState();
}

void ShareLevelSettingsLayer::onUnlistedFriendsOnly(cocos2d::CCObject* sender) {
    m_level->m_friendsOnly = !m_level->m_friendsOnly;
}

void ShareLevelSettingsLayer::updateSettingsState() {
    cocos2d::ccColor3B color;
    if (m_level->m_unlisted) {
        color = cocos2d::ccc3(255, 255, 255);
        m_friendsOnlyToggler->setEnabled(true);
    } else {
        m_friendsOnlyToggler->toggle(false);
        color = cocos2d::ccc3(128, 128, 128);
        m_friendsOnlyToggler->setEnabled(false);
    }

    ((cocos2d::CCSprite*)m_friendsOnlyToggler->m_offButton->getNormalImage())->setColor(color);
    ((cocos2d::CCSprite*)m_friendsOnlyToggler->m_onButton->getNormalImage())->setColor(color);
    m_friendsOnlyLabel->setColor(color);
}

ShareLevelSettingsLayer::~ShareLevelSettingsLayer() {
}