#include "SelectPremadeLayer.h"
#include "SelectPremadeDelegate.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

SelectPremadeLayer* SelectPremadeLayer::create() {
    SelectPremadeLayer* layer = new SelectPremadeLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SelectPremadeLayer::init() {
    if (!FLAlertLayer::init(150)) {
        return false;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360.0f, 260.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Select Premade", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f + 130.0f - 20.0f));
    m_mainLayer->addChild(title);
    title->setScale(0.9f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCPoint menuOrigin = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 22.5f + 50.0f));

    for (int i = 0; i < 2; i++) {
        const char* labelText = (i == 0) ? "Homing\nMissile" : "Homing\nBall";

        ButtonSprite* btnSprite = ButtonSprite::create(
            labelText, 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_04.png", 30.0f);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            btnSprite, nullptr, menu_selector(SelectPremadeLayer::onSelectPremade));
        m_buttonMenu->addChild(item);
        item->setTag(i);

        float x = (float)i * 45.0f - 90.0f + 22.5f;
        item->setPosition(menuOrigin + cocos2d::CCPoint(x, 0.0f));
    }

    ButtonSprite* cancelSprite = ButtonSprite::create(
        "Cancel", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* cancelButton = CCMenuItemSpriteExtra::create(
        cancelSprite, nullptr, menu_selector(SelectPremadeLayer::onClose));
    m_buttonMenu->addChild(cancelButton);
    cocos2d::CCPoint cancelPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 130.0f + 25.0f));
    cancelButton->setPosition(cancelPos);

    return true;
}

void SelectPremadeLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectPremadeLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SelectPremadeLayer::onSelectPremade(cocos2d::CCObject* sender) {
    if (m_delegate) {
        m_delegate->selectPremadeClosed(this, sender->getTag());
    }
    onClose(nullptr);
}

SelectPremadeLayer::~SelectPremadeLayer() {
}