#include "Layers/ShareListLayer.h"
#include "Models/GJLevelList.h"
#include "Utils/GameToolbox.h"
#include "UI/TextArea.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/InfoAlertButton.h"
#include "Popups/UploadListPopup.h"

ShareListLayer* ShareListLayer::create(GJLevelList* list) {
    ShareListLayer* returnVal = new ShareListLayer();
    if (returnVal->init(list)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool ShareListLayer::init(GJLevelList* list) {
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    FLAlertLayer::incrementForcePrio(this);
    m_list = list;
    m_list->retain();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(420.0f, 300.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -1);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(m_list->m_listName.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setPosition(centerX, centerY + 150.0f - 20.0f);

    TextArea* warningText = TextArea::create(
        "<cy>You cannot change the name once it is shared.</c>",
        "chatFont.fnt", 1.0f, 400.0f, cocos2d::CCPoint(20.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(warningText);
    warningText->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    warningText->setScale(0.8f);

    cocos2d::CCPoint descPoint(centerX, centerY + 45.0f);
    cocos2d::extension::CCScale9Sprite* descBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    descBg->setContentSize(cocos2d::CCSize(380.0f, 80.0f));
    m_mainLayer->addChild(descBg, -1);
    descBg->setPosition(descPoint);
    descBg->setColor(cocos2d::ccc3(0, 0, 0));
    descBg->setOpacity(100);

    std::string descText = m_list->getUnpackedDescription();
    if (descText.empty())
        descText = "(No description provided)";

    size_t pos = 0;
    while ((pos = descText.find("\\n", pos)) != std::string::npos) {
        descText.replace(pos, 2, "\n");
    }

    float scale;
    if (descText.length() <= 0x12C) {
        if (descText.length() > 0x46)
            scale = (float)((float)(180 - (int)descText.length()) / 300.0f) * 0.2f + 0.8f;
        else
            scale = 1.0f;
    } else {
        scale = 0.72f;
    }

    TextArea* descArea = TextArea::create(
        descText, "chatFont.fnt", 1.0f, 320.0f / scale,
        cocos2d::CCPoint(20.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(descArea);
    descArea->setPosition(descPoint + cocos2d::CCPoint(0.0f, 1.0f));
    descArea->setScale(scale);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCArray* buttons = cocos2d::CCArray::create();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel");
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(
        cancelSprite, nullptr, menu_selector(ShareListLayer::onClose));
    m_buttonMenu->addChild(cancelBtn);
    buttons->addObject(cancelBtn);

    ButtonSprite* shareSprite = ButtonSprite::create("Share List");
    CCMenuItemSpriteExtra* shareBtn = CCMenuItemSpriteExtra::create(
        shareSprite, nullptr, menu_selector(ShareListLayer::onShare));
    m_buttonMenu->addChild(shareBtn);
    buttons->addObject(shareBtn);

    cocos2d::CCPoint buttonPos(centerX, centerY - 150.0f + 25.0f);
    cocos2d::CCPoint menuPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &buttonPos);
    GameToolbox::alignItemsHorisontally(buttons, 15.0f, menuPos, false);

    cocos2d::CCSprite* infoIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoIcon, nullptr, menu_selector(ShareListLayer::onInfo));
    infoBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(infoBtn);
    cocos2d::CCPoint infoPos(centerX - 210.0f + 20.0f, centerY + 150.0f - 20.0f);
    infoBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &infoPos));

    cocos2d::CCPoint unlistedPos(centerX - 120.0f, centerY - 70.0f);
    m_mainLayer = m_mainLayer;
    cocos2d::CCPoint unlistedOffset(8.0f, 0.0f);
    GameToolbox::createToggleButton(
        "Unlisted",
        menu_selector(ShareListLayer::onUnlisted),
        false,
        m_list->m_unlisted,
        m_buttonMenu,
        &unlistedPos,
        this,
        m_mainLayer,
        0.8f,
        0.4f,
        0.0f,
        &unlistedOffset,
        "bigFont.fnt",
        nullptr,
        0,
        0);

    InfoAlertButton* unlistedInfo = InfoAlertButton::create(
        "Info",
        "Makes it so that your list can only be found by <cy>searching for the exact ID</c>. It will <cr>not</c> be visible on your profile, from search, or anywhere else.",
        0.8f);
    m_buttonMenu->addChild(unlistedInfo);
    cocos2d::CCPoint unlistedInfoPos(centerX - 120.0f - 30.0f, centerY - 70.0f);
    unlistedInfo->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &unlistedInfoPos));

    cocos2d::CCPoint friendsOnlyPos(centerX + 40.0f, centerY - 70.0f);
    cocos2d::CCPoint friendsOnlyOffset(8.0f, 0.0f);
    m_friendsOnlyToggler = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Friends Only",
        menu_selector(ShareListLayer::onUnlisted),
        false,
        m_list->m_friendsOnly,
        m_buttonMenu,
        &friendsOnlyPos,
        this,
        m_mainLayer,
        0.8f,
        0.4f,
        0.0f,
        &friendsOnlyOffset,
        "bigFont.fnt",
        nullptr,
        0,
        0);
    m_friendsOnlyToggler->setTag(1);

    InfoAlertButton* friendsOnlyInfo = InfoAlertButton::create(
        "Info",
        "Makes it so that your list can only be found by <cg>friends</c> who <cy>search for the exact ID</c>. It will <cr>not</c> be visible on your profile, from search, or anywhere else.",
        0.8f);
    m_buttonMenu->addChild(friendsOnlyInfo);
    cocos2d::CCPoint friendsOnlyInfoPos(centerX + 40.0f - 30.0f, centerY - 70.0f);
    friendsOnlyInfo->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &friendsOnlyInfoPos));

    updateUnlistedF();

    std::string diffFrame = GJLevelList::frameForListDifficulty(m_list->m_difficulty, 0);
    cocos2d::CCSprite* diffSprite = cocos2d::CCSprite::createWithSpriteFrameName(diffFrame.c_str());
    m_mainLayer->addChild(diffSprite, 1);
    diffSprite->setPosition(centerX - 50.0f, centerY - 25.0f);

    int totalLevels = m_list->totalLevels();
    cocos2d::CCLabelBMFont* levelsLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i Levels", totalLevels)->getCString(), "bigFont.fnt");
    levelsLabel->setScale(0.8f);
    m_mainLayer->addChild(levelsLabel, 1);
    levelsLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    levelsLabel->setPosition(diffSprite->getPosition() + cocos2d::CCPoint(30.0f, 0.0f));

    return true;
}

