#include "Popups/SetLevelOrderPopup.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

SetLevelOrderPopup* SetLevelOrderPopup::create(int levelID, int order, int amount) {
    SetLevelOrderPopup* popup = new SetLevelOrderPopup();
    if (popup && popup->init(levelID, order, amount)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetLevelOrderPopup::init(int levelID, int order, int amount) {
    if (!SetIDPopup::init(order, 1, amount, "Level Order", "OK", false, false, 0.0f, true, true)) {
        return false;
    }

    m_levelID = levelID;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 5.0f);
    cocos2d::CCPoint menuOrigin = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, center);

    cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn2_001.png");
    leftSprite->setColor(ccc3(255, 255, 255));
    leftSprite->setScale(1.1f);
    leftSprite->setRotation(90.0f);
    CCMenuItemSpriteExtra* leftButton = CCMenuItemSpriteExtra::create(
        leftSprite, nullptr, menu_selector(SetLevelOrderPopup::onOrderButton));
    m_buttonMenu->addChild(leftButton);
    leftButton->setTag(0);
    leftButton->setPosition(menuOrigin + cocos2d::CCPoint(-85.0f, 0.0f));
    leftButton->setSizeMult(1.4f);

    cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn2_001.png");
    rightSprite->setColor(ccc3(255, 255, 255));
    rightSprite->setScale(1.1f);
    rightSprite->setRotation(90.0f);
    CCMenuItemSpriteExtra* rightButton = CCMenuItemSpriteExtra::create(
        rightSprite, nullptr, menu_selector(SetLevelOrderPopup::onOrderButton));
    m_buttonMenu->addChild(rightButton);
    rightButton->setTag(1);
    rightButton->setPosition(menuOrigin + cocos2d::CCPoint(85.0f, 0.0f));
    rightButton->setSizeMult(1.4f);

    return true;
}

void SetLevelOrderPopup::onOrderButton(cocos2d::CCObject* sender) {
    if (sender->getTag() == 1) {
        m_value = m_maximum;
    } else {
        m_value = 1;
    }
    SetIDPopup::updateTextInputLabel();
}

SetLevelOrderPopup::~SetLevelOrderPopup() {
}