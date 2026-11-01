#include "AdvancedFollowEditObject.h"
#include "GJBaseGameLayer.h"
#include <string>

AdvancedFollowEditObject* AdvancedFollowEditObject::create(const char* frame)
{
    AdvancedFollowEditObject* pRet = new AdvancedFollowEditObject();
    if (pRet && pRet->init(frame))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

std::string AdvancedFollowEditObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string saveString = "";
    
    // Get base save string from AdvancedFollowTriggerObject
    // This would call the parent class's getSaveString
    
    if (m_modX != 0.0 || m_modXVariance != 0.0)
    {
        if (m_modX != 0.0)
        {
            saveString += ",566," + std::to_string(m_modX);
        }
        if (m_modXVariance != 0.0)
        {
            saveString += ",567," + std::to_string(m_modXVariance);
        }
    }
    
    if (m_modY != 0.0 || m_modYVariance != 0.0)
    {
        if (m_modY != 0.0)
        {
            saveString += ",568," + std::to_string(m_modY);
        }
        if (m_modYVariance != 0.0)
        {
            saveString += ",569," + std::to_string(m_modYVariance);
        }
    }
    
    if (m_redirectDirection != 0)
    {
        saveString += ",570," + std::to_string(m_redirectDirection);
    }
    
    if (m_targetControlID != 0)
    {
        saveString += ",535," + std::to_string(m_targetControlID);
    }
    
    return saveString;
}

AdvancedFollowEditObject::~AdvancedFollowEditObject()
{
    // Destructor would call parent destructors
}