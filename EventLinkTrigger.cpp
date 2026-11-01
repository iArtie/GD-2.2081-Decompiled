#include "EventLinkTrigger.h"
#include "GJBaseGameLayer.h"

EventLinkTrigger* EventLinkTrigger::create() {
    EventLinkTrigger* ret = new EventLinkTrigger();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::string EventLinkTrigger::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    std::string result = saveString;
    
    if (m_targetGroupID) {
        result += ",3," + std::to_string(m_targetGroupID);
    }
    
    if (m_extraID) {
        result += ",�," + std::to_string(m_extraID); // 0x1BF
    }
    
    if (m_extraID2) {
        result += ",�," + std::to_string(m_extraID2); // 0x20D
    }
    
    if (m_resetRemap) {
        result += ",�," + std::to_string(m_resetRemap); // 0x1AF
    }
    
    if (!m_eventIDs.empty()) {
        result += ",�,"; // 0x1AE
        
        bool first = true;
        for (int id : m_eventIDs) {
            if (!first) {
                result += ".";
            }
            result += std::to_string(id);
            first = false;
        }
    }
    
    return result;
}

bool EventLinkTrigger::init() {
    return EffectGameObject::init("edit_eEventLinkBtn_001.png");
}

EventLinkTrigger::~EventLinkTrigger() {
    // Base class destructor handles cleanup
}