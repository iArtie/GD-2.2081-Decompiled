#include "SequenceTriggerGameObject.h"
#include "GJBaseGameLayer.h"
#include "EffectGameObject.h"

void SequenceTriggerGameObject::addCount(int index, int count) {
    if (index < 0) {
        return;
    }

    if (index < (int)m_chanceObjects.size() && !m_chanceObjects.empty()) {
        ChanceObject& obj = m_chanceObjects[index];
        int newChance = obj.m_chance + count;
        if (newChance <= 0) {
            newChance = 1;
        }
        obj.m_chance = newChance;
        m_sequenceTotalCount = -1;
    }
}

void SequenceTriggerGameObject::addTarget(int groupID, int count) {
    int finalCount = (count <= 0) ? 1 : count;

    ChanceObject newObj;
    newObj.m_groupID = groupID;
    newObj.m_oldGroupID = groupID;
    newObj.m_chance = finalCount;
    newObj.m_unk00c = 0;

    m_chanceObjects.push_back(newObj);
    m_sequenceTotalCount = -1;
}

SequenceTriggerGameObject* SequenceTriggerGameObject::create() {
    SequenceTriggerGameObject* obj = new SequenceTriggerGameObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

void SequenceTriggerGameObject::deleteTarget(int index) {
    if (index < 0) {
        return;
    }

    if (index < (int)m_chanceObjects.size() && !m_chanceObjects.empty()) {
        m_chanceObjects.erase(m_chanceObjects.begin() + index);
        m_sequenceTotalCount = -1;
    }
}

std::string SequenceTriggerGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EffectGameObject::getSaveString(layer);

    if (m_minInt != 0.0f) {
        result += ",";
        result += "437,";
        result += ",";
        result += std::to_string(m_minInt);
    }

    if (m_sequenceMode != 0) {
        result += ",";
        result += "436,";
        result += ",";
        result += std::to_string(m_sequenceMode);
    }

    if (m_resetMode != 0) {
        result += ",";
        result += "439,";
        result += ",";
        result += std::to_string(m_resetMode);
    }

    if (m_reset != 0.0f) {
        result += ",";
        result += "438,";
        result += ",";
        result += std::to_string(m_reset);
    }

    if (m_uniqueRemap != 0) {
        result += ",";
        result += "505,";
        result += ",";
        result += std::to_string(m_uniqueRemap);
    }

    if (!m_chanceObjects.empty()) {
        result += ",";
        result += "435,";
        result += ",";
        for (size_t i = 0; i < m_chanceObjects.size(); i++) {
            if (i > 0) {
                result += ".";
            }
            result += std::to_string(m_chanceObjects[i].m_groupID);
            result += ".";
            result += std::to_string(m_chanceObjects[i].m_chance);
        }
    }

    return result;
}

bool SequenceTriggerGameObject::init() {
    return ChanceTriggerGameObject::init("edit_eSequenceBtn_001.png");
}

bool SequenceTriggerGameObject::reorderTarget(int index, bool left) {
    int count = (int)m_chanceObjects.size();
    if (count <= index) {
        return false;
    }

    if (!left && index == count - 1) {
        return false;
    }

    if (m_chanceObjects.empty() || (left && index == 0)) {
        return false;
    }

    int offset = left ? -1 : 1;
    ChanceObject& current = m_chanceObjects[index];
    ChanceObject& adjacent = m_chanceObjects[index + offset];

    ChanceObject temp = current;
    current = adjacent;
    adjacent = temp;

    return true;
}

void SequenceTriggerGameObject::resetObject() {
    EnhancedGameObject::resetObject();
    m_sequenceState.m_sequenceIndices.clear();
    m_sequenceState.m_sequenceTimes.clear();
}

void SequenceTriggerGameObject::updateSequenceTotalCount() {
    m_sequenceTotalCount = 0;
    for (const ChanceObject& obj : m_chanceObjects) {
        m_sequenceTotalCount += obj.m_chance;
    }
}

SequenceTriggerGameObject::~SequenceTriggerGameObject() {
}