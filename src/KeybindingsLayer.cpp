#include "KeybindingsLayer.h"
#include "FLAlertLayer.h"
#include "GameManager.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"

KeybindingsLayer* KeybindingsLayer::create() {
    auto layer = new KeybindingsLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool KeybindingsLayer::init() {
    if (!FLAlertLayer::init(75)) {
        return false;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(420.0f, 280.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    auto titleLabel = cocos2d::CCLabelBMFont::create("Keybindings", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 124.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.8f);
    
    auto subLabel = cocos2d::CCLabelBMFont::create("(Keyboard shortcuts)", "bigFont.fnt");
    subLabel->setPosition(titleLabel->getPosition() + cocos2d::CCPoint(0.0f, -20.0f));
    m_mainLayer->addChild(subLabel);
    subLabel->setScale(0.6f);
    
    m_variables = cocos2d::CCDictionary::create();
    m_variables->retain();
    
    m_values = cocos2d::CCDictionary::create();
    m_values->retain();
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_keyCount = 0;
    m_maxPage = 0;
    m_page = -1;
    
    auto header = addKeyPair("Gameplay Keys", " ");
    header->setColor(cocos2d::ccc3(200, 200, 200));
    addKeyPair("Jump", "Space / Up");
    addKeyPair("Pause", "Escape");
    addKeyPair("Place Checkpoint", "Z");
    addKeyPair("Delete Checkpoint", "X");
    m_keyCount = 7;
    
    auto editorHeader = addKeyPair("Editor Keys", " ");
    editorHeader->setColor(cocos2d::ccc3(200, 200, 200));
    addKeyPair("Build Mode", "1");
    addKeyPair("Edit Mode", "2");
    addKeyPair("Delete Mode", "3");
    addKeyPair("Swipe Modifier", "Shift");
    addKeyPair("Move Modifier", "Space");
    addKeyPair("Rotate CCW", "Q");
    addKeyPair("Rotate CW", "E");
    addKeyPair("Flip X", "Alt + Q");
    addKeyPair("Flip Y", "Alt + E");
    addKeyPair("Delete selected", "Delete");
    addKeyPair("Undo", "Ctrl + Z");
    addKeyPair("Redo", "Ctrl + Shift + Z");
    addKeyPair("Deselect", "Alt + D");
    addKeyPair("Copy", "Ctrl + C");
    addKeyPair("Paste", "Ctrl + V");
    addKeyPair("Duplicate", "Ctrl + D");
    addKeyPair("Rotate", "R");
    addKeyPair("Free Move", "F");
    addKeyPair("Swipe", "T");
    addKeyPair("Snap", "G");
    addKeyPair("Playtest", "Enter");
    addKeyPair("Playback music", "Ctrl + Enter");
    addKeyPair("Next Group", "Arrow Right");
    addKeyPair("Previous Group", "Arrow Left");
    addKeyPair("Object Left", "A");
    addKeyPair("Object Right", "D");
    addKeyPair("Object Up", "W");
    addKeyPair("Object Down", "S");
    addKeyPair("Object Left small", "Shift + A");
    addKeyPair("Object Right small", "Shift + D");
    addKeyPair("Object Up small", "Shift + W");
    addKeyPair("Object Down small", "Shift + S");
    addKeyPair("Lock Preview", "F1");
    addKeyPair("Unlock Preview", "F2");
    addKeyPair("Preview Mode On/Off", "F3");
    addKeyPair("Particle Icons On/Off", "F4");
    addKeyPair("Debug Draw On/Off", "F5");
    addKeyPair("Hide Invisible On/Off", "F6");
    addKeyPair("Save Editor Pos", "Ctrl + 0-9");
    addKeyPair("Load Editor Pos", "Alt + 0-9");
    addKeyPair("Zoom", "Ctrl + ScrollWheel");
    addKeyPair("Transform", "Ctrl + T");
    
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    auto closeItem = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(KeybindingsLayer::onClose));
    closeItem->setSizeMult(1.5f);
    m_buttonMenu->addChild(closeItem);
    closeItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX - 205.0f, centerY + 135.0f)));
    
    auto prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(KeybindingsLayer::onPrevPage));
    m_buttonMenu->addChild(m_prevButton);
    m_prevButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(director->getScreenLeft() + 24.0f, centerY)));
    
    auto nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(KeybindingsLayer::onNextPage));
    m_buttonMenu->addChild(m_nextButton);
    m_nextButton->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(director->getScreenRight() - 24.0f, centerY)));
    
    goToPage(0);
    
    return true;
}

cocos2d::CCLabelBMFont* KeybindingsLayer::addKeyPair(const char* action, const char* key) {
    int page = m_keyCount / 7;
    auto layer = layerForPage(page);
    cocos2d::CCPoint pos = nextPosition(page);
    
    m_keyCount++;
    if (page > m_maxPage) {
        m_maxPage = page;
    }
    
    auto bg = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, (m_keyCount & 1) ? 50 : 25));
    layer->addChild(bg, -1);
    bg->setPosition(pos + cocos2d::CCPoint(-180.0f, -15.0f));
    bg->setContentSize(cocos2d::CCSize(360.0f, 30.0f));
    
    auto actionLabel = cocos2d::CCLabelBMFont::create(action, "bigFont.fnt");
    actionLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    layer->addChild(actionLabel);
    actionLabel->setPosition(pos + cocos2d::CCPoint(-170.0f, 0.0f));
    actionLabel->limitLabelWidth(260.0f, 0.5f, 0.0f);
    
    auto keyLabel = cocos2d::CCLabelBMFont::create(key, "bigFont.fnt");
    keyLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    layer->addChild(keyLabel);
    keyLabel->setPosition(pos + cocos2d::CCPoint(170.0f, 0.0f));
    keyLabel->limitLabelWidth(100.0f, 0.6f, 0.0f);
    
    incrementCountForPage(page);
    
    return actionLabel;
}

