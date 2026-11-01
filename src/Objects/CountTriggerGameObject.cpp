#include "Objects/CountTriggerGameObject.h"
#include "Utils/GameToolbox.h"

CountTriggerGameObject* CountTriggerGameObject::create(const char* frame) {
    CountTriggerGameObject* pRet = new CountTriggerGameObject();
    if (pRet && pRet->init(frame)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

std::string CountTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveStr = EffectGameObject::getSaveString(layer);
    
    // Object ID 1811: Count Trigger (Pickup)
    if (m_objectID == 1811) {
        if (m_targetGroupID != 0) {
            saveStr += ",33," + std::to_string(m_targetGroupID);
        }
        if (m_itemID != 0) {
            saveStr += ",80," + std::to_string(m_itemID);
        }
        if (m_pickupCount != 0) {
            saveStr += ",77," + std::to_string(m_pickupCount);
        }
        if (m_activateGroup != 0) {
            saveStr += ",56," + std::to_string(m_activateGroup);
        }
        if (m_pickupTriggerMode != 0) {
            saveStr += ",88," + std::to_string(m_pickupTriggerMode);
        }
    }
    // Object ID 1817: Count Trigger (Item)
    else if (m_objectID == 1817) {
        if (m_itemID != 0) {
            saveStr += ",80," + std::to_string(m_itemID);
        }
        if (m_pickupCount != 0) {
            saveStr += ",77," + std::to_string(m_pickupCount);
        }
        if (m_isOverride) {
            saveStr += ",139," + std::to_string(m_isOverride ? 1 : 0);
        }
        if (m_pickupTriggerMultiplier != 0.0f) {
            saveStr += ",449," + GameToolbox::floatToString(m_pickupTriggerMultiplier);
        }
        if (m_pickupTriggerMode != 0) {
            saveStr += ",88," + std::to_string(m_pickupTriggerMode);
        }
    }
    // Object ID 1611: Count Trigger (Group)
    else if (m_objectID == 1611) {
        if (m_targetGroupID != 0) {
            saveStr += ",33," + std::to_string(m_targetGroupID);
        }
        if (m_itemID != 0) {
            saveStr += ",80," + std::to_string(m_itemID);
        }
        if (m_pickupCount != 0) {
            saveStr += ",77," + std::to_string(m_pickupCount);
        }
        if (m_activateGroup != 0) {
            saveStr += ",56," + std::to_string(m_activateGroup);
        }
        if (m_multiActivate) {
            saveStr += ",104," + std::to_string(m_multiActivate ? 1 : 0);
        }
    }
    
    return saveStr;
}

CountTriggerGameObject::~CountTriggerGameObject() {
    // Base class destructor handles cleanup
}