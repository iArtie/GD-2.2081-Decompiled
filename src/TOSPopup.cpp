#include "TOSPopup.h"
#include "GameManager.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "TextArea.h"

TOSPopup* TOSPopup::create() {
    TOSPopup* returnVal = new TOSPopup();
    if (returnVal->init()) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool TOSPopup::init() {
    if (!FLAlertLayer::init(150))
        return false;

    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(350.0f, 200.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -2);

    cocos2d::extension::CCScale9Sprite* innerBg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    innerBg->setColor(cocos2d::ccc3(50, 50, 50));
    innerBg->setOpacity(60);
    innerBg->setContentSize(cocos2d::CCSize(320.0f, 100.0f));
    m_mainLayer->addChild(innerBg, -1);
    innerBg->setPosition(centerX, centerY + 6.0f);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Terms of Service", "bigFont.fnt");
    titleLabel->setPosition(centerX, centerY + 100.0f - 16.0f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.8f);

    TextArea* textArea = TextArea::create(
        "By tapping \"OK\", you accept the\n"
        "<cl>Terms of Use</c> and confirm that\n"
        "you have read the <cl>Privacy Policy</c>.",
        "bigFont.fnt", 1.0f, 1000.0f, cocos2d::CCPoint(42.0f, 0.0f), 20.0f, false);
    m_mainLayer->addChild(textArea, 10);
    textArea->setPosition(centerX, centerY + 15.0f);
    textArea->setScale(0.5f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCArray* buttons = cocos2d::CCArray::create();

    CCMenuItemSpriteExtra* okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK"), nullptr, menu_selector(TOSPopup::onClose));
    m_buttonMenu->addChild(okBtn);
    buttons->addObject(okBtn);

    m_buttonMenu->setPosition(centerX, centerY - 100.0f + 25.0f);

    cocos2d::CCSprite* tosHitbox = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    tosHitbox->setOpacity(0);
    tosHitbox->setScaleX(4.0f);
    tosHitbox->setScaleY(1.2f);
    CCMenuItemSpriteExtra* tosBtn = CCMenuItemSpriteExtra::create(
        tosHitbox, nullptr, menu_selector(TOSPopup::onTOS));
    m_buttonMenu->addChild(tosBtn);
    tosBtn->m_animationEnabled = false;
    tosBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX - 80.0f, centerY + 6.0f)));

    cocos2d::CCSprite* privacyHitbox = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    privacyHitbox->setOpacity(0);
    privacyHitbox->setScaleX(4.0f);
    privacyHitbox->setScaleY(1.2f);
    CCMenuItemSpriteExtra* privacyBtn = CCMenuItemSpriteExtra::create(
        privacyHitbox, nullptr, menu_selector(TOSPopup::onPrivacy));
    m_buttonMenu->addChild(privacyBtn);
    privacyBtn->m_animationEnabled = false;
    privacyBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX + 80.0f, centerY - 14.0f)));

    return true;
}

void TOSPopup::keyBackClicked() {
}

void TOSPopup::onClose(cocos2d::CCObject* sender) {
    GameManager::sharedState()->setUGV("30", true);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void TOSPopup::onPrivacy(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.robtopgames.com/privacy");
}

void TOSPopup::onTOS(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.robtopgames.com/tos");
}

TOSPopup::~TOSPopup() {
}