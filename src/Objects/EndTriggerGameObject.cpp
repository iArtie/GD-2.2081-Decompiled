#include "Objects/EndTriggerGameObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "fmt/format.h"

EndTriggerGameObject* EndTriggerGameObject::create() {
    EndTriggerGameObject* ret = new EndTriggerGameObject();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::string EndTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    std::string result = saveString;
    
    if (m_targetGroupID) {
        result += ",3," + std::to_string(m_targetGroupID);
    }
    
    if (m_centerGroupID) {
        result += ",G," + std::to_string(m_centerGroupID);
    }
    
    if (m_noEffects) {
        result += ",�," + std::to_string(m_noEffects); // 0x1CC
    }
    
    if (m_noSFX) {
        result += ",�," + std::to_string(m_noSFX); // 0x1CD
    }
    
    if (m_instant) {
        result += ",�," + std::to_string(m_instant); // 0x1E7
    }
    
    return result;
}

bool EndTriggerGameObject::init() {
    return EffectGameObject::init("edit_eEndBtn_001.png");
}

EndTriggerGameObject::~EndTriggerGameObject() {
    // Base class destructor handles cleanup
}