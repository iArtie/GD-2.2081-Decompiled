#include "TutorialPopup.h"
#include "CCMenuItemSpriteExtra.h"

void TutorialPopup::animateMenu() {
    m_targetNode->setScale(0.1f);
    m_targetNode->setVisible(true);
    m_targetNode->runAction(cocos2d::CCEaseElasticOut::create(
        cocos2d::CCScaleTo::create(0.5f, 1.0f), 0.6f));
}

void TutorialPopup::closeTutorial(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    if (m_callbackRegistered) {
        // invoke callback
        if (m_callbackSelector.adj & 1) {
            void (*callback)() = (void (*)())m_callbackSelector.ptr;
            callback();
        } else {
            void (*callback)() = (void (*)())m_callbackSelector.ptr;
            callback();
        }
    }
    removeFromParentAndCleanup(true);
}

TutorialPopup* TutorialPopup::create(std::string frame) {
    TutorialPopup* returnVal = new TutorialPopup();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool TutorialPopup::init(std::string frame) {
    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    setTouchEnabled(true);
    setKeypadEnabled(true);
    m_currentSprite = frame;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(frame.c_str());
    sprite->setPosition(centerX, centerY + 30.0f);
    m_mainLayer->addChild(sprite, 1);

    m_targetNode = CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("NextBtn_001.png"),
        nullptr, menu_selector(TutorialPopup::closeTutorial));

    m_buttonMenu = cocos2d::CCMenu::create((CCMenuItemSpriteExtra*)m_targetNode, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCPoint btnPos = sprite->getPosition() + cocos2d::CCPoint(0.0f, -35.0f - sprite->getContentSize().height * 0.5f);
    m_buttonMenu->setPosition(btnPos);

    ((CCMenuItemSpriteExtra*)m_targetNode)->setScale(0.8f);
    m_targetNode->setVisible(false);
    return true;
}

void TutorialPopup::keyBackClicked() {
    closeTutorial(nullptr);
}

void TutorialPopup::show() {
    FLAlertLayer::show();
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(TutorialPopup::animateMenu)),
        nullptr));
}

TutorialPopup::~TutorialPopup() {
}