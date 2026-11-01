#include "TutorialLayer.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "TextArea.h"
#include "AppDelegate.h"

TutorialLayer* TutorialLayer::create() {
    TutorialLayer* returnVal = new TutorialLayer();
    if (returnVal->init()) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool TutorialLayer::init() {
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    m_tutorialLayer = cocos2d::CCLayer::create();
    m_mainLayer->addChild(m_tutorialLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(420.0f, 260.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -1);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Tutorial", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel, 20);
    titleLabel->setPosition(centerX, centerY + 130.0f - 25.0f);
    if (titleLabel->getContentSize().width > 300.0f)
        titleLabel->setScale(300.0f / titleLabel->getContentSize().width);

    m_buttonMenu = cocos2d::CCMenu::create(
        CCMenuItemSpriteExtra::create(
            cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            nullptr, menu_selector(TutorialLayer::onClose)),
        nullptr);
    m_buttonMenu->setPosition(centerX - 210.0f + 10.0f, centerY + 130.0f - 10.0f);
    m_buttonMenu->setSizeMult(1.6f);
    m_mainLayer->addChild(m_buttonMenu, 10);

    m_nextButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Next", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", 40.0f),
        nullptr, menu_selector(TutorialLayer::onNext));
    m_buttonMenu->addChild(m_nextButton);

    m_exitButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Exit", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", 40.0f),
        nullptr, menu_selector(TutorialLayer::onClose));
    m_buttonMenu->addChild(m_exitButton);

    m_nextButton->setPosition(cocos2d::CCNode::convertToNodeSpace(
        m_buttonMenu, &cocos2d::CCPoint(centerX + 210.0f - 52.0f, centerY - 130.0f + 50.0f)));
    m_exitButton->setPosition(m_nextButton->getPosition());
    m_exitButton->setVisible(false);

    m_page = 0;
    loadPage(1);
    return true;
}

void TutorialLayer::keyBackClicked() {
    onClose(nullptr);
}

void TutorialLayer::loadPage(int page) {
    m_tutorialLayer->removeAllChildrenWithCleanup(true);
    removeTutorialTexture();
    m_page = page;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    std::string tutorialImage = cocos2d::CCString::createWithFormat("tutorial_%02d.png", page)->getCString();
    cocos2d::CCSprite* tutorialSprite = cocos2d::CCSprite::create(tutorialImage.c_str());
    tutorialSprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    if (AppDelegate::get()->m_platformID == 3)
        tutorialSprite->setScale(2.0f);
    m_tutorialLayer->addChild(tutorialSprite);
    tutorialSprite->setPosition(centerX - 210.0f + 10.0f, centerY - 130.0f + 10.0f);

    std::string tutorialText;
    float textYOffset = 0.0f;
    float textWidth = 40.0f;
    float textScale = 0.7f;

    switch (page) {
        case 1:
            tutorialText = "<cg>Tap</c> the screen to jump.\n<cg>Hold</c> down to keep jumping.";
            break;
        case 2:
            tutorialText = "<cg>Hold</c> to fly up.\n<cg>Release</c> to fly down.";
            break;
        case 3:
            tutorialText = "You can enter <cl>practice mode</c> from the pause menu.\nPractice mode lets you place <cg>checkpoints</c>.";
            textYOffset = -13.0f;
            textWidth = 40.0f;
            textScale = 0.6f;
            break;
        case 4:
            tutorialText = "You can place checkpoints manually, or use the auto-checkpoint feature.\n"
                           "Tap the delete button to remove your last checkpoint.";
            textYOffset = -13.0f;
            textWidth = 45.0f;
            textScale = 0.55f;
            break;
        case 5:
            tutorialText = "<cy>Jump Orbs</c> activate when you are on top of them.\n"
                           "<cg>Tap</c> while touching an orb to interact with it and use its effect.";
            textYOffset = -13.0f;
            textWidth = 40.0f;
            textScale = 0.6f;
            break;
    }

    TextArea* textArea = TextArea::create(tutorialText, "bigFont.fnt", 1.0f, 400.0f / textScale,
                                          cocos2d::CCPoint(textWidth, 0.0f), 20.0f, false);
    textArea->setScale(textScale);
    m_tutorialLayer->addChild(textArea);
    textArea->setPosition(centerX, centerY + 130.0f - 75.0f + textYOffset);

    if (m_page <= 4) {
        m_nextButton->setVisible(true);
        m_nextButton->setEnabled(true);
        m_exitButton->setVisible(false);
        m_exitButton->setEnabled(false);
    } else {
        m_nextButton->setVisible(false);
        m_nextButton->setEnabled(false);
        m_exitButton->setVisible(true);
        m_exitButton->setEnabled(true);
    }
}

void TutorialLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    m_tutorialLayer->removeAllChildrenWithCleanup(true);
    removeFromParentAndCleanup(true);
    removeTutorialTexture();
}

void TutorialLayer::onNext(cocos2d::CCObject* sender) {
    if (m_page > 4)
        onClose(nullptr);
    else
        loadPage(m_page + 1);
}

void TutorialLayer::removeTutorialTexture() {
    if (m_page) {
        std::string textureName = cocos2d::CCString::createWithFormat("tutorial_%02d.png", m_page)->getCString();
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey(textureName.c_str());
    }
}

TutorialLayer::~TutorialLayer() {
}