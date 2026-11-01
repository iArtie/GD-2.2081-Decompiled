#include "Layers/GJAccountSettingsLayer.h"
#include "Cocos/CCTextInputNode.h"
#include "Cocos/CCMenuItemToggler.h"
#include "UI/InfoAlertButton.h"
#include "Popups/UpdateAccountSettingsPopup.h"
#include "GameLevelManager.h"
#include "Models/GJUserScore.h"
#include "Utils/GameToolbox.h"

GJAccountSettingsLayer* GJAccountSettingsLayer::create(int accountID)
{
    GJAccountSettingsLayer* instance = new GJAccountSettingsLayer();
    if (instance && instance->init(accountID))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJAccountSettingsLayer::init(int accountID)
{
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 150)))
        return false;

    m_noElasticity = true;
    m_accountID = accountID;
    m_joystickConnected = -1;

    setTouchEnabled(true);
    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    // Load user data
    GameLevelManager* glm = GameLevelManager::sharedState();
    GJUserScore* userScore = glm->userInfoForAccountID(accountID);

    if (userScore)
    {
        m_messageStatus = userScore->m_messageState;
        m_friendStatus = userScore->m_friendStatus;
        m_commentHistoryStatus = userScore->m_commentHistoryStatus;

        // Store social links
        m_links[0] = userScore->m_youtubeURL;
        m_links[1] = userScore->m_twitterURL;
        m_links[2] = userScore->m_twitchURL;
        m_links[3] = userScore->m_instagramURL;
        m_links[4] = userScore->m_tiktokURL;
        m_links[5] = userScore->m_discordUsername;
        m_links[6] = userScore->m_customString;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    // Main layer
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png",
        cocos2d::CCRect(0, 0, 80, 80)
    );
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(400, 300));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));

    // Button menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    // Message settings
    m_messageSettings = cocos2d::CCArray::create();
    m_messageSettings->retain();

    cocos2d::CCPoint basePos1 = cocos2d::CCPoint(centerX, centerY + 138.0f);
    cocos2d::CCSize lineSize = cocos2d::CCSize(400, 30);

    // Message setting line background
    cocos2d::extension::CCScale9Sprite* line1 = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png",
        cocos2d::CCRect(0, 0, 40, 40)
    );
    line1->setOpacity(50);
    line1->setContentSize(lineSize);
    line1->setScale(0.7f);
    m_mainLayer->addChild(line1, -1);
    line1->setPosition(basePos1 + cocos2d::CCPoint(0, -20));

    // "Messages" label
    cocos2d::CCLabelBMFont* msgLabel = cocos2d::CCLabelBMFont::create("Messages", "bigFont.fnt");
    m_mainLayer->addChild(msgLabel, 20);
    msgLabel->setPosition(basePos1);
    msgLabel->setScale(0.4f);

    // Message toggle buttons
    cocos2d::CCPoint msgPos = basePos1 + cocos2d::CCPoint(-105, -20);
    CCMenuItemToggler* allMsg = createToggleButton("All", menu_selector(GJAccountSettingsLayer::onMessageSetting), true, m_buttonMenu, msgPos, 50.0f, 0.4f);
    allMsg->setTag(0);
    allMsg->m_notClickable = true;

    msgPos = basePos1 + cocos2d::CCPoint(-30, -20);
    CCMenuItemToggler* friendsMsg = createToggleButton("Friends", menu_selector(GJAccountSettingsLayer::onMessageSetting), true, m_buttonMenu, msgPos, 50.0f, 0.4f);
    friendsMsg->setTag(1);
    friendsMsg->m_notClickable = true;

    msgPos = basePos1 + cocos2d::CCPoint(70, -20);
    CCMenuItemToggler* noneMsg = createToggleButton("None", menu_selector(GJAccountSettingsLayer::onMessageSetting), true, m_buttonMenu, msgPos, 50.0f, 0.4f);
    noneMsg->setTag(2);
    noneMsg->m_notClickable = true;

    m_messageSettings->addObject(allMsg);
    m_messageSettings->addObject(friendsMsg);
    m_messageSettings->addObject(noneMsg);

        // Friend request settings
    m_friendRequestSettings = cocos2d::CCArray::create();
    m_friendRequestSettings->retain();

    cocos2d::CCPoint basePos2 = basePos1 + cocos2d::CCPoint(0, -40);
    cocos2d::extension::CCScale9Sprite* line2 = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png",
        cocos2d::CCRect(0, 0, 40, 40)
    );
    line2->setOpacity(50);
    line2->setContentSize(lineSize);
    line2->setScale(0.7f);
    m_mainLayer->addChild(line2, -1);
    line2->setPosition(basePos2 + cocos2d::CCPoint(0, -20));

    cocos2d::CCLabelBMFont* friendLabel = cocos2d::CCLabelBMFont::create("Friend Requests", "bigFont.fnt");
    m_mainLayer->addChild(friendLabel, 20);
    friendLabel->setPosition(basePos2);
    friendLabel->setScale(0.4f);

    cocos2d::CCPoint friendPos = basePos2 + cocos2d::CCPoint(-70, -20);
    CCMenuItemToggler* allFriend = createToggleButton("All", menu_selector(GJAccountSettingsLayer::onFriendRequests), true, m_buttonMenu, friendPos, 50.0f, 0.4f);
    allFriend->setTag(0);
    allFriend->m_notClickable = true;

    friendPos = basePos2 + cocos2d::CCPoint(30, -20);
    CCMenuItemToggler* noneFriend = createToggleButton("None", menu_selector(GJAccountSettingsLayer::onFriendRequests), true, m_buttonMenu, friendPos, 50.0f, 0.4f);
    noneFriend->setTag(1);
    noneFriend->m_notClickable = true;

    m_friendRequestSettings->addObject(allFriend);
    m_friendRequestSettings->addObject(noneFriend);

    // Comment settings
    m_commentSettings = cocos2d::CCArray::create();
    m_commentSettings->retain();

    cocos2d::CCPoint basePos3 = basePos2 + cocos2d::CCPoint(0, -40);
    cocos2d::extension::CCScale9Sprite* line3 = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png",
        cocos2d::CCRect(0, 0, 40, 40)
    );
    line3->setOpacity(50);
    line3->setContentSize(lineSize);
    line3->setScale(0.7f);
    m_mainLayer->addChild(line3, -1);
    line3->setPosition(basePos3 + cocos2d::CCPoint(0, -20));

    cocos2d::CCLabelBMFont* commentLabel = cocos2d::CCLabelBMFont::create("Comments", "bigFont.fnt");
    m_mainLayer->addChild(commentLabel, 20);
    commentLabel->setPosition(basePos3);
    commentLabel->setScale(0.4f);

    cocos2d::CCPoint commentPos = basePos3 + cocos2d::CCPoint(-105, -20);
    CCMenuItemToggler* allComment = createToggleButton("All", menu_selector(GJAccountSettingsLayer::onCommentSetting), true, m_buttonMenu, commentPos, 50.0f, 0.4f);
    allComment->setTag(0);
    allComment->m_notClickable = true;

    commentPos = basePos3 + cocos2d::CCPoint(-30, -20);
    CCMenuItemToggler* friendsComment = createToggleButton("Friends", menu_selector(GJAccountSettingsLayer::onCommentSetting), true, m_buttonMenu, commentPos, 50.0f, 0.4f);
    friendsComment->setTag(1);
    friendsComment->m_notClickable = true;

    commentPos = basePos3 + cocos2d::CCPoint(70, -20);
    CCMenuItemToggler* meComment = createToggleButton("Me", menu_selector(GJAccountSettingsLayer::onCommentSetting), true, m_buttonMenu, commentPos, 50.0f, 0.4f);
    meComment->setTag(2);
    meComment->m_notClickable = true;

    m_commentSettings->addObject(allComment);
    m_commentSettings->addObject(friendsComment);
    m_commentSettings->addObject(meComment);

        // Social media input fields
    m_inputs = cocos2d::CCArray::create();
    m_inputs->retain();

    // Social media field data: {label, placeholder, key}
    struct SocialFieldData {
        const char* label;
        const char* placeholder;
        int key;
        int maxLength;
    };

    SocialFieldData fields[] = {
        {"YouTube", "YouTube URL", 0, 50},
        {"Twitter", "Twitter URL", 1, 50},
        {"Twitch", "Twitch URL", 2, 50},
        {"Instagram", "Instagram URL", 3, 50},
        {"TikTok", "TikTok URL", 4, 50},
        {"Discord", "Discord Username", 5, 32},
        {"Custom", "Custom String", 6, 50}
    };

    for (int i = 0; i < 7; i++)
    {
        float xOffset = (i & 1) ? 90.0f : -90.0f;
        float yOffset = (centerY - 5.0f) - (i / 2) * 39.0f;
        cocos2d::CCPoint pos = cocos2d::CCPoint(centerX + xOffset, yOffset);

        // Label
        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(fields[i].label, "bigFont.fnt");
        label->setScale(0.45f);
        label->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.0f));
        m_mainLayer->addChild(label);
        label->setPosition(pos + cocos2d::CCPoint(0, 12));

        // Info button
        std::string infoTitle = fields[i].label;
        std::string infoDesc = std::string("Enter your ") + fields[i].label + " information";
        InfoAlertButton* infoBtn = InfoAlertButton::create(infoTitle, infoDesc, 0.5f);
        m_buttonMenu->addChild(infoBtn);

        cocos2d::CCSize labelSize = label->getContentSize();
        float labelWidth = labelSize.width * label->getScale();
        cocos2d::CCPoint infoPos = label->getPosition() + cocos2d::CCPoint(labelWidth * 0.5f + 10.0f, 5.0f);
        infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(infoPos));

        // Background bar
        cocos2d::extension::CCScale9Sprite* bar = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png",
            cocos2d::CCRect(0, 0, 40, 40)
        );
        bar->setColor(cocos2d::ccc3(52, 85, 31));
        bar->setContentSize(cocos2d::CCSize(160, 20));
        m_mainLayer->addChild(bar, -1);
        bar->setPosition(pos + cocos2d::CCPoint(0, -1));

        // Text input
        CCTextInputNode* input = CCTextInputNode::create(160.0f, 20.0f, fields[i].placeholder, "Thonburi");
        input->m_delegate = this;
        input->setPosition(pos);
        input->setMaxLabelScale(0.5f);
        input->setMaxLabelWidth(140.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = fields[i].maxLength;
        input->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_'.");
        input->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 170));
        input->setLabelPlaceholderScale(0.45f);
        input->forceOffset();
        input->setTag(i);
        m_mainLayer->addChild(input, 20);

        // Set existing value
        auto it = m_links.find(fields[i].key);
        if (it != m_links.end() && !it->second.empty())
            input->setString(it->second);

        m_inputs->addObject(input);
    }
        // Cancel and Update buttons
    cocos2d::CCArray* buttons = cocos2d::CCArray::create();

    cocos2d::CCNode* cancelBtn = ButtonSprite::create("Cancel", 0, true, 0.5f, "bigFont.fnt", "GJ_button_05.png", 25.0f);
    CCMenuItemSpriteExtra* cancelItem = CCMenuItemSpriteExtra::create(cancelBtn, nullptr, this, menu_selector(GJAccountSettingsLayer::onClose));
    m_buttonMenu->addChild(cancelItem);
    buttons->addObject(cancelItem);

    cocos2d::CCNode* updateBtn = ButtonSprite::create("Update", 0, true, 0.5f, "bigFont.fnt", "GJ_button_01.png", 25.0f);
    CCMenuItemSpriteExtra* updateItem = CCMenuItemSpriteExtra::create(updateBtn, nullptr, this, menu_selector(GJAccountSettingsLayer::onUpdate));
    m_buttonMenu->addChild(updateItem);
    buttons->addObject(updateItem);

    cocos2d::CCPoint btnPos = cocos2d::CCPoint(centerX + 90.0f, centerY - 5.0f - 117.0f);
    cocos2d::CCPoint menuPos = m_buttonMenu->convertToNodeSpace(btnPos);
    GameToolbox::alignItemsHorisontally(buttons, 15.0f, menuPos);

    // Update toggle states
    onMessageSetting(nullptr);
    onFriendRequests(nullptr);
    onCommentSetting(nullptr);

    return true;
}