void ShareListLayer::keyBackClicked() {
    onClose(nullptr);
}

void ShareListLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ShareListLayer::onInfo(cocos2d::CCObject* sender) {
    FLAlertLayer::create(
        nullptr,
        "Share List",
        "After uploading, a list's visibility can be toggled freely between public or unlisted. However, this does <cr>not</c> apply if the list gets <cy>Featured</c>, in which case it is locked to public. (This does not reward Creator Points.) Levels that are <cg>unlisted and friends only</c> are <cr>not</c> included in the list.")->show();
}

void ShareListLayer::onShare(cocos2d::CCObject* sender) {
    if (m_list->totalLevels()) {
        UploadListPopup* popup = UploadListPopup::create(m_list);
        popup->show();
        onClose(nullptr);
    } else {
        FLAlertLayer::create(nullptr, "Empty", "You can't share an empty list.")->show();
    }
}

void ShareListLayer::onUnlisted(cocos2d::CCObject* sender) {
    if (sender->getTag() == 1) {
        m_list->m_friendsOnly = !m_list->m_friendsOnly;
    } else {
        bool wasUnlisted = m_list->m_unlisted;
        m_list->m_unlisted = !wasUnlisted;
        if (wasUnlisted)
            m_list->m_friendsOnly = false;
        updateUnlistedF();
    }
}

void ShareListLayer::updateUnlistedF() {
    cocos2d::ccColor3B color;
    if (m_list->m_unlisted) {
        color = cocos2d::ccc3(255, 255, 255);
        m_friendsOnlyToggler->setEnabled(true);
    } else {
        color = cocos2d::ccc3(128, 128, 128);
        m_friendsOnlyToggler->toggle(false);
        m_friendsOnlyToggler->setEnabled(false);
    }

    ((cocos2d::CCSprite*)m_friendsOnlyToggler->m_offButton->getNormalImage())->setColor(color);
    ((cocos2d::CCSprite*)m_friendsOnlyToggler->m_onButton->getNormalImage())->setColor(color);
    m_friendsOnlyLabel->setColor(color);
}

ShareListLayer::~ShareListLayer() {
}