#include "Layers/SelectFontLayer.h"
#include "Layers/LevelEditorLayer.h"
#include "LevelSettings.h"
#include "Managers/GameManager.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/InfoAlertButton.h"

SelectFontLayer* SelectFontLayer::create(LevelEditorLayer* editorLayer) {
    SelectFontLayer* layer = new SelectFontLayer();
    if (layer && layer->init(editorLayer)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SelectFontLayer::init(LevelEditorLayer* layer) {
    if (!cocos2d::CCLayerColor::initWithColor(ccc4(0, 0, 0, 150))) {
        return false;
    }

    FLAlertLayer::incrementForcePrio();

    m_editorLayer = layer;
    m_noElasticity = true;
    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(300.0f, 200.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Select Font", "bigFont.fnt");
    addChild(title);
    title->setScale(0.8f);
    title->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f + 100.0f - 16.0f));

    ButtonSprite* okSprite = ButtonSprite::create(
        "OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okButton = CCMenuItemSpriteExtra::create(
        okSprite, nullptr, menu_selector(SelectFontLayer::onClose));
    m_buttonMenu = cocos2d::CCMenu::create(okButton, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        winSize.height * 0.5f - 100.0f + 24.0f));

    InfoAlertButton* infoButton = InfoAlertButton::create(
        "Help",
        "Choose a font that will be used in this level.\nYou can only use one font at a time.",
        1.0f);
    m_buttonMenu->addChild(infoButton);
    infoButton->setPosition(cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        cocos2d::CCPoint(
            winSize.width * 0.5f - 150.0f + 18.0f,
            winSize.height * 0.5f + 100.0f - 18.0f)));

    m_joystickConnected = false;

    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrow->setFlipX(true);
    rightArrow->setScale(0.8f);
    CCMenuItemSpriteExtra* rightButton = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SelectFontLayer::onChangeFont));
    m_buttonMenu->addChild(rightButton);
    rightButton->setTag(1);
    cocos2d::CCPoint rightPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        center + cocos2d::CCPoint(110.0f, 0.0f));
    rightButton->setPosition(rightPos);
    rightButton->setSizeMult(2.0f);
    rightButton->setTag(1);

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrow->setScale(0.8f);
    CCMenuItemSpriteExtra* leftButton = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SelectFontLayer::onChangeFont));
    m_buttonMenu->addChild(leftButton);
    leftButton->setTag(2);
    cocos2d::CCPoint leftPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu,
        center + cocos2d::CCPoint(-110.0f, 0.0f));
    leftButton->setPosition(leftPos);
    leftButton->setSizeMult(2.0f);
    leftButton->setTag(0);

    updateFontLabel();

    return true;
}

void SelectFontLayer::keyBackClicked() {
    onClose(nullptr);
}

void SelectFontLayer::onChangeFont(cocos2d::CCObject* sender) {
    int currentFont = m_editorLayer->m_levelSettings->m_fontIndex;
    int newFont;

    if (sender->getTag() == 1) {
        int candidate = currentFont + 1;
        if (candidate < 0) {
            candidate = 0;
        }
        newFont = (candidate > 59) ? 59 : candidate;
    } else {
        int candidate = currentFont - 1;
        if (candidate < 0) {
            candidate = 0;
        }
        newFont = (candidate > 59) ? 59 : candidate;
    }

    if (m_font != newFont) {
        if (m_fontLabel) {
            m_fontLabel->removeFromParent();
            m_fontLabel = nullptr;
        }
        m_editorLayer->updateLevelFont(newFont);
        updateFontLabel();
    }
}

void SelectFontLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SelectFontLayer::updateFontLabel() {
    int currentFont = m_editorLayer->m_levelSettings->m_fontIndex;

    if (currentFont == m_font) {
        return;
    }

    if (m_fontLabel) {
        m_fontLabel->removeFromParent();
        m_fontLabel = nullptr;
    }

    m_font = currentFont;

    std::string labelText = cocos2d::CCString::createWithFormat(
        "Font %i", currentFont + 1)->getCString();

    GameManager* gm = GameManager::sharedState();
    gm->loadFont(currentFont);

    m_fontLabel = cocos2d::CCLabelBMFont::create(
        labelText.c_str(), GameManager::sharedState()->getFontFile(gm->m_loadedFont));
    addChild(m_fontLabel);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_fontLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
}

SelectFontLayer::~SelectFontLayer() {
}