CCMenuItemToggler* GJAccountSettingsLayer::createToggleButton(const std::string& text, cocos2d::SEL_MenuHandler handler, bool enabled, cocos2d::CCMenu* menu, cocos2d::CCPoint pos, float width, float scale)
{
    cocos2d::CCSprite* offSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    cocos2d::CCSprite* onSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    CCMenuItemToggler* toggler = CCMenuItemToggler::create(offSprite, onSprite, this, handler);
    toggler->setSizeMult(0.7f);
    toggler->setPosition(menu->convertToNodeSpace(pos));
    menu->addChild(toggler);

    // Label
    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setScale(scale);
    label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    toggler->addChild(label);
    label->setPosition(cocos2d::CCPoint(18.0f, 0.0f));

    if (enabled)
        toggler->toggle(true);

    return toggler;
}

void GJAccountSettingsLayer::onMessageSetting(cocos2d::CCObject* sender)
{
    if (sender)
        m_messageStatus = sender->getTag();

    if (!m_messageSettings)
        return;

    for (int i = 0; i < m_messageSettings->count(); i++)
    {
        CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_messageSettings->objectAtIndex(i);
        bool enabled = toggler->getTag() == m_messageStatus;
        toggler->toggle(enabled);
    }
}

void GJAccountSettingsLayer::onFriendRequests(cocos2d::CCObject* sender)
{
    if (sender)
        m_friendStatus = sender->getTag();

    if (!m_friendRequestSettings)
        return;

    for (int i = 0; i < m_friendRequestSettings->count(); i++)
    {
        CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_friendRequestSettings->objectAtIndex(i);
        bool enabled = toggler->getTag() == m_friendStatus;
        toggler->toggle(enabled);
    }
}

