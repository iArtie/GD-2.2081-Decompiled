// GJColorSetupLayer.cpp
#include "Layers/GJColorSetupLayer.h"
#include "Models/LevelSettingsObject.h"
#include "Editor/ColorChannelSprite.h"
#include "Popups/ColorSelectPopup.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJColorSetupLayer* GJColorSetupLayer::create(LevelSettingsObject* object) {
    GJColorSetupLayer* instance = new GJColorSetupLayer();
    if (instance && instance->init(object)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

GJColorSetupLayer::GJColorSetupLayer() : m_delegate(nullptr), m_colorID(0), 
    m_settingsObject(nullptr), m_page(0), m_colorsPerPage(24), m_totalPages(0),
    m_closeOnSelect(false), m_noElasticity(false), m_unk290(nullptr), m_unk298(nullptr),
    m_colorSprites(nullptr), m_colorLabels(nullptr), m_buttonMenu(nullptr),
    m_prevButton(nullptr), m_nextButton(nullptr), m_mainLayer(nullptr) {}

bool GJColorSetupLayer::init(LevelSettingsObject* object) {
    if (!FLAlertLayer::init(this, 150)) {
        return false;
    }
    
    m_settingsObject = object;
    m_noElasticity = true;
    object->retain();
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setColor(cocos2d::ccc3(255, 255, 255));
    bg->setContentSize(cocos2d::CCSize(450, 300));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    m_unk290 = cocos2d::CCDictionary::create();
    m_unk290->retain();
    
    m_unk298 = cocos2d::CCDictionary::create();
    m_unk298->retain();
    
    m_colorLabels = cocos2d::CCArray::create();
    m_colorLabels->retain();
    
    m_colorSprites = cocos2d::CCArray::create();
    m_colorSprites->retain();
    
    m_colorsPerPage = 24;
    m_totalPages = 42; // 1000 / 24 + 2
    
    // Menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    // Title label
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setScale(0.8f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 130.0f));
    
    // Create color sprites and labels
    cocos2d::CCPoint startPos(winSize.width * 0.5f - 132.5f, winSize.height * 0.5f + 100.0f);
    
    for (int i = 1; i <= m_colorsPerPage; ++i) {
        // Label
        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("", "smallFont.fnt");
        label->setScale(0.3f);
        m_mainLayer->addChild(label);
        m_colorLabels->addObject(label);
        
        float x = ((i - 1) % 6) * 53.0f;
        float y = -((i - 1) / 6) * 53.0f;
        label->setPosition(cocos2d::CCPoint(startPos.x + x, startPos.y + y));
        
        // Color sprite
        ColorChannelSprite* colorSprite = ColorChannelSprite::create();
        colorSprite->setScale(0.8f);
        colorSprite->setColor(cocos2d::ccc3(255, 255, 255));
        m_colorSprites->addObject(colorSprite);
        
        // Menu item
        CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(
            colorSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJColorSetupLayer::onColor));
        m_buttonMenu->addChild(menuItem);
        
        cocos2d::CCPoint labelPos = label->getPosition();
        cocos2d::CCPoint itemPos = cocos2d::CCPoint(labelPos.x, labelPos.y - 22.0f);
        cocos2d::CCPoint nodePos = m_buttonMenu->convertToNodeSpace(itemPos);
        menuItem->setPosition(nodePos);
        menuItem->setTag(i);
    }
    
    updateSpriteColors();
    
    // Previous button
    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    prevSprite->setScale(1.0f);
    m_prevButton = CCMenuItemSpriteExtra::create(
        prevSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJColorSetupLayer::onPage));
    m_buttonMenu->addChild(m_prevButton);
    
    cocos2d::CCPoint centerPos = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    m_prevButton->setPosition(cocos2d::CCPoint(centerPos.x - 190.0f, centerPos.y));
    m_prevButton->setSizeMult(2.0f);
    m_prevButton->setTag(1);
    
    // Next button
    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setScale(1.0f);
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, cocos2d::SEL_MenuHandler(&GJColorSetupLayer::onPage));
    m_buttonMenu->addChild(m_nextButton);
    
    m_nextButton->setPosition(cocos2d::CCPoint(centerPos.x + 190.0f, centerPos.y));
    m_nextButton->setSizeMult(2.0f);
    m_nextButton->setTag(0);
    
    // Close button
    cocos2d::CCNode* closeButton = ButtonSprite::create(
        "OK", 0, 1, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", nullptr, 30.0f);
    CCMenuItemSpriteExtra* closeItem = CCMenuItemSpriteExtra::create(
        closeButton, nullptr, this, cocos2d::SEL_MenuHandler(&GJColorSetupLayer::onClose));
    m_buttonMenu->addChild(closeItem);
    
    cocos2d::CCPoint closePos = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 126.0f));
    closeItem->setPosition(closePos);
    
    // Show current page
    m_page = m_settingsObject->m_colorPage;
    showPage(m_page);
    
    return true;
}

