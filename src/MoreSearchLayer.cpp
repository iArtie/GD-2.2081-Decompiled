#include "MoreSearchLayer.h"
#include "FLAlertLayer.h"
#include "GameLevelManager.h"
#include "GameToolbox.h"
#include "LevelTools.h"
#include "CCTextInputNode.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "CCScale9Sprite.h"

void MoreSearchLayer::audioNext(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    int songFilter = glm->getIntForKey("song_filter");
    if (songFilter != 22)
        selectSong(songFilter + 1);
}

void MoreSearchLayer::audioPrevious(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    int songFilter = glm->getIntForKey("song_filter");
    if (songFilter != 1)
        selectSong(songFilter - 1);
}

MoreSearchLayer* MoreSearchLayer::create()
{
    MoreSearchLayer* ret = new MoreSearchLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool MoreSearchLayer::init()
{
    cocos2d::ccColor4B color = ccc4(0, 0, 0, 0);
    if (!FLAlertLayer::initWithColor(color, 150))
        return false;

    setTouchEnabled(true);
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(m_mainLayer);

    m_normalSongNodes = cocos2d::CCArray::create();
    m_normalSongNodes->retain();
    m_customSongNodes = cocos2d::CCArray::create();
    m_customSongNodes->retain();
    m_commonSongNodes = cocos2d::CCArray::create();
    m_commonSongNodes->retain();

    cocos2d::CCRect rect(0.0f, 0.0f, 80.0f, 80.0f);
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", rect);
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(440.0f, 290.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(MoreSearchLayer::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(ccp(winSize.width * 0.5f - 220.0f + 10.0f, winSize.height * 0.5f + 145.0f - 10.0f));
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("More Search", "bigFont.fnt");
    titleLabel->setScale(0.7f);
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 145.0f - 18.0f));

    float leftX = winSize.width * 0.5f - 170.0f;
    float midLeftX = winSize.width * 0.5f - 40.0f;
    float rightX = winSize.width * 0.5f + 90.0f;
    float topY = winSize.height * 0.5f + 90.0f;
    float bottomY = topY - 80.0f;

    GameManager_Helper:

    std::string labelStr;
    bool initialValue;

    GameLevelManager* glm;

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("uncompleted_filter");
    GameToolbox::createToggleButton("Uncompleted", this, menu_selector(MoreSearchLayer::onUncompleted),
        initialValue, m_buttonMenu, ccp(leftX, topY), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("completed_filter");
    GameToolbox::createToggleButton("Completed", this, menu_selector(MoreSearchLayer::onCompleted),
        initialValue, m_buttonMenu, ccp(midLeftX, topY), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("original_filter");
    GameToolbox::createToggleButton("Original", this, menu_selector(MoreSearchLayer::onOriginal),
        initialValue, m_buttonMenu, ccp(rightX, topY), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("coin_filter");
    GameToolbox::createToggleButton("Coins", this, menu_selector(MoreSearchLayer::onCoins),
        initialValue, m_buttonMenu, ccp(leftX, topY - 40.0f), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("twoP_filter");
    GameToolbox::createToggleButton("2-Player", this, menu_selector(MoreSearchLayer::onTwoPlayer),
        initialValue, m_buttonMenu, ccp(midLeftX, topY - 40.0f), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("enable_songFilter");
    GameToolbox::createToggleButton("Song", this, menu_selector(MoreSearchLayer::onSongFilter),
        initialValue, m_buttonMenu, ccp(rightX, topY - 40.0f), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("nostar_filter");
    GameToolbox::createToggleButton("No Star", this, menu_selector(MoreSearchLayer::onNoStar),
        initialValue, m_buttonMenu, ccp(leftX, bottomY), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("featured_filter");
    GameToolbox::createToggleButton("Featured", this, menu_selector(MoreSearchLayer::onFeatured),
        initialValue, m_buttonMenu, ccp(midLeftX, bottomY), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("epic_filter");
    GameToolbox::createToggleButton("Epic", this, menu_selector(MoreSearchLayer::onEpic),
        initialValue, m_buttonMenu, ccp(rightX, bottomY), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("legendary_filter");
    GameToolbox::createToggleButton("Legendary", this, menu_selector(MoreSearchLayer::onLegendary),
        initialValue, m_buttonMenu, ccp(leftX, topY - 120.0f), nullptr, nullptr);

    glm = GameLevelManager::sharedState();
    initialValue = !glm->getBoolForKey("mythic_filter");
    GameToolbox::createToggleButton("Mythic", this, menu_selector(MoreSearchLayer::onMythic),
        initialValue, m_buttonMenu, ccp(midLeftX, topY - 120.0f), nullptr, nullptr);

    cocos2d::CCLabelBMFont* songLabel = cocos2d::CCLabelBMFont::create("Song", "bigFont.fnt");
    m_mainLayer->addChild(songLabel);
    songLabel->setScale(0.5f);
    songLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 95.0f));
    m_commonSongNodes->addObject(songLabel);

    float normalBtnY = songLabel->getPosition().y + 25.0f;

    cocos2d::CCNode* normalNode = ButtonSprite::create(
        "Normal", 0, false, "bigFont.fnt", "GJ_button_01.png", 20.0f);
    m_normalBtn = CCMenuItemSpriteExtra::create(
        normalNode, nullptr, this, menu_selector(MoreSearchLayer::onSongMode));
    m_normalBtn->setSizeMult(1.1f);
    m_buttonMenu->addChild(m_normalBtn);
    m_normalBtn->setPosition(m_buttonMenu->convertToNodeSpace(ccp(winSize.width * 0.5f - 30.0f, normalBtnY)));
    m_normalBtn->setTag(1);
    m_commonSongNodes->addObject(m_normalBtn);

    cocos2d::CCNode* customNode = ButtonSprite::create(
        "Custom", 0, false, "bigFont.fnt", "GJ_button_01.png", 20.0f);
    m_customBtn = CCMenuItemSpriteExtra::create(
        customNode, nullptr, this, menu_selector(MoreSearchLayer::onSongMode));
    m_customBtn->setSizeMult(1.1f);
    m_buttonMenu->addChild(m_customBtn);
    m_customBtn->setPosition(m_buttonMenu->convertToNodeSpace(ccp(winSize.width * 0.5f + 30.0f, normalBtnY)));
    m_customBtn->setTag(2);
    m_commonSongNodes->addObject(m_customBtn);

    m_audioTrackName = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_audioTrackName);
    m_audioTrackName->setPosition(songLabel->getPosition() + ccp(0.0f, -25.0f));
    m_audioTrackName->setScale(0.6f);
    m_normalSongNodes->addObject(m_audioTrackName);

    float audioTrackY = m_audioTrackName->getPosition().y;

    cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftSprite->setScale(1.2f);
    m_songLeftBtn = CCMenuItemSpriteExtra::create(
        leftSprite, nullptr, this, menu_selector(MoreSearchLayer::audioPrevious));
    m_buttonMenu->addChild(m_songLeftBtn);
    m_songLeftBtn->setPosition(m_buttonMenu->convertToNodeSpace(ccp(winSize.width * 0.5f - 120.0f, audioTrackY)));
    m_songLeftBtn->setSizeMult(2.0f);
    m_normalSongNodes->addObject(m_songLeftBtn);

    cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightSprite->setScale(1.2f);
    m_songRightBtn = CCMenuItemSpriteExtra::create(
        rightSprite, nullptr, this, menu_selector(MoreSearchLayer::audioNext));
    m_buttonMenu->addChild(m_songRightBtn);
    m_songRightBtn->setPosition(m_buttonMenu->convertToNodeSpace(ccp(winSize.width * 0.5f + 120.0f, audioTrackY)));
    m_songRightBtn->setSizeMult(2.0f);
    m_normalSongNodes->addObject(m_songRightBtn);

    cocos2d::CCPoint inputPos = m_audioTrackName->getPosition();

    cocos2d::extension::CCScale9Sprite* inputBG = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    inputBG->setOpacity(100);
    inputBG->setContentSize(cocos2d::CCSize(180.0f, 30.0f));
    m_mainLayer->addChild(inputBG, -1);
    inputBG->setPosition(inputPos);
    m_customSongNodes->addObject(inputBG);

    m_enterSongID = CCTextInputNode::create(100.0f, 50.0f, "Enter song ID", "Thonburi");
    m_mainLayer->addChild(m_enterSongID, 20);
    m_enterSongID->m_delegate = this;
    m_enterSongID->setPosition(inputPos);
    m_enterSongID->setMaxLabelScale(0.7f);
    m_enterSongID->setMaxLabelWidth(160.0f);
    m_enterSongID->m_textField->setVisible(false);
    m_enterSongID->m_maxLabelLength = 15;
    m_enterSongID->setAllowedChars("0123456789");
    m_enterSongID->setLabelPlaceholderColor(ccc3(120, 170, 240));
    m_enterSongID->setLabelPlaceholderScale(0.5f);
    m_enterSongID->forceOffset();
    m_customSongNodes->addObject(m_enterSongID);

    onSongFilter(nullptr);

    glm = GameLevelManager::sharedState();
    bool isCustom = glm->getBoolForKey("customsong_filter");
    glm = GameLevelManager::sharedState();
    bool songFilterEnabled = glm->getBoolForKey("enable_songFilter");
    glm = GameLevelManager::sharedState();
    int songFilter = glm->getIntForKey("song_filter");

    if (isCustom) {
        onSongMode(m_customBtn);
        if (songFilter && songFilterEnabled) {
            cocos2d::CCString* songStr = cocos2d::CCString::createWithFormat("%i", songFilter);
            m_enterSongID->setString(songStr->getCString());
        }
    } else {
        onSongMode(m_normalBtn);
    }

    cocos2d::CCSprite* infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
        infoSprite, nullptr, this, menu_selector(MoreSearchLayer::onInfo));
    infoBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f + 220.0f - 17.0f, winSize.height * 0.5f + 145.0f - 17.0f)));

    return true;
}

void MoreSearchLayer::keyBackClicked()
{
    onClose(nullptr);
}

void MoreSearchLayer::onClose(cocos2d::CCObject* sender)
{
    m_enterSongID->onClickTrackNode(false);
    int songID = atoi(m_enterSongID->getString().c_str());

    GameLevelManager* glm = GameLevelManager::sharedState();
    bool isCustom = glm->getBoolForKey("customsong_filter");
    glm = GameLevelManager::sharedState();
    if (glm->getBoolForKey("enable_songFilter") && isCustom) {
        glm = GameLevelManager::sharedState();
        glm->setIntForKey(songID, "song_filter");
    }

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void MoreSearchLayer::onCoins(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("coin_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "coin_filter");
}

void MoreSearchLayer::onCompleted(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("completed_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "completed_filter");
}

void MoreSearchLayer::onEpic(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("epic_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "epic_filter");
}

void MoreSearchLayer::onFeatured(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("featured_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "featured_filter");
}

void MoreSearchLayer::onFollowed(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("follow_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "follow_filter");
}

void MoreSearchLayer::onFriends(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("friend_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "friend_filter");
}

void MoreSearchLayer::onInfo(cocos2d::CCObject* sender)
{
    FLAlertLayer* alert = FLAlertLayer::create(
        nullptr, "Search Info",
        "<cl>Custom Songs:</c> To only show levels using custom songs. Select <cy>Song</c> and <cy>Custom</c> but leave the textfield blank.");
    alert->show();
}

void MoreSearchLayer::onLegendary(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("legendary_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "legendary_filter");
}

void MoreSearchLayer::onMythic(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("mythic_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "mythic_filter");
}

void MoreSearchLayer::onNoStar(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("nostar_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "nostar_filter");
}

void MoreSearchLayer::onOriginal(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("original_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "original_filter");
}

void MoreSearchLayer::onSongFilter(cocos2d::CCObject* sender)
{
    if (sender) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        bool value = glm->getBoolForKey("enable_songFilter");
        glm = GameLevelManager::sharedState();
        glm->setBoolForKey(!value, "enable_songFilter");
    }

    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->getBoolForKey("enable_songFilter")) {
        glm = GameLevelManager::sharedState();
        bool isCustom = glm->getBoolForKey("customsong_filter");
        toggleSongNodes(isCustom, false);
    } else {
        glm = GameLevelManager::sharedState();
        glm->setIntForKey(0, "song_filter");
        toggleSongNodes(false, true);
        selectSong(0);
    }
}

void MoreSearchLayer::onSongMode(cocos2d::CCObject* sender)
{
    bool isCustom = sender->getTag() == 2;

    GameLevelManager* glm = GameLevelManager::sharedState();
    bool wasCustom = glm->getBoolForKey("customsong_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(isCustom, "customsong_filter");

    if (isCustom != wasCustom) {
        glm = GameLevelManager::sharedState();
        glm->setIntForKey(isCustom ? 0 : 1, "song_filter");
        m_enterSongID->setString("");
    }

    ButtonSprite* normalSprite = (ButtonSprite*)m_normalBtn->getNormalImage();
    ButtonSprite* customSprite = (ButtonSprite*)m_customBtn->getNormalImage();
    if (isCustom) {
        normalSprite->updateBGImage("GJ_button_04.png");
        customSprite->updateBGImage("GJ_button_01.png");
    } else {
        normalSprite->updateBGImage("GJ_button_01.png");
        customSprite->updateBGImage("GJ_button_04.png");
    }

    glm = GameLevelManager::sharedState();
    bool songFilterEnabled = glm->getBoolForKey("enable_songFilter");
    toggleSongNodes(isCustom, !songFilterEnabled);
    updateAudioLabel();
}

void MoreSearchLayer::onTwoPlayer(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("twoP_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "twoP_filter");
}

void MoreSearchLayer::onUncompleted(cocos2d::CCObject* sender)
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    bool value = glm->getBoolForKey("uncompleted_filter");
    glm = GameLevelManager::sharedState();
    glm->setBoolForKey(!value, "uncompleted_filter");
}

void MoreSearchLayer::selectSong(int songID)
{
    int clampedID = (songID > 22) ? 22 : songID;
    int finalID = (clampedID <= 0) ? 1 : clampedID;
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->setIntForKey(finalID, "song_filter");
    updateAudioLabel();
}

void MoreSearchLayer::textInputReturn(CCTextInputNode* node)
{
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, ccp(0.0f, 0.0f));
    m_mainLayer->runAction(move);
}

void MoreSearchLayer::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, ccp(0.0f, yOffset));
    m_mainLayer->runAction(move);
}

void MoreSearchLayer::toggleSongNodes(bool custom, bool disable)
{
    bool showNormal = !custom && !disable;
    for (unsigned int i = 0; i < m_normalSongNodes->count(); ++i) {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_normalSongNodes->objectAtIndex(i);
        node->setVisible(showNormal);
    }

    bool showCustom = custom && !disable;
    for (unsigned int i = 0; i < m_customSongNodes->count(); ++i) {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_customSongNodes->objectAtIndex(i);
        node->setVisible(showCustom);
    }

    bool showCommon = !disable;
    for (unsigned int i = 0; i < m_commonSongNodes->count(); ++i) {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_commonSongNodes->objectAtIndex(i);
        node->setVisible(showCommon);
    }
}

void MoreSearchLayer::updateAudioLabel()
{
    int audioTrack;
    int displayID;

    GameLevelManager* glm = GameLevelManager::sharedState();
    if (glm->getIntForKey("song_filter") <= 1) {
        audioTrack = 0;
        displayID = 1;
    } else {
        glm = GameLevelManager::sharedState();
        int songFilter = glm->getIntForKey("song_filter");
        audioTrack = songFilter - 1;
        displayID = songFilter;
    }

    std::string audioTitle = LevelTools::getAudioTitle(audioTrack);
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%02d: %s", displayID, audioTitle.c_str());
    m_audioTrackName->setString(str->getCString());

    float width = m_audioTrackName->getContentSize().width;
    float scale = (float)(200.0 / width);
    if (scale > 0.8f)
        scale = 0.8f;
    m_audioTrackName->setScale(scale);
}

MoreSearchLayer::~MoreSearchLayer()
{
    if (m_normalSongNodes)
        m_normalSongNodes->release();
    if (m_customSongNodes)
        m_customSongNodes->release();
    if (m_commonSongNodes)
        m_commonSongNodes->release();
}