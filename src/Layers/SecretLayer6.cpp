#include "Layers/SecretLayer6.h"
#include "Layers/SecretGame01Layer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "Layers/MenuLayer.h"

SecretLayer6* SecretLayer6::create() {
    SecretLayer6* layer = new SecretLayer6();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool SecretLayer6::init() {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    cocos2d::CCTextureCache::sharedTextureCache()->addImage("SecretSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("SecretSheet.plist");

    GameManager::sharedState()->fadeInMusic("secretLoop03.mp3");

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCSprite* background = cocos2d::CCSprite::create("GJ_gradientBG.png");
    background->setAnchorPoint(cocos2d::CCPoint(0, 0));
    addChild(background, -3);
    background->setScaleX((winSize.width + 10.0f) / background->getContentSize().width);
    background->setScaleY((winSize.height + 10.0f) / background->getContentSize().height);
    background->setPosition(cocos2d::CCPoint(-5, -5));
    background->setColor(ccc3(30, 60, 60));

    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    label->setScale(1.0f);
    addChild(label);
    label->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height - 24.0f));

    m_gameLayer = SecretGame01Layer::create();
    addChild(m_gameLayer, 100);

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backButton = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(SecretLayer6::onBack));
    backButton->setSizeMult(1.5f);
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(backButton, nullptr);
    addChild(menu, 1);
    menu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backButton);
    }

    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer6::startGame01)),
        nullptr);
    cocos2d::CCNode::runAction(this, seq);

    return true;
}

void SecretLayer6::keyBackClicked() {
    onBack(nullptr);
}

void SecretLayer6::onBack(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    GameManager::sharedState()->fadeInMenuMusic();
    cocos2d::CCScene* scene = MenuLayer::scene(false);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
}

cocos2d::CCScene* SecretLayer6::scene() {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretLayer6* layer = SecretLayer6::create();
    scene->addChild(layer);
    return scene;
}

void SecretLayer6::startGame01() {
    if (m_gameLayer) {
        m_gameLayer->resetGame();
    }
}

SecretLayer6::~SecretLayer6() {
}