void GJAccountSettingsLayer::onCommentSetting(cocos2d::CCObject* sender)
{
    if (sender)
        m_commentHistoryStatus = sender->getTag();

    if (!m_commentSettings)
        return;

    for (int i = 0; i < m_commentSettings->count(); i++)
    {
        CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_commentSettings->objectAtIndex(i);
        bool enabled = toggler->getTag() == m_commentHistoryStatus;
        toggler->toggle(enabled);
    }
}

void GJAccountSettingsLayer::textInputReturn(CCTextInputNode* node)
{
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPointZero));
}

void GJAccountSettingsLayer::textInputShouldOffset(CCTextInputNode* node, float yOffset)
{
    m_mainLayer->stopAllActions();
    m_mainLayer->runAction(cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0.0f, yOffset)));
}

void GJAccountSettingsLayer::closeAccInputNodes(cocos2d::CCArray* nodes)
{
    if (!nodes)
        return;

    for (int i = 0; i < nodes->count(); i++)
    {
        CCTextInputNode* node = (CCTextInputNode*)nodes->objectAtIndex(i);
        if (!node)
            continue;

        node->m_delegate = nullptr;
        node->onClickTrackNode(false);

        int tag = node->getTag();
        std::string value = node->getString();

        // Store in links map
        auto it = m_links.find(tag);
        if (it != m_links.end())
            it->second = value;
        else
            m_links[tag] = value;
    }
}