int KeybindingsLayer::countForPage(int page) {
    auto key = pageKey(page);
    auto str = static_cast<cocos2d::CCString*>(m_values->objectForKey(key));
    return str ? str->intValue() : 0;
}

void KeybindingsLayer::incrementCountForPage(int page) {
    int count = countForPage(page) + 1;
    auto str = cocos2d::CCString::createWithFormat("%i", count);
    m_values->setObject(str, pageKey(page));
}

void KeybindingsLayer::goToPage(int page) {
    if (page > m_maxPage) {
        page = 0;
    } else if (page < 0) {
        page = m_maxPage;
    }
    
    if (m_page == page) {
        return;
    }
    
    auto oldKey = objectKey(m_page);
    auto oldObjects = static_cast<cocos2d::CCArray*>(m_values->objectForKey(oldKey));
    
    if (oldObjects) {
        for (int i = 0; i < oldObjects->count(); ++i) {
            auto obj = oldObjects->objectAtIndex(i);
            obj->setVisible(false);
        }
    }
    
    auto oldLayerKey = layerKey(m_page);
    auto oldLayer = m_values->objectForKey(oldLayerKey);
    if (oldLayer) {
        oldLayer->setVisible(false);
    }
    
    auto newKey = objectKey(page);
    auto newObjects = static_cast<cocos2d::CCArray*>(m_values->objectForKey(newKey));
    
    if (newObjects) {
        for (int i = 0; i < newObjects->count(); ++i) {
            auto obj = newObjects->objectAtIndex(i);
            obj->setVisible(true);
        }
    }
    
    auto newLayerKey = layerKey(page);
    auto newLayer = m_values->objectForKey(newLayerKey);
    if (newLayer) {
        newLayer->setVisible(true);
    }
    
    m_page = page;
    m_prevButton->setVisible(page > 0);
    m_nextButton->setVisible(page < m_maxPage);
}

cocos2d::CCLayer* KeybindingsLayer::layerForPage(int page) {
    auto key = layerKey(page);
    auto layer = static_cast<cocos2d::CCLayer*>(m_values->objectForKey(key));
    
    if (!layer) {
        layer = cocos2d::CCLayer::create();
        m_mainLayer->addChild(layer);
        m_values->setObject(layer, key);
        layer->setVisible(false);
    }
    
    return layer;
}

cocos2d::CCArray* KeybindingsLayer::objectsForPage(int page) {
    auto key = objectKey(page);
    auto array = static_cast<cocos2d::CCArray*>(m_values->objectForKey(key));
    
    if (!array) {
        array = cocos2d::CCArray::create();
        m_values->setObject(array, key);
    }
    
    return array;
}

cocos2d::CCPoint KeybindingsLayer::nextPosition(int page) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    cocos2d::CCPoint pos(centerX, centerY + 100.0f);
    pos.y -= 30.0f * (countForPage(page) + 1);
    return pos;
}

const char* KeybindingsLayer::pageKey(int page) {
    return cocos2d::CCString::createWithFormat("page_%i", page)->getCString();
}

const char* KeybindingsLayer::layerKey(int page) {
    return cocos2d::CCString::createWithFormat("layer_%i", page)->getCString();
}

const char* KeybindingsLayer::objectKey(int page) {
    return cocos2d::CCString::createWithFormat("object_%i", page)->getCString();
}

const char* KeybindingsLayer::infoKey(int index) {
    return cocos2d::CCString::createWithFormat("info_%i", index)->getCString();
}

void KeybindingsLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void KeybindingsLayer::keyBackClicked() {
    onClose(nullptr);
}

void KeybindingsLayer::onNextPage(cocos2d::CCObject* sender) {
    goToPage(m_page + 1);
}

void KeybindingsLayer::onPrevPage(cocos2d::CCObject* sender) {
    goToPage(m_page - 1);
}

void KeybindingsLayer::onInfo(cocos2d::CCObject* sender) {
    int id = sender->getTag();
    auto key = infoKey(id);
    auto str = static_cast<cocos2d::CCString*>(m_values->objectForKey(key));
    
    if (str) {
        auto alert = FLAlertLayer::create(nullptr, "Info", str->getCString());
        alert->show();
    }
}

void KeybindingsLayer::onToggle(cocos2d::CCObject* sender) {
    int id = sender->getTag();
    auto variable = static_cast<cocos2d::CCString*>(m_variables->objectForKey(id));
    
    if (variable) {
        auto manager = GameManager::sharedState();
        manager->toggleGameVariable(variable->getCString());
    }
}

KeybindingsLayer::~KeybindingsLayer() {
    if (m_variables) {
        m_variables->release();
    }
    if (m_values) {
        m_values->release();
    }
}