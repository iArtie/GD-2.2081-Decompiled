#include "ShareLevelLayer.h"
#include "GJGameLevel.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "TextArea.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "UploadPopup.h"
#include "ShareLevelSettingsLayer.h"

ShareLevelLayer* ShareLevelLayer::create(GJGameLevel* level) {
    ShareLevelLayer* returnVal = new ShareLevelLayer();
    if (returnVal->init(level)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool ShareLevelLayer::init(GJGameLevel* level) {
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    FLAlertLayer::incrementForcePrio(this);
    m_level = level;
    m_level->retain();
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

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(m_level->m_levelName.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setPosition(centerX, centerY + 150.0f - 20.0f);

    TextArea* warningText = TextArea::create(
        "<cy>You cannot change the name once it is shared.</c>",
        "chatFont.fnt", 1.0f, 400.0f, cocos2d::CCPoint(20.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(warningText);
    warningText->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    warningText->setScale(0.8f);

    cocos2d::CCPoint descPoint(centerX, centerY + 60.0f);
    cocos2d::extension::CCScale9Sprite* descBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    descBg->setContentSize(cocos2d::CCSize(380.0f, 50.0f));
    m_mainLayer->addChild(descBg, -1);
    descBg->setPosition(descPoint);
    descBg->setColor(cocos2d::ccc3(0, 0, 0));
    descBg->setOpacity(100);

    std::string descText = m_level->getUnpackedLevelDescription();
    if (descText.empty())
        descText = "(No description provided)";

    size_t pos = 0;
    while ((pos = descText.find("\\n", pos)) != std::string::npos) {
        descText.replace(pos, 2, "\n");
    }

    float scale;
    if (descText.length() <= 0xB4) {
        if (descText.length() > 0x46)
            scale = (float)((float)(180 - (int)descText.length()) / 180.0f) * 0.3f + 0.7f;
        else
            scale = 1.0f;
    } else {
        scale = 0.7f;
    }

    TextArea* descArea = TextArea::create(
        descText, "chatFont.fnt", 1.0f, 335.0f / scale,
        cocos2d::CCPoint(20.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(descArea);
    descArea->setPosition(descPoint + cocos2d::CCPoint(0.0f, 1.0f));
    descArea->setScale(scale);

    TextArea* legalText = TextArea::create(
        "By sharing a level you give the rights to use the level\n"
        "(in any way) to the creators of Geometry Dash.",
        "chatFont.fnt", 1.0f, 380.0f,
        cocos2d::CCPoint(20.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(legalText);
    legalText->setPosition(centerX, centerY - 150.0f + 65.0f);
    legalText->setScale(0.65f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    setupStars();

    cocos2d::CCArray* buttons = cocos2d::CCArray::create();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel");
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(
        cancelSprite, nullptr, menu_selector(ShareLevelLayer::onClose));
    m_buttonMenu->addChild(cancelBtn);
    buttons->addObject(cancelBtn);

    ButtonSprite* shareSprite = ButtonSprite::create("Share Level");
    CCMenuItemSpriteExtra* shareBtn = CCMenuItemSpriteExtra::create(
        shareSprite, nullptr, menu_selector(ShareLevelLayer::onShare));
    m_buttonMenu->addChild(shareBtn);
    buttons->addObject(shareBtn);

    cocos2d::CCPoint buttonPos(centerX, centerY - 150.0f + 25.0f);
    cocos2d::CCPoint menuPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &buttonPos);
    GameToolbox::alignItemsHorisontally(buttons, 15.0f, menuPos, false);

    cocos2d::CCSprite* settingsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
    settingsSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSprite, nullptr, menu_selector(ShareLevelLayer::onSettings));
    settingsBtn->setSizeMult(2.0f);
    m_buttonMenu->addChild(settingsBtn);
    settingsBtn->setPosition(cocos2d::CCPoint(centerX + 210.0f - 26.0f, centerY + 150.0f - 26.0f));

    GameManager* gameManager = GameManager::sharedState();
    if (!GameManager::getGameVariable(gameManager, "0029")) {
        cocos2d::CCSprite* settingsText = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsTxt_001.png");
        m_mainLayer->addChild(settingsText);
        cocos2d::CCPoint worldPos = cocos2d::CCNode::convertToWorldSpace(m_buttonMenu, settingsBtn->getPosition());
        settingsText->setPosition(worldPos + cocos2d::CCPoint(-55.0f, 0.0f));
    }

    return true;
}

void ShareLevelLayer::keyBackClicked() {
    onClose(nullptr);
}

void ShareLevelLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ShareLevelLayer::onSettings(cocos2d::CCObject* sender) {
    GameManager::sharedState()->setGameVariable("0029", true);
    ShareLevelSettingsLayer* settingsLayer = ShareLevelSettingsLayer::create(m_level);
    settingsLayer->show();
}

void ShareLevelLayer::onShare(cocos2d::CCObject* sender) {
    if (m_level->m_isUploaded) {
        FLAlertLayer::create(nullptr, "Uploaded", "This version has already been submitted")->show();
        return;
    }
    if (m_level->m_isVerified.random - m_level->m_isVerified.seed != 1 && !m_level->m_unlisted) {
        FLAlertLayer::create(
            nullptr,
            "Unverified",
            "You cannot share a level until you have <cg>completed</c> it.\n"
            "Complete the level in <cl>Normal</c> mode to verify that it can be beaten!\n"
            "A level cannot be verified if it has a <cy>Start Pos</c>.\n"
            "<cp>Unlisted</c> levels do not need to be verified.")->show();
        return;
    }
    UploadPopup* popup = UploadPopup::create(m_level);
    popup->show();
    onClose(nullptr);
}

void ShareLevelLayer::selectRating(cocos2d::CCObject* sender) {
    if (sender)
        m_starsRequested = sender->getTag();

    for (unsigned int i = 0; i < m_starButtons->count(); i++) {
        cocos2d::CCObject* obj = m_starButtons->objectAtIndex(i);
        ButtonSprite* button = (ButtonSprite*)obj;
        const char* bgImage = (button->getTag() == m_starsRequested) ? "GJ_button_01.png" : "GJ_button_04.png";
        button->updateBGImage(bgImage);
    }

    m_level->m_starsRequested = m_starsRequested;

    int difficultyIndex;
    if (m_starsRequested <= 1) {
        if (m_starsRequested == 1) {
            m_difficultySprite->setDisplayFrame(
                cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                    "difficulty_auto_btn_001.png"));
            return;
        }
        difficultyIndex = 0;
    } else if (m_starsRequested == 2) {
        difficultyIndex = 1;
    } else if (m_starsRequested == 3) {
        difficultyIndex = 2;
    } else if (m_starsRequested <= 5) {
        difficultyIndex = 3;
    } else if (m_starsRequested <= 7) {
        difficultyIndex = 4;
    } else if (m_starsRequested <= 9) {
        difficultyIndex = 5;
    } else if (m_starsRequested == 10) {
        difficultyIndex = 6;
    } else {
        difficultyIndex = 0;
    }

    std::string frameName = cocos2d::CCString::createWithFormat(
        "difficulty_%02d_btn_001.png", difficultyIndex)->getCString();
    m_difficultySprite->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str()));
}

void ShareLevelLayer::setupStars() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    m_starButtons = cocos2d::CCArray::create();
    m_starButtons->retain();

    cocos2d::CCArray* row1 = cocos2d::CCArray::create();
    cocos2d::CCArray* row2 = cocos2d::CCArray::create();

    for (int i = 1; i <= 10; i++) {
        CCMenuItemSpriteExtra* starButton = getStarsButton(i, menu_selector(ShareLevelLayer::selectRating), m_buttonMenu, 0.8f);
        m_starButtons->addObject(starButton);
        starButton->setTag(i);
        if (i <= 5)
            row1->addObject(starButton);
        else
            row2->addObject(starButton);
    }

    cocos2d::CCPoint basePoint(centerX, centerY - 50.0f + 35.0f + 2.0f);
    cocos2d::CCPoint worldPos = cocos2d::CCNode::convertToWorldSpace(m_mainLayer, &basePoint);
    cocos2d::CCPoint menuPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &worldPos);
    GameToolbox::alignItemsHorisontally(row1, 18.0f, menuPos, true);
    cocos2d::CCPoint row2Pos = menuPos + cocos2d::CCPoint(0.0f, -35.0f);
    GameToolbox::alignItemsHorisontally(row2, 18.0f, row2Pos, true);

    m_starsRequested = m_level->m_starsRequested;

    const char* labelText = GJGameLevel::isPlatformer(m_level) ? "Request Moons" : "Request Stars";
    cocos2d::CCLabelBMFont* requestLabel = cocos2d::CCLabelBMFont::create(labelText, "bigFont.fnt");
    requestLabel->setScale(0.5f);
    m_mainLayer->addChild(requestLabel, 2);
    requestLabel->setPosition(basePoint + cocos2d::CCPoint(0.0f, 30.0f));

    m_difficultySprite = cocos2d::CCSprite::createWithSpriteFrameName("difficulty_00_btn_001.png");
    m_mainLayer->addChild(m_difficultySprite, 3);
    m_difficultySprite->setPosition(basePoint + cocos2d::CCPoint(155.0f, -18.0f));

    selectRating(nullptr);
}

ShareLevelLayer::~ShareLevelLayer() {
}