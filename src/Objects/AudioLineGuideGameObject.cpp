#include "Objects/AudioLineGuideGameObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Objects/EffectGameObject.h"
#include <string>

AudioLineGuideGameObject* AudioLineGuideGameObject::create()
{
    AudioLineGuideGameObject* pRet = new AudioLineGuideGameObject();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

std::string AudioLineGuideGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string saveString = "";
    
    // Get base save string from EffectGameObject
    // This would call the parent class's getSaveString
    
    if (m_beatsPerMinute != 0)
    {
        saveString += ",498," + std::to_string(m_beatsPerMinute);
        if (m_beatsPerBar != 0)
        {
            saveString += ",501," + std::to_string(m_beatsPerBar);
        }
    }
    else if (m_beatsPerBar != 0)
    {
        saveString += ",501," + std::to_string(m_beatsPerBar);
    }
    
    if (m_speed != Normal)
    {
        saveString += ",499," + std::to_string(m_speed);
    }
    
    if (m_disabled)
    {
        saveString += ",500," + std::to_string(1);
    }
    
    if (m_duration != 0.0)
    {
        saveString += ",10," + std::to_string(m_duration);
    }
    
    return saveString;
}

bool AudioLineGuideGameObject::init()
{
    if (!EffectGameObject::init(this, "edit_eBPMBtn_001.png"))
        return false;
    
    m_duration = 60.0;
    return true;
}

AudioLineGuideGameObject::~AudioLineGuideGameObject()
{
    // Destructor would call parent destructors
}