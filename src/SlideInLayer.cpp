#include "SlideInLayer.h"
#include "SlideInLayerDelegate.h"

bool SlideInLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    return true;
}

void SlideInLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

void SlideInLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

void SlideInLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

SlideInLayer* SlideInLayer::create() {
    SlideInLayer* returnVal = new SlideInLayer();
    if (returnVal->init()) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void SlideInLayer::customSetup() {
}

void SlideInLayer::disableUI() {
    m_buttonMenu->setTouchEnabled(false);
}

void SlideInLayer::draw() {
    if (getOpacity())
        cocos2d::CCLayerColor::draw();
}

void SlideInLayer::enableUI() {
    m_buttonMenu->setTouchEnabled(true);
}

void SlideInLayer::enterAnimFinished() {
}

void SlideInLayer::enterLayer() {
    enableUI();
    showLayer(false);
}

void SlideInLayer::exitLayer(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    if (m_delegate)
        m_delegate->slideInLayerClosed(this);
    disableUI();
    hideLayer(false);
}

void SlideInLayer::hideLayer(bool instant) {
    m_mainLayer->stopAllActions();
    if (instant) {
        m_mainLayer->setPosition(&m_startPosition);
        setOpacity(0);
        layerHidden();
    } else {
        cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, &m_startPosition);
        cocos2d::CCEaseInOut* ease = cocos2d::CCEaseInOut::create(moveTo, 2.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(SlideInLayer::layerHidden));
        m_mainLayer->runAction(cocos2d::CCSequence::create(ease, callFunc, nullptr));
        runAction(cocos2d::CCFadeTo::create(0.5f, 0));
    }
}

bool SlideInLayer::init() {
    m_closeOnHide = true;

    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->registerForcePrio(this, 2);

    if (!cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0)))
        return false;

    setTouchEnabled(true);
    setKeypadEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);

    cocos2d::CCSprite* bg = cocos2d::CCSprite::createWithSpriteFrameName("pauseBG_001.png");
    bg->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.0f));
    bg->setPosition(winSize.width * 0.5f, winSize.height * 0.5f - 240.0f);
    m_mainLayer->addChild(bg, -2);
    bg->setScaleX(cocos2d::CCDirector::sharedDirector()->getScreenScaleFactorW());

    float extraHeight = (winSize.height - 320.0f) + 4.0f;
    if (cocos2d::CCDirector::sharedDirector()->getScreenScaleFactorMax() != 1.0f && extraHeight > 0.0f) {
        cocos2d::CCLayerColor* extraLayer = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, 255));
        m_mainLayer->addChild(extraLayer, -3);
        extraLayer->setPosition(0.0f, bg->getPosition().y - extraHeight + 2.0f);
    }

    m_exitSprite = cocos2d::CCSprite::createWithSpriteFrameName("startBtn_001.png");
    m_exitButton = CCMenuItemSpriteExtra::create(m_exitSprite, nullptr, menu_selector(SlideInLayer::exitLayer));
    m_exitButton->m_activateSound = "";
    m_exitButton->setScale(0.85f);
    m_exitButton->m_baseScale = 0.85f;

    m_buttonMenu = cocos2d::CCMenu::create(m_exitButton, nullptr);
    m_buttonMenu->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 210.0f);

    m_endPosition = cocos2d::CCPoint(0.0f, (winSize.height * 0.5f - 240.0f) - ((winSize.height - 480.0f) * 0.5f));
    m_startPosition = cocos2d::CCPoint(m_endPosition.x, m_endPosition.y - winSize.height);

    customSetup();

    m_mainLayer->addChild(m_buttonMenu, 10);

    hideLayer(true);

    return true;
}

void SlideInLayer::keyBackClicked() {
    exitLayer(nullptr);
}

void SlideInLayer::layerHidden() {
    setVisible(false);
    if (m_closeOnHide)
        removeFromParentAndCleanup(true);
}

void SlideInLayer::layerVisible() {
    setVisible(true);
}

void SlideInLayer::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void SlideInLayer::showLayer(bool instant) {
    m_mainLayer->stopAllActions();
    layerVisible();
    if (instant) {
        m_mainLayer->setPosition(&m_endPosition);
        setOpacity(125);
        enterAnimFinished();
    } else {
        cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, &m_endPosition);
        cocos2d::CCEaseInOut* ease = cocos2d::CCEaseInOut::create(moveTo, 2.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(SlideInLayer::enterAnimFinished));
        m_mainLayer->runAction(cocos2d::CCSequence::create(ease, callFunc, nullptr));
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.5f, 125));
    }
}

SlideInLayer::~SlideInLayer() {
}