void GJColorSetupLayer::keyBackClicked() {
    onClose(nullptr);
}

void GJColorSetupLayer::onClose(cocos2d::CCObject* sender) {
    if (m_delegate && m_colorID > 0) {
        m_delegate->colorSetupClosed();
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJColorSetupLayer::onColor(cocos2d::CCObject* sender) {
    int index = m_page * m_colorsPerPage + sender->getTag();
    
    if (m_closeOnSelect) {
        m_colorID = index;
        onClose(nullptr);
    } else {
        ColorSelectPopup* popup = ColorSelectPopup::create(
            m_settingsObject->m_effectManager->getColorAction(index));
        // Set delegate
        popup->setDelegate(this);
        popup->show();
    }
}

void GJColorSetupLayer::onPage(cocos2d::CCObject* sender) {
    int page = m_page;
    if (sender->getTag() == 1) {
        page--;
    } else {
        page++;
    }
    
    if (page < 0) {
        page = 0;
    } else if (page >= m_totalPages) {
        page = m_totalPages - 1;
    }
    
    m_page = page;
    showPage(page);
}

void GJColorSetupLayer::showPage(int page) {
    m_prevButton->setVisible(page != 0);
    m_nextButton->setVisible(page != m_totalPages - 1);
    m_settingsObject->m_colorPage = page;
    updateSpriteColors();
}

void GJColorSetupLayer::updateSpriteColors() {
    int startIndex = m_page * m_colorsPerPage;
    int count = m_colorSprites->count();
    
    for (int i = 0; i < count; ++i) {
        ColorChannelSprite* sprite = static_cast<ColorChannelSprite*>(m_colorSprites->objectAtIndex(i));
        cocos2d::CCLabelBMFont* label = static_cast<cocos2d::CCLabelBMFont*>(m_colorLabels->objectAtIndex(i));
        
        int colorIndex = startIndex + i;
        
        if (colorIndex > 998) {
            sprite->getParent()->setScale(0.0f);
            sprite->setVisible(false);
            label->setVisible(false);
        } else {
            sprite->getParent()->setScale(1.0f);
            sprite->setVisible(true);
            label->setVisible(true);
            updateSpriteColor(sprite, label, colorIndex + 1);
        }
    }
}

// GJColorSetupLayer.cpp
void GJColorSetupLayer::updateSpriteColor(ColorChannelSprite* sprite, cocos2d::CCLabelBMFont* label, int colorID) {
    // Set label text
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("Col %i", colorID);
    label->setString(str->getCString());
    
    // Update sprite colors
    if (m_settingsObject->m_effectManager->colorExists(colorID)) {
        ColorAction* colorAction = m_settingsObject->m_effectManager->getColorAction(colorID);
        sprite->updateValues(colorAction);
    } else {
        sprite->updateValues(nullptr);
    }
}

GJColorSetupLayer::~GJColorSetupLayer() {
    if (m_settingsObject) {
        m_settingsObject->release();
    }
    if (m_unk290) {
        m_unk290->release();
    }
    if (m_unk298) {
        m_unk298->release();
    }
    if (m_colorSprites) {
        m_colorSprites->release();
    }
    if (m_colorLabels) {
        m_colorLabels->release();
    }
}