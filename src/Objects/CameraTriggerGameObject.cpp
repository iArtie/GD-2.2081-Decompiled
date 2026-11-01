#include "Objects/CameraTriggerGameObject.h"
#include "Objects/EffectGameObject.h"
#include "Layers/GJBaseGameLayer.h"

CameraTriggerGameObject* CameraTriggerGameObject::create(const char* frame) {
    CameraTriggerGameObject* obj = new CameraTriggerGameObject();
    if (obj && obj->init(frame)) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

std::string CameraTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result;
    
    // Get base save string from EffectGameObject
    std::string baseString = EffectGameObject::getSaveString(this, layer);
    result += baseString;
    
    if (m_objectID == 2015) {
        if (m_duration != 0.0) {
            result += ",10," + std::to_string(m_duration);
        }
        if (m_easingType != 0) {
            result += ",30," + std::to_string(m_easingType);
        }
        if (m_easingRate != 0.0) {
            result += ",85," + std::to_string(m_easingRate);
        }
        if (m_rotationDegrees != 0.0) {
            result += ",68," + std::to_string(m_rotationDegrees);
        }
        if (m_lockObjectRotation) {
            result += ",70,1";
        }
        if (m_isDirectionFollowSnap360) {
            result += ",394,1";
        }
    } else if (m_objectID == 2016) {
        if (m_moveOffset.x != 0.0) {
            result += ",28," + std::to_string(m_moveOffset.x);
        }
        if (m_moveOffset.y != 0.0) {
            result += ",29," + std::to_string(m_moveOffset.y);
        }
        if (m_zoomValue != 0.0) {
            result += ",371," + std::to_string(m_zoomValue);
        }
        if (m_previewOpacity != 0.0) {
            result += ",506," + std::to_string(m_previewOpacity);
        }
    } else if (m_objectID == 2062) {
        if (m_targetGroupID != 0) {
            result += ",51," + std::to_string(m_targetGroupID);
        }
        if (m_edgeDirection != 0) {
            result += ",164," + std::to_string(m_edgeDirection);
        }
    } else if (m_objectID == 1914) {
        if (m_duration != 0.0) {
            result += ",10," + std::to_string(m_duration);
        }
        if (m_easingType != 0) {
            result += ",30," + std::to_string(m_easingType);
        }
        if (m_easingRate != 0.0) {
            result += ",85," + std::to_string(m_easingRate);
        }
        if (m_centerGroupID != 0) {
            result += ",71," + std::to_string(m_centerGroupID);
        }
        if (m_moveTargetMode != 0) {
            result += ",101," + std::to_string(m_moveTargetMode);
        }
        if (m_exitStatic) {
            result += ",110,1";
        }
        if (m_followObject) {
            result += ",212,1";
        }
        if (m_followEasing != 0.0) {
            result += ",213," + std::to_string(m_followEasing);
        }
        if (m_smoothVelocity) {
            result += ",453,1";
        }
        if (m_velocityModifier != 0.0) {
            result += ",454," + std::to_string(m_velocityModifier);
        }
        if (m_exitInstant) {
            result += ",465,1";
        }
    } else if (m_objectID == 1916) {
        if (m_moveOffset.x != 0.0) {
            result += ",28," + std::to_string(m_moveOffset.x);
        }
        if (m_moveOffset.y != 0.0) {
            result += ",29," + std::to_string(m_moveOffset.y);
        }
        if (m_duration != 0.0) {
            result += ",10," + std::to_string(m_duration);
        }
        if (m_easingType != 0) {
            result += ",30," + std::to_string(m_easingType);
        }
        if (m_easingRate != 0.0) {
            result += ",85," + std::to_string(m_easingRate);
        }
        if (m_moveTargetMode != 0) {
            result += ",101," + std::to_string(m_moveTargetMode);
        }
    }
    
    return result;
}

CameraTriggerGameObject::~CameraTriggerGameObject() {
    EffectGameObject::~EffectGameObject(this);
    operator delete(this);
}