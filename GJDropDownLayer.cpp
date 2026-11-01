#include "GJDropDownLayer.h"
#include "GameManager.h"

GJDropDownLayer* GJDropDownLayer::create(const char* title)
{
    GJDropDownLayer* layer = new GJDropDownLayer();
    if (layer && layer->init(title))
    {
        layer->autorelease();
        return layer;
    }
    
    delete layer;
    return nullptr;
}

bool GJDropDownLayer::init(const char* title)
{
    if (!cocos2d::CCLayerColor::init())
    {
        return false;
    }
    
    // Inicialización básica
    m_buttonMenu = nullptr;
    m_mainLayer = nullptr;
    m_delegate = nullptr;
    m_fastMenu = false;
    m_closeOnHide = false;
    
    // Configuración personalizada
    customSetup();
    
    return true;
}

void GJDropDownLayer::customSetup()
{
}

void GJDropDownLayer::draw()
{
    if (getOpacity() > 0)
    {
        cocos2d::CCLayerColor::draw();
    }
}

void GJDropDownLayer::enterLayer()
{
    enableUI();
    showLayer(false);
}

void GJDropDownLayer::exitLayer(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    disableUI();
    hideLayer(m_fastMenu);
}

void GJDropDownLayer::hideLayer(bool instant)
{
    if (m_closeOnHide && m_delegate)
    {
        m_delegate->dropDownLayerWillClose(this);
    }
    
    m_mainLayer->stopAllActions();
    
    GameManager* gameManager = GameManager::sharedState();
    bool fastMode = gameManager->getGameVariable("0168") || instant;
    
    if (fastMode)
    {
        m_mainLayer->setPosition(m_startPosition);
        setOpacity(0);
        layerHidden();
    }
    else
    {
        cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, m_startPosition);
        cocos2d::CCEaseInOut* easeInOut = cocos2d::CCEaseInOut::create(moveTo, 2.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(GJDropDownLayer::layerHidden));
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(easeInOut, callFunc, nullptr);
        m_mainLayer->runAction(sequence);
        
        cocos2d::CCFadeTo* fadeTo = cocos2d::CCFadeTo::create(0.5f, 0);
        runAction(fadeTo);
    }
}

void GJDropDownLayer::showLayer(bool instant)
{
    m_mainLayer->stopAllActions();
    layerVisible();
    
    GameManager* gameManager = GameManager::sharedState();
    bool fastMode = gameManager->getGameVariable("0168") || instant;
    
    if (fastMode)
    {
        m_mainLayer->setPosition(m_endPosition);
        setOpacity(125);
        enterAnimFinished();
    }
    else
    {
        cocos2d::CCMoveTo* moveTo = cocos2d::CCMoveTo::create(0.5f, m_endPosition);
        cocos2d::CCEaseInOut* easeInOut = cocos2d::CCEaseInOut::create(moveTo, 2.0f);
        cocos2d::CCCallFunc* callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(GJDropDownLayer::enterAnimFinished));
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(easeInOut, callFunc, nullptr);
        m_mainLayer->runAction(sequence);
        
        setOpacity(0);
        cocos2d::CCFadeTo* fadeTo = cocos2d::CCFadeTo::create(0.5f, 125);
        runAction(fadeTo);
    }
}

void GJDropDownLayer::layerHidden()
{
    setVisible(false);
    if (m_closeOnHide)
    {
        removeFromParentAndCleanup(true);
    }
}

void GJDropDownLayer::layerVisible()
{
    setVisible(true);
}

void GJDropDownLayer::enterAnimFinished()
{
}

void GJDropDownLayer::enableUI()
{
    if (m_buttonMenu)
    {
        m_buttonMenu->setTouchEnabled(true);
    }
}

void GJDropDownLayer::disableUI()
{
    if (m_buttonMenu)
    {
        m_buttonMenu->setTouchEnabled(false);
    }
}

void GJDropDownLayer::keyBackClicked()
{
    exitLayer(nullptr);
}

void GJDropDownLayer::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTouchDispatcher* dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

bool GJDropDownLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    return true;
}

void GJDropDownLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void GJDropDownLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void GJDropDownLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

GJDropDownLayer::~GJDropDownLayer()
{
}