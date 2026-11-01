#include "EnhancedTriggerObject.h"
#include "GJBaseGameLayer.h"

EnhancedTriggerObject* EnhancedTriggerObject::create(const char* frame) {
    EnhancedTriggerObject* ret = new EnhancedTriggerObject();
    if (ret && ret->init(frame)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::string EnhancedTriggerObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    std::string result = saveString;
    
    if (m_minXID) {
        result += ",�," + std::to_string(m_minXID); // 0x204
    }
    
    if (m_minYID) {
        result += ",�," + std::to_string(m_minYID); // 0x205
    }
    
    if (m_maxXID) {
        result += ",�," + std::to_string(m_maxXID); // 0x206
    }
    
    if (m_maxYID) {
        result += ",�," + std::to_string(m_maxYID); // 0x207
    }
    
    return result;
}

EnhancedTriggerObject::~EnhancedTriggerObject() {
    // Base class destructor handles cleanup
}