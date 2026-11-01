#include "Objects/TransformTriggerGameObject.h"
#include "Layers/GJBaseGameLayer.h"

TransformTriggerGameObject* TransformTriggerGameObject::create(const char* frame) {
    TransformTriggerGameObject* returnVal = new TransformTriggerGameObject();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool TransformTriggerGameObject::init(const char* frame) {
    return EffectGameObject::init(frame);
}

std::string TransformTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);

    if (m_objectID != 2067)
        return saveString;

    if (m_targetGroupID) {
        saveString += ",";
        saveString += std::to_string(51);
        saveString += ",";
        saveString += std::to_string(m_targetGroupID);
    }
    if (m_centerGroupID) {
        saveString += ",";
        saveString += std::to_string(71);
        saveString += ",";
        saveString += std::to_string(m_centerGroupID);
    }
    if (m_duration != 0.0) {
        saveString += ",";
        saveString += std::to_string(10);
        saveString += ",";
        saveString += std::to_string(m_duration);
    }
    if (m_easingType) {
        saveString += ",";
        saveString += std::to_string(30);
        saveString += ",";
        saveString += std::to_string(m_easingType);
    }
    if (m_easingRate != 0.0) {
        saveString += ",";
        saveString += std::to_string(85);
        saveString += ",";
        saveString += std::to_string(m_easingRate);
    }
    if (m_objectScaleX != 0.0) {
        saveString += ",";
        saveString += std::to_string(150);
        saveString += ",";
        saveString += std::to_string(m_objectScaleX);
    }
    if (m_objectScaleY != 0.0) {
        saveString += ",";
        saveString += std::to_string(151);
        saveString += ",";
        saveString += std::to_string(m_objectScaleY);
    }
    if (m_property450 != 0.0) {
        saveString += ",";
        saveString += std::to_string(450);
        saveString += ",";
        saveString += std::to_string(m_property450);
    }
    if (m_property451 != 0.0) {
        saveString += ",";
        saveString += std::to_string(451);
        saveString += ",";
        saveString += std::to_string(m_property451);
    }
    if (m_divideX) {
        saveString += ",";
        saveString += std::to_string(153);
        saveString += ",";
        saveString += std::to_string(m_divideX);
    }
    if (m_divideY) {
        saveString += ",";
        saveString += std::to_string(154);
        saveString += ",";
        saveString += std::to_string(m_divideY);
    }
    if (m_onlyMove) {
        saveString += ",";
        saveString += std::to_string(133);
        saveString += ",";
        saveString += std::to_string(m_onlyMove);
    }
    if (m_relativeRotation) {
        saveString += ",";
        saveString += std::to_string(452);
        saveString += ",";
        saveString += std::to_string(m_relativeRotation);
    }
    if (m_relativeScale) {
        saveString += ",";
        saveString += std::to_string(577);
        saveString += ",";
        saveString += std::to_string(m_relativeScale);
    }
    return saveString;
}

TransformTriggerGameObject::~TransformTriggerGameObject() {
}