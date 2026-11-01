#include "Objects/TimerTriggerGameObject.h"
#include "Layers/GJBaseGameLayer.h"

TimerTriggerGameObject* TimerTriggerGameObject::create(const char* frame) {
    TimerTriggerGameObject* returnVal = new TimerTriggerGameObject();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool TimerTriggerGameObject::init(const char* frame) {
    return EffectGameObject::init(frame);
}

std::string TimerTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);

    if (m_itemID) {
        saveString += ",";
        saveString += std::to_string(80);
        saveString += ",";
        saveString += std::to_string(m_itemID);
    }

    if (m_objectID == 3615) {
        if (m_targetTime != 0.0) {
            saveString += ",";
            saveString += std::to_string(473);
            saveString += ",";
            saveString += std::to_string(m_targetTime);
        }
        if (m_multiActivate) {
            saveString += ",";
            saveString += std::to_string(475);
            saveString += ",";
            saveString += std::to_string(m_multiActivate);
        }
        if (m_targetGroupID) {
            saveString += ",";
            saveString += std::to_string(51);
            saveString += ",";
            saveString += std::to_string(m_targetGroupID);
        }
    } else if (m_objectID == 3617) {
        if (m_controlType) {
            saveString += ",";
            saveString += std::to_string(472);
            saveString += ",";
            saveString += std::to_string(m_controlType);
        }
    } else if (m_objectID == 3614) {
        if (m_startTime != 0.0) {
            saveString += ",";
            saveString += std::to_string(467);
            saveString += ",";
            saveString += std::to_string(m_startTime);
        }
        if (m_targetTime != 0.0) {
            saveString += ",";
            saveString += std::to_string(473);
            saveString += ",";
            saveString += std::to_string(m_targetTime);
        }
        if (m_stopTimeEnabled) {
            saveString += ",";
            saveString += std::to_string(474);
            saveString += ",";
            saveString += std::to_string(m_stopTimeEnabled);
        }
        if (m_dontOverride) {
            saveString += ",";
            saveString += std::to_string(468);
            saveString += ",";
            saveString += std::to_string(m_dontOverride);
        }
        if (m_ignoreTimeWarp) {
            saveString += ",";
            saveString += std::to_string(469);
            saveString += ",";
            saveString += std::to_string(m_ignoreTimeWarp);
        }
        if (m_timeMod != 0.0) {
            saveString += ",";
            saveString += std::to_string(470);
            saveString += ",";
            saveString += std::to_string(m_timeMod);
        }
        if (m_startPaused) {
            saveString += ",";
            saveString += std::to_string(471);
            saveString += ",";
            saveString += std::to_string(m_startPaused);
        }
        if (m_targetGroupID) {
            saveString += ",";
            saveString += std::to_string(51);
            saveString += ",";
            saveString += std::to_string(m_targetGroupID);
        }
    }

    return saveString;
}

TimerTriggerGameObject::~TimerTriggerGameObject() {
}