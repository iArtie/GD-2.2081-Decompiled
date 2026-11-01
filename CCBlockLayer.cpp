#include "CCBlockLayer.h"
#include "cocos2d.h"

bool CCBlockLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    return true;
}

void CCBlockLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    // Empty implementation
}

void CCBlockLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    // Empty implementation
}

void CCBlockLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    // Empty implementation
}

CCBlockLayer* CCBlockLayer::create() {
    CCBlockLayer* layer = new CCBlockLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

void CCBlockLayer::customSetup() {
    // Empty implementation - to be overridden by subclasses
}

void CCBlockLayer::decrementForcePrio() {
    if (m_forcePrioRegistered) {
        m_forcePrioRegistered = false;
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);
    }
}

void CCBlockLayer::disableUI() {
    // Empty implementation - to be overridden by subclasses
}

void CCBlockLayer::draw() {
    if (this->getOpacity() > 0) {
        cocos2d::CCLayerColor::draw();
    }
}

void CCBlockLayer::enableUI() {
    // Empty implementation - to be overridden by subclasses
}

void CCBlockLayer::enterAnimFinished() {
    // Empty implementation - to be overridden by subclasses
}

void CCBlockLayer::enterLayer() {
    enableUI();
    showLayer(false);
}

void CCBlockLayer::exitLayer() {
    this->setKeypadEnabled(false);
    disableUI();
    hideLayer(false);
}

void CCBlockLayer::hideLayer(bool instant) {
    layerHidden();
}

void CCBlockLayer::incrementForcePrio() {
    if (!m_forcePrioRegistered) {
        m_forcePrioRegistered = true;
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->registerForcePrio(this, 2);
    }
}

bool CCBlockLayer::init() {
    cocos2d::ccColor4B color = {0, 0, 0, 125};
    if (!cocos2d::CCLayerColor::initWithColor(color)) {
        return false;
    }

    incrementForcePrio();
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    customSetup();

    m_closeOnHide = true;
    return true;
}

void CCBlockLayer::keyBackClicked() {
    exitLayer();
}

void CCBlockLayer::layerHidden() {
    this->setVisible(false);
    if (m_closeOnHide) {
        this->removeFromParentAndCleanup(true);
    }
}

void CCBlockLayer::layerVisible() {
    this->setVisible(true);
}

void CCBlockLayer::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void CCBlockLayer::showLayer(bool instant) {
    layerVisible();
}

CCBlockLayer::~CCBlockLayer() {
    // Destructor
}