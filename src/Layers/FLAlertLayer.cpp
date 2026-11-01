#include "Layers/FLAlertLayer.h"
#include "Layers/ScrollingLayer.h"

FLAlertLayer* FLAlertLayer::create(const char* title, const std::string* desc, const char* btn)
{
    FLAlertLayer* layer = new FLAlertLayer();
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    
    delete layer;
    return nullptr;
}

bool FLAlertLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected == -1 && m_buttonMenu && m_buttonMenu->ccTouchBegan(touch, event))
    {
        m_joystickConnected = touch->m_nId;
        return true;
    }
    
    if (m_scrollingLayer && m_controlConnected == -1 && m_scrollingLayer->ccTouchBegan(touch, event))
    {
        m_controlConnected = touch->m_nId;
        return true;
    }
    
    return true;
}

void FLAlertLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected == touch->m_nId)
    {
        m_buttonMenu->ccTouchCancelled(touch, event);
    }
    else if (touch->m_nId == m_controlConnected)
    {
        m_scrollingLayer->ccTouchCancelled(touch, event);
    }
}

void FLAlertLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected == touch->m_nId)
    {
        m_buttonMenu->ccTouchEnded(touch, event);
        m_joystickConnected = -1;
    }
    else if (touch->m_nId == m_controlConnected)
    {
        m_scrollingLayer->ccTouchEnded(touch, event);
        m_controlConnected = -1;
    }
}

void FLAlertLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_joystickConnected == touch->m_nId)
    {
        m_buttonMenu->ccTouchMoved(touch, event);
    }
    else if (touch->m_nId == m_controlConnected)
    {
        m_scrollingLayer->ccTouchMoved(touch, event);
    }
}

void FLAlertLayer::incrementForcePrio()
{
    if (!m_forcePrioRegistered)
    {
        m_forcePrioRegistered = true;
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
        dispatcher->registerForcePrio(this, 2);
    }
}

void FLAlertLayer::keyBackClicked()
{
    setKeypadEnabled(false);
    setKeyboardEnabled(false);
    
    if (m_reverseKeyBack)
    {
        onBtn2(nullptr);
    }
    else
    {
        onBtn1(nullptr);
    }
}

void FLAlertLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp)
{
    if (key == cocos2d::KEY_Space)
    {
        if (m_noAction)
        {
            onBtn2(nullptr);
            return;
        }
    }
    else if (key == cocos2d::CONTROLLER_X)
    {
        onBtn2(nullptr);
        return;
    }
    
    cocos2d::CCLayer::keyDown(key, timestamp);
}

void FLAlertLayer::onBtn1(cocos2d::CCObject* sender)
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    if (m_alertProtocol)
    {
        m_alertProtocol->FLAlert_Clicked(this, false);
    }
    
    removeFromParentAndCleanup(true);
}

void FLAlertLayer::onBtn2(cocos2d::CCObject* sender)
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    if (m_alertProtocol)
    {
        m_alertProtocol->FLAlert_Clicked(this, true);
    }
    
    removeFromParentAndCleanup(true);
}

void FLAlertLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->addPrioTargetedDelegate(this, -501, true);
}

void FLAlertLayer::show()
{
    if (m_noElasticity)
    {
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCScene* scene = director->m_pRunningScene;
        
        if (!m_scene)
        {
            int highestZ = cocos2d::CCScene::getHighestChildZ(scene);
            m_ZOrder = 105;
            if (highestZ > 104)
            {
                m_ZOrder = highestZ + 1;
            }
            m_scene = scene;
        }
        
        if (!m_ZOrder)
        {
            m_ZOrder = 105;
        }
        
        m_scene->addChild(this, m_ZOrder);
    }
    else
    {
        unsigned char opacity = getOpacity();
        m_mainLayer->setScale(0.1f);
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f), 0.6f));
        
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCScene* scene = director->m_pRunningScene;
        
        if (!m_scene)
        {
            int highestZ = cocos2d::CCScene::getHighestChildZ(scene);
            m_ZOrder = 105;
            if (highestZ > 104)
            {
                m_ZOrder = highestZ + 1;
            }
            m_scene = scene;
        }
        
        if (!m_ZOrder)
        {
            m_ZOrder = 105;
        }
        
        m_scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
    }
    
    setVisible(true);
}

FLAlertLayer::~FLAlertLayer()
{
}