#include "CustomizeObjectSettingsPopup.h"

CustomizeObjectSettingsPopup* CustomizeObjectSettingsPopup::create(GameObject* targetObject, cocos2d::CCArray* targetObjects, CustomizeObjectLayer* customizeLayer)
{
    CustomizeObjectSettingsPopup* pRet = new CustomizeObjectSettingsPopup();
    if (pRet && pRet->init(targetObject, targetObjects, customizeLayer))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CustomizeObjectSettingsPopup::init(GameObject* targetObject, cocos2d::CCArray* targetObjects, CustomizeObjectLayer* customizeLayer)
{
    if (!SetupTriggerPopup::init())
        return false;
    
    m_customizeLayer = customizeLayer;
    m_settingsChanged = false;
    
    // Setup UI for object settings
    // This would include toggles for glow, animation, etc.
    
    return true;
}

void CustomizeObjectSettingsPopup::onClose(cocos2d::CCObject* sender)
{
    if (m_customizeLayer && m_settingsChanged)
    {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(this);
        SetupTriggerPopup::onClose(sender);
        m_customizeLayer->recreateLayer();
    }
    else
    {
        cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
        dispatcher->unregisterForcePrio(this);
        SetupTriggerPopup::onClose(sender);
    }
}

CustomizeObjectSettingsPopup::~CustomizeObjectSettingsPopup()
{
    // Destructor - base class handles cleanup
}