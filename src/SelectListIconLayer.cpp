#include "SelectListIconLayer.h"
#include "SelectListIconDelegate.h"
#include "GJLevelList.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

SelectListIconLayer* SelectListIconLayer::create(int difficulty) {
    SelectListIconLayer* layer = new SelectListIconLayer();
    if (layer && layer->init(difficulty)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SelectListIconLayer::init(int difficulty) {
    if (!cocos2d::CCLayerColor::initWithColor(ccc4(0, 0, 0, 150))) {
        return false;
    }

    FLAlertLayer::incrementForcePrio();

    m_currentDifficulty = difficulty;

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360.0f, 240.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Select Difficulty", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f + 120.0f - 20.0f));
    m_mainLayer->addChild(title);
    title->setScale(0.8f);

    m_difficulties = cocos2d::CCArray::create();
    m_difficulties->retain();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCPoint menuOrigin = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 30.0f + 5.0f));

    for (int i = 0; i < 12; i++) {
        const char* frameName = GJLevelList::frameForListDifficulty(i, DifficultyIconType::DefaultText);
        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        m_difficulties->addObject(sprite);
        sprite->setScale(1.0f);

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            sprite, nullptr, menu_selector(SelectListIconLayer::onSelect));
        sprite->setTag(i);
        item->setTag(i);
        m_buttonMenu->addChild(item);

        float x = (float)(i % 6) * 52.0f - 156.0f + 26.0f;
        float y = floorf((float)(i / 6)) * -60.0f;
        item->setPosition(menuOrigin + cocos2d::CCPoint(x, y));
    }

    m_joystickConnected = false;

    ButtonSprite* okSprite = ButtonSprite::create(
        "OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okButton = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, menu_selector(SelectListIconLayer::onClose));
    m_buttonMenu->addChild(okButton);
    cocos2d::CCPoint okPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 120.0f + 25.0f));
    okButton->setPosition(okPos);

    onSelect(nullptr);

    m_noElasticity = true;

    return true;
}

void SelectListIconLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectListIconLayer::onClose(cocos2d::CCObject* sender) {
    if (m_delegate) {
        m_delegate->iconSelectClosed(this);
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SelectListIconLayer::onSelect(cocos2d::CCObject* sender) {
    if (sender) {
        m_currentDifficulty = sender->getTag();
    }

    for (unsigned int i = 0; i < m_difficulties->count(); i++) {
        cocos2d::CCSprite* sprite = (cocos2d::CCSprite*)m_difficulties->objectAtIndex(i);
        if (m_currentDifficulty == sprite->getTag()) {
            sprite->setColor(ccc3(255, 255, 255));
        } else {
            sprite->setColor(ccc3(125, 125, 125));
        }
    }
}

SelectListIconLayer::~SelectListIconLayer() {
}