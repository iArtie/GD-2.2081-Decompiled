#include "SelectSettingLayer.h"
#include "SelectSettingDelegate.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

SelectSettingLayer* SelectSettingLayer::create(SelectSettingType type, int value) {
    SelectSettingLayer* layer = new SelectSettingLayer();
    if (layer && layer->init(type, value)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

std::string SelectSettingLayer::frameForItem(SelectSettingType type, int index) {
    if (type <= SelectSettingType::StartingModePlatformer) {
        switch (index) {
        case 0: return "gj_iconBtn_off_001.png";
        case 1: return "gj_shipBtn_off_001.png";
        case 2: return "gj_ballBtn_off_001.png";
        case 3: return "gj_birdBtn_off_001.png";
        case 4: return "gj_dartBtn_off_001.png";
        case 5: return "gj_robotBtn_off_001.png";
        case 6: return "gj_spiderBtn_off_001.png";
        case 7: return "gj_swingBtn_off_001.png";
        default: return "";
        }
    } else if (type == SelectSettingType::StartingSpeed) {
        switch (index) {
        case 0: return "boost_01_001.png";
        case 2: return "boost_03_001.png";
        case 3: return "boost_04_001.png";
        case 4: return "boost_05_001.png";
        default: return "boost_02_001.png";
        }
    }
    return "";
}

std::string SelectSettingLayer::frameForValue(SelectSettingType type, int value) {
    int idx = valueToIdx(type, value);
    return frameForItem(type, idx);
}

std::string SelectSettingLayer::getSelectedFrame() {
    return frameForItem(m_type, m_settingID);
}

int SelectSettingLayer::getSelectedValue() {
    return idxToValue(m_type, m_settingID);
}

int SelectSettingLayer::idxToValue(SelectSettingType type, int idx) {
    static const int speedValues[] = { 0, 1, 2, 3, 4 };
    if (type == SelectSettingType::StartingSpeed) {
        if ((unsigned int)idx <= 4) {
            return speedValues[idx];
        }
        return 0;
    }
    return idx;
}

int SelectSettingLayer::valueToIdx(SelectSettingType type, int value) {
    static const int speedIdx[] = { 1, 2, 3, 4, 5 };
    if (type == SelectSettingType::StartingSpeed) {
        if ((unsigned int)value <= 4) {
            return speedIdx[value];
        }
        return 1;
    }
    return value;
}

bool SelectSettingLayer::init(SelectSettingType type, int value) {
    if (!cocos2d::CCLayerColor::initWithColor(ccc4(0, 0, 0, 150))) {
        return false;
    }

    FLAlertLayer::incrementForcePrio(this);

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_type = type;
    m_settingID = valueToIdx(type, value);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint menuOrigin = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 5.0f));

    float spacing;
    int cols;
    float iconScale;
    if (m_type <= SelectSettingType::StartingModePlatformer) {
        spacing = 42.0f;
        cols = 8;
        iconScale = 0.9f;
    } else if (m_type == SelectSettingType::StartingSpeed) {
        spacing = 60.0f;
        cols = 5;
        iconScale = 0.8f;
    } else {
        return false;
    }

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360.0f, 160.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f + 80.0f - 20.0f));
    m_mainLayer->addChild(title);
    title->setScale(0.9f);

    m_settingSprites = cocos2d::CCArray::create();
    m_settingSprites->retain();

    for (int i = 0; i < cols; i++) {
        std::string frameName = frameForItem(m_type, i);
        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());
        m_settingSprites->addObject(sprite);
        sprite->setScale(iconScale);

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            sprite, nullptr, menu_selector(SelectSettingLayer::onSelect));

        if (type == SelectSettingType::StartingModePlatformer && (i == 7 || i == 4)) {
            item->setEnabled(false);
            sprite->setOpacity(125);
        }

        item->setTag(i);
        m_buttonMenu->addChild(item);

        float x = (float)(i % cols) * spacing - (float)(cols * 0.5f) * spacing + spacing * 0.5f;
        float y = floorf((float)(i / cols)) * -35.0f;
        item->setPosition(menuOrigin + cocos2d::CCPoint(x, y));
    }

    m_joystickConnected = false;

    ButtonSprite* okSprite = ButtonSprite::create(
        "OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okButton = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, menu_selector(SelectSettingLayer::onClose));
    m_buttonMenu->addChild(okButton);
    cocos2d::CCPoint okPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 80.0f + 25.0f));
    okButton->setPosition(okPos);

    onSelect(nullptr);

    m_noElasticity = true;

    return true;
}

void SelectSettingLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectSettingLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
    if (m_delegate) {
        m_delegate->selectSettingClosed(this);
    }
}

void SelectSettingLayer::onSelect(cocos2d::CCObject* sender) {
    if (sender) {
        m_settingID = sender->getTag();
    }

    for (unsigned int i = 0; i < m_settingSprites->count(); i++) {
        cocos2d::CCSprite* sprite = (cocos2d::CCSprite*)m_settingSprites->objectAtIndex(i);
        if (m_settingID == (int)i) {
            sprite->setColor(ccc3(255, 255, 255));
        } else {
            sprite->setColor(ccc3(125, 125, 125));
        }
    }
}

SelectSettingLayer::~SelectSettingLayer() {
}