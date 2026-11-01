#include "ArtTriggerGameObject.h"
#include "GJBaseGameLayer.h"
#include "EffectGameObject.h"
#include <string>

ArtTriggerGameObject* ArtTriggerGameObject::create(const char* frame)
{
    ArtTriggerGameObject* pRet = new ArtTriggerGameObject();
    if (pRet && pRet->init(frame))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

std::string ArtTriggerGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string saveString = "";
    
    // Get base save string from EffectGameObject
    // This would call the parent class's getSaveString
    
    if (m_artIndex != 0)
    {
        saveString += ",533," + std::to_string(m_artIndex);
    }
    
    return saveString;
}

ArtTriggerGameObject::~ArtTriggerGameObject()
{
    // Destructor would call parent destructors
}