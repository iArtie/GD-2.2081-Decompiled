#include "SpawnParticleGameObject.h"
#include "GJBaseGameLayer.h"

SpawnParticleGameObject* SpawnParticleGameObject::create() {
    SpawnParticleGameObject* returnVal = new SpawnParticleGameObject();
    if (returnVal->init()) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

std::string SpawnParticleGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
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
    if (m_offset.x != 0.0) {
        saveString += ",";
        saveString += std::to_string(547);
        saveString += ",";
        saveString += std::to_string(m_offset.x);
    }
    if (m_offset.y != 0.0) {
        saveString += ",";
        saveString += std::to_string(548);
        saveString += ",";
        saveString += std::to_string(m_offset.y);
    }
    if (m_offsetVariance.x != 0.0) {
        saveString += ",";
        saveString += std::to_string(549);
        saveString += ",";
        saveString += std::to_string(m_offsetVariance.x);
    }
    if (m_offsetVariance.y != 0.0) {
        saveString += ",";
        saveString += std::to_string(550);
        saveString += ",";
        saveString += std::to_string(m_offsetVariance.y);
    }
    if (m_matchRotation) {
        saveString += ",";
        saveString += std::to_string(551);
        saveString += ",";
        saveString += std::to_string(m_matchRotation);
    }
    if (m_rotation != 0.0) {
        saveString += ",";
        saveString += std::to_string(552);
        saveString += ",";
        saveString += std::to_string(m_rotation);
    }
    if (m_rotationVariance != 0.0) {
        saveString += ",";
        saveString += std::to_string(553);
        saveString += ",";
        saveString += std::to_string(m_rotationVariance);
    }
    if (m_scale != 0.0) {
        saveString += ",";
        saveString += std::to_string(554);
        saveString += ",";
        saveString += std::to_string(m_scale);
    }
    if (m_scaleVariance != 0.0) {
        saveString += ",";
        saveString += std::to_string(555);
        saveString += ",";
        saveString += std::to_string(m_scaleVariance);
    }
    return saveString;
}

bool SpawnParticleGameObject::init() {
    return EffectGameObject::init("edit_eSpawnParticleBtn_001.png");
}

SpawnParticleGameObject::~SpawnParticleGameObject() {
}