void GJAccountSettingsLayer::onUpdate(cocos2d::CCObject* sender)
{
    closeAccInputNodes(m_inputs);

    UpdateAccountSettingsPopup* popup = UpdateAccountSettingsPopup::create(
        this,
        m_messageStatus,
        m_friendStatus,
        m_commentHistoryStatus,
        m_links[0], // YouTube
        m_links[1], // Twitter
        m_links[2], // Twitch
        m_links[3], // Instagram
        m_links[4], // TikTok
        m_links[5], // Discord
        m_links[6]  // Custom
    );
    popup->show();
}

void GJAccountSettingsLayer::updateScoreValues()
{
    GameLevelManager* glm = GameLevelManager::sharedState();
    GJUserScore* userScore = glm->userInfoForAccountID(m_accountID);

    if (!userScore)
        return;

    // Update user score with current settings
    userScore->m_messageState = m_messageStatus;
    userScore->m_friendStatus = m_friendStatus;
    userScore->m_commentHistoryStatus = m_commentHistoryStatus;

    userScore->m_youtubeURL = m_links[0];
    userScore->m_twitterURL = m_links[1];
    userScore->m_twitchURL = m_links[2];
    userScore->m_instagramURL = m_links[3];
    userScore->m_tiktokURL = m_links[4];
    userScore->m_discordUsername = m_links[5];
    userScore->m_customString = m_links[6];
}

void GJAccountSettingsLayer::onClose(cocos2d::CCObject* sender)
{
    closeAccInputNodes(m_inputs);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJAccountSettingsLayer::keyBackClicked()
{
    onClose(nullptr);
}

GJAccountSettingsLayer::~GJAccountSettingsLayer()
{
    if (m_messageSettings)
        m_messageSettings->release();
    if (m_friendRequestSettings)
        m_friendRequestSettings->release();
    if (m_commentSettings)
        m_commentSettings->release();
    if (m_inputs)
        m_inputs->release();
}