#include "SelectSFXSortLayer.h"
#include "SelectSFXSortDelegate.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

SelectSFXSortLayer* SelectSFXSortLayer::create(AudioSortType sortType) {
    SelectSFXSortLayer* layer = new SelectSFXSortLayer();
    if (layer && layer->init(sortType)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SelectSFXSortLayer::init(AudioSortType sortType) {
    if (!cocos2d::CCLayerColor::initWithColor(ccc4(0, 0, 0, 150))) {
        return false;
    }

    FLAlertLayer::incrementForcePrio(this);

    m_sortType = sortType;

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

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Select Sorting", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f + 120.0f - 20.0f));
    m_mainLayer->addChild(title);
    title->setScale(0.8f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(SelectSFXSortLayer::onClose));
    m_buttonMenu->addChild(closeButton);
    closeButton->setScale(0.8f);
    closeButton->setSizeMult(1.6f);
    cocos2d::CCPoint closePos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(
            winSize.width * 0.5f - 180.0f + 10.0f,
            winSize.height * 0.5f + 120.0f - 10.0f));
    closeButton->setPosition(closePos);

    cocos2d::CCPoint menuOrigin = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 30.0f + 15.0f));

    for (int i = 0; i < 6; i++) {
        std::string label;
        switch (i) {
        case 0: label = "Name+"; break;
        case 1: label = "Name-"; break;
        case 2: label = "Len+"; break;
        case 3: label = "Len-"; break;
        case 4: label = "ID+"; break;
        case 5: label = "ID-"; break;
        }

        const char* bgFrame = (m_sortType == (AudioSortType)i) ? "GJ_button_02.png" : "GJ_button_05.png";
        ButtonSprite* btnSprite = ButtonSprite::create(
            label.c_str(), 0, 0, 0.5f, true, "bigFont.fnt", bgFrame, 30.0f);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            btnSprite, nullptr, menu_selector(SelectSFXSortLayer::onSelect));
        item->setTag(i);
        m_buttonMenu->addChild(item);

        float x = (float)(i & 1) * 100.0f - 100.0f + 50.0f;
        float y = floorf((float)(i >> 1)) * -60.0f;
        item->setPosition(menuOrigin + cocos2d::CCPoint(x, y));
    }

    m_joystickConnected = false;
    m_noElasticity = true;

    return true;
}

void SelectSFXSortLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectSFXSortLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SelectSFXSortLayer::onSelect(cocos2d::CCObject* sender) {
    m_sortType = (AudioSortType)sender->getTag();
    if (m_delegate) {
        m_delegate->sortSelectClosed(this);
    }
    onClose(nullptr);
}

SelectSFXSortLayer::~SelectSFXSortLayer() {
}