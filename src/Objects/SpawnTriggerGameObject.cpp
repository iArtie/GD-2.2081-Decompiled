#include "Objects/SpawnTriggerGameObject.h"
#include "Layers/GJBaseGameLayer.h"

void SpawnTriggerGameObject::addRemap(int oldID, int newID) {
    if (oldID == newID)
        return;

    for (auto& obj : m_remapObjects) {
        if (obj.m_groupID == oldID && obj.m_chance == newID)
            return;
    }

    m_remapObjects.emplace_back(oldID, newID);
}

void SpawnTriggerGameObject::changeRemap(int oldID, int newID, bool reverse) {
    if (m_remapObjects.empty())
        return;

    for (auto& obj : m_remapObjects) {
        if (reverse) {
            if (obj.m_groupID == oldID && obj.m_chance != newID)
                obj.m_groupID = newID;
        } else {
            if (obj.m_chance == oldID && obj.m_groupID != newID)
                obj.m_chance = newID;
        }
    }

    std::set<std::pair<int, int>> seen;
    std::vector<ChanceObject> unique;
    for (auto& obj : m_remapObjects) {
        std::pair<int, int> key(obj.m_groupID, obj.m_chance);
        if (seen.insert(key).second)
            unique.push_back(obj);
    }
    m_remapObjects = unique;
}

SpawnTriggerGameObject* SpawnTriggerGameObject::create() {
    SpawnTriggerGameObject* returnVal = new SpawnTriggerGameObject();
    if (returnVal->init()) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

ChanceObject* SpawnTriggerGameObject::getRemapWithSource(int id) {
    for (auto& obj : m_remapObjects) {
        if (obj.m_groupID == id)
            return &obj;
    }
    return nullptr;
}

ChanceObject* SpawnTriggerGameObject::getRemapWithTarget(int id) {
    for (auto& obj : m_remapObjects) {
        if (obj.m_chance == id)
            return &obj;
    }
    return nullptr;
}

std::string SpawnTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    if (m_targetGroupID) {
        saveString += ",";
        saveString += std::to_string(51);
        saveString += ",";
        saveString += std::to_string(m_targetGroupID);
    }
    if (m_spawnDelay != 0.0f) {
        saveString += ",";
        saveString += std::to_string(63);
        saveString += ",";
        saveString += std::to_string(m_spawnDelay);
    }
    if (m_delayRange != 0.0f) {
        saveString += ",";
        saveString += std::to_string(556);
        saveString += ",";
        saveString += std::to_string(m_delayRange);
    }
    if (m_previewDisable) {
        saveString += ",";
        saveString += std::to_string(102);
        saveString += ",";
        saveString += std::to_string(m_previewDisable);
    }
    if (m_spawnOrdered) {
        saveString += ",";
        saveString += std::to_string(441);
        saveString += ",";
        saveString += std::to_string(m_spawnOrdered);
    }
    if (m_resetRemap) {
        saveString += ",";
        saveString += std::to_string(581);
        saveString += ",";
        saveString += std::to_string(m_resetRemap);
    }
    if (!m_remapObjects.empty()) {
        saveString += ",";
        saveString += std::to_string(442);
        saveString += ",";
        bool first = true;
        for (auto& obj : m_remapObjects) {
            if (!first)
                saveString += ".";
            saveString += std::to_string(obj.m_groupID);
            saveString += ".";
            saveString += std::to_string(obj.m_chance);
            first = false;
        }
    }
    return saveString;
}

bool SpawnTriggerGameObject::init() {
    return EffectGameObject::init("edit_eSpawnBtn_001.png");
}

void SpawnTriggerGameObject::removeRemap(int oldID, int newID) {
    for (auto it = m_remapObjects.begin(); it != m_remapObjects.end(); ++it) {
        if (it->m_groupID == oldID && it->m_chance == newID) {
            m_remapObjects.erase(it);
            return;
        }
    }
}

void SpawnTriggerGameObject::updateRemapKeys(const std::vector<int>* remapKeys) {
    if (m_remapKey > 0) {
        m_remapKeys.clear();
        m_remapKeys.push_back(m_remapKey);
        if (!m_resetRemap) {
            m_remapKeys.insert(m_remapKeys.end(), remapKeys->begin(), remapKeys->end());
            if (!remapKeys->empty() && !m_resetRemap)
                return;
        }
        m_remapKeys.push_back(-m_uniqueID);
    } else {
        if (m_resetRemap || remapKeys == &m_remapKeys)
            return;
        m_remapKeys = *remapKeys;
    }
}

SpawnTriggerGameObject::~SpawnTriggerGameObject() {
}