#include "GJPromoPopup.h"

GJPromoPopup* GJPromoPopup::create(const std::string& filename) {
    auto popup = new GJPromoPopup();
    if (popup && popup->init(filename)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool GJPromoPopup::init(const std::string& filename) {
    m_promoFrame = filename;
    
    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->addImage(m_promoFrame.c_str());
    
    FLAlertLayer::incrementForcePrio();
    
    if (!FLAlertLayer::initWithColor(cocos2d::ccc4(0, 0, 0, 150))) {
        return false;
    }
    
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    auto promo = cocos2d::CCSprite::create(m_promoFrame.c_str());
    m_mainLayer->addChild(promo, -1);
    promo->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    auto closeItem = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(GJPromoPopup::onClose));
    closeItem->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(closeItem, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(
        12.0f - (promo->getContentSize().width * promo->getScale() * 0.5f - centerX),
        promo->getContentSize().height * promo->getScale() * 0.5f + centerY - 12.0f
    ));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    return true;
}

void GJPromoPopup::keyBackClicked() {
    onClose(nullptr);
}

void GJPromoPopup::onClose(cocos2d::CCObject* sender) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    stopAllActions();
    unscheduleAllSelectors();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void GJPromoPopup::onExit() {
    cocos2d::CCLayer::onExit();
    
    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->removeTextureForKey(m_promoFrame.c_str());
}

void GJPromoPopup::registerWithTouchDispatcher() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void GJPromoPopup::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();
        
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));
        
        auto move = cocos2d::CCMoveTo::create(0.5f, pos);
        auto ease = cocos2d::CCEaseElasticOut::create(move, 0.6f);
        m_mainLayer->runAction(ease);
        
        auto director = cocos2d::CCDirector::sharedDirector();
        auto scene = director->getRunningScene();
        
        if (!m_scene) {
            m_scene = scene;
            int highestZ = scene->getHighestChildZ();
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
        }
        
        scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

GJPromoPopup::~GJPromoPopup() {
}