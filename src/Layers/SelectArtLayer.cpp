#include "Layers/SelectArtLayer.h"
#include "Delegates/SelectArtDelegate.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Editor/ObjectToolbox.h"
#include "Editor/EditorUI.h"

SelectArtLayer* SelectArtLayer::create(SelectArtType type, int index) {
    SelectArtLayer* layer = new SelectArtLayer();
    if (layer && layer->init(type, index)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SelectArtLayer::init(SelectArtType type, int index) {
    if (!cocos2d::CCLayerColor::initWithColor(ccc4(0, 0, 0, 150))) {
        return false;
    }

    FLAlertLayer::incrementForcePrio();

    m_type = type;
    if (index != -1) {
        m_art = index;
    }

    int maxArt;
    if (type == SelectArtType::Background) {
        type = SelectArtType::Ground;
        if (m_art <= 0) {
            m_art = 1;
        }
        maxArt = 59;
    } else if (type == SelectArtType::Ground) {
        if (m_art <= 0) {
            m_art = 1;
        }
        maxArt = 22;
    } else {
        bool isMiddle = (type == SelectArtType::Middleground);
        type = SelectArtType::Background;
        if (isMiddle) {
            maxArt = 3;
        } else {
            maxArt = 11;
        }
    }

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    float layerHeight;
    if (m_type == SelectArtType::Background) {
        layerHeight = 300.0f;
    } else if (m_type == SelectArtType::Ground) {
        layerHeight = 260.0f;
    } else {
        layerHeight = 240.0f;
    }

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360.0f, layerHeight));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    title->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        layerHeight * 0.5f + winSize.height * 0.5f - 20.0f));
    m_mainLayer->addChild(title);
    title->setScale(0.8f);

    float iconScale;
    if (m_type == SelectArtType::Background) {
        iconScale = 0.6f;
    } else if (m_type == SelectArtType::Ground) {
        iconScale = 0.8f;
    } else {
        iconScale = 0.7f;
    }

    m_artSprites = cocos2d::CCArray::create();
    m_artSprites->retain();

    cocos2d::CCArray* tempArray = cocos2d::CCArray::create();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    float spacingX, spacingY, startX, startY, stepY;
    int cols, startIdx;
    if (m_type == SelectArtType::Background) {
        spacingX = 35.0f;
        spacingY = -35.0f;
        startX = 17.5f;
        startY = 75.0f;
        stepY = 40.0f;
        cols = 10;
        startIdx = -10;
    } else if (m_type == SelectArtType::Ground) {
        spacingX = 40.0f;
        spacingY = -40.0f;
        startX = 20.0f;
        startY = 40.0f;
        stepY = 40.0f;
        cols = 8;
        startIdx = -8;
    } else {
        spacingX = 45.0f;
        spacingY = -45.0f;
        startX = 22.5f;
        startY = 25.0f;
        stepY = 45.0f;
        cols = 7;
        startIdx = -7;
    }

    cocos2d::CCPoint origin = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(
            winSize.width * 0.5f,
            startY + winSize.height * 0.5f + startY));

    SelectArtType currentIdx = type;
    while (currentIdx <= maxArt) {
        std::string frameName;
        if (m_type == SelectArtType::Background) {
            frameName = cocos2d::CCString::createWithFormat(
                "bgIcon_%02d_001.png", (int)currentIdx)->getCString();
        } else if (m_type == SelectArtType::Ground) {
            frameName = cocos2d::CCString::createWithFormat(
                "gIcon_%02d_001.png", (int)currentIdx)->getCString();
        } else if (m_type == SelectArtType::Middleground) {
            frameName = cocos2d::CCString::createWithFormat(
                "mgIcon_%02d_001.png", (int)currentIdx)->getCString();
        } else {
            int key = EditorUI::smartTypeForKey((int)currentIdx);
            frameName = ObjectToolbox::intKeyToFrame(ObjectToolbox::sharedState(), key);
        }

        cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());
        m_artSprites->addObject(sprite);
        sprite->setScale(iconScale);

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            sprite, nullptr, menu_selector(SelectArtLayer::selectArt));
        item->setTag((int)currentIdx);
        m_buttonMenu->addChild(item);
        tempArray->addObject(item);

        int col = (currentIdx - type) % cols;
        int row = (currentIdx - type) / cols;
        float x = spacingX * col + spacingX * (startIdx * 0.5f) + startX;
        float y = floorf((float)row) * spacingY;
        item->setPosition(origin + cocos2d::CCPoint(x, y));

        currentIdx = (SelectArtType)((int)currentIdx + 1);
    }

    m_joystickConnected = false;
    tempArray->removeAllObjects();

    ButtonSprite* okButton = ButtonSprite::create(
        "OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SelectArtLayer::onClose));
    m_buttonMenu->addChild(okItem);
    okItem->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(
            winSize.width * 0.5f,
            25.0f - (layerHeight * 0.5f - winSize.height * 0.5f))));

    selectArt(nullptr);

    if (m_type == SelectArtType::Ground) {
        m_lineSprites = cocos2d::CCArray::create();
        m_lineSprites->retain();

        cocos2d::CCLabelBMFont* lineLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        lineLabel->setScale(0.7f);
        m_mainLayer->addChild(lineLabel);
        lineLabel->setPosition(cocos2d::CCPoint(
            winSize.width * 0.5f + 130.0f,
            winSize.height * 0.5f - 45.0f));

        ButtonSprite* line1 = ButtonSprite::create(
            "1", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 20.0f);
        CCMenuItemSpriteExtra* line1Item = CCMenuItemSpriteExtra::create(
            line1, nullptr, menu_selector(SelectArtLayer::onSelectCustom));
        line1Item->setTag(1);
        m_buttonMenu->addChild(line1Item);
        m_lineSprites->addObject(line1);
        line1Item->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
            lineLabel->getPosition() + cocos2d::CCPoint(-23.0f, -25.0f)));

        ButtonSprite* line2 = ButtonSprite::create(
            "2", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 20.0f);
        CCMenuItemSpriteExtra* line2Item = CCMenuItemSpriteExtra::create(
            line2, nullptr, menu_selector(SelectArtLayer::onSelectCustom));
        line2Item->setTag(2);
        m_buttonMenu->addChild(line2Item);
        m_lineSprites->addObject(line2);
        line2Item->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
            lineLabel->getPosition() + cocos2d::CCPoint(23.0f, -25.0f)));

        ButtonSprite* line3 = ButtonSprite::create(
            "3", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 20.0f);
        CCMenuItemSpriteExtra* line3Item = CCMenuItemSpriteExtra::create(
            line3, nullptr, menu_selector(SelectArtLayer::onSelectCustom));
        line3Item->setTag(3);
        m_buttonMenu->addChild(line3Item);
        m_lineSprites->addObject(line3);
        line3Item->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
            lineLabel->getPosition() + cocos2d::CCPoint(-23.0f, -50.0f)));
    }

    m_noElasticity = true;
    return true;
}

void SelectArtLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectArtLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
    if (m_delegate) {
        m_delegate->selectArtClosed(this);
    }
}

void SelectArtLayer::onSelectCustom(cocos2d::CCObject* sender) {
    if (sender) {
        m_line = sender->getTag();
    } else if (m_line <= 0) {
        m_line = 1;
    }
    if (m_line > 3) {
        m_line = 3;
    }

    for (unsigned int i = 0; i < m_lineSprites->count(); i++) {
        ButtonSprite* sprite = (ButtonSprite*)m_lineSprites->objectAtIndex(i);
        if (m_line == (int)i) {
            sprite->updateBGImage("GJ_button_02.png");
        } else {
            sprite->updateBGImage("GJ_button_04.png");
        }
    }
}

void SelectArtLayer::selectArt(cocos2d::CCObject* sender) {
    if (sender) {
        m_art = sender->getTag();
    }

    bool isGroundType = (m_type <= SelectArtType::Ground);
    for (unsigned int i = 0; i < m_artSprites->count(); i++) {
        cocos2d::CCObject* sprite = m_artSprites->objectAtIndex(i);
        if ((int)i + (isGroundType ? 1 : 0) == m_art) {
            ((cocos2d::CCSprite*)sprite)->setColor(ccc3(255, 255, 255));
        } else {
            ((cocos2d::CCSprite*)sprite)->setColor(ccc3(125, 125, 125));
        }
    }
}

void SelectArtLayer::updateSelectedCustom(int idx) {
    if (m_lineSprites) {
        m_line = idx;
        onSelectCustom(nullptr);
    }
}

SelectArtLayer::~SelectArtLayer() {
}