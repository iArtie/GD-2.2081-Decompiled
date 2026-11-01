#include "Objects/DashRingObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Utils/GameToolbox.h"

DashRingObject* DashRingObject::create(const char* frame)
{
    DashRingObject* pRet = new DashRingObject();
    if (pRet && pRet->init(frame))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

std::string DashRingObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string saveStr = RingObject::getSaveString(layer);
    
    // Dash speed (588 decimal = 0x24C)
    if (m_dashSpeed != 0.0f)
    {
        saveStr += ",588," + GameToolbox::floatToString(m_dashSpeed);
    }
    
    // End boost (589 decimal = 0x24D)
    if (m_endBoost != 0.0f)
    {
        saveStr += ",589," + GameToolbox::floatToString(m_endBoost);
    }
    
    // Allow collide (587 decimal = 0x24B)
    if (m_allowCollide)
    {
        saveStr += ",587," + std::to_string(m_allowCollide ? 1 : 0);
    }
    
    // Stop slide (590 decimal = 0x24E)
    if (m_stopSlide)
    {
        saveStr += ",590," + std::to_string(m_stopSlide ? 1 : 0);
    }
    
    // Max duration (591 decimal = 0x24F)
    if (m_maxDuration != 0.0f)
    {
        saveStr += ",591," + GameToolbox::floatToString(m_maxDuration);
    }
    
    return saveStr;
}

DashRingObject::~DashRingObject()
{
    // Base class handles cleanup
}