#include "ChanceTriggerGameObject.h"
#include "EffectGameObject.h"
#include <vector>

bool ChanceTriggerGameObject::containsTargetID(int id) {
    for (const auto& obj : m_chanceObjects) {
        if (obj.m_groupID == id) {
            return true;
        }
    }
    return false;
}

void ChanceTriggerGameObject::editChanceObject(int oldID, int newID) {
    for (auto& obj : m_chanceObjects) {
        if (obj.m_groupID == oldID) {
            obj.m_groupID = newID;
        }
    }
}

void ChanceTriggerGameObject::getTargetIDs(std::vector<int>* ids) {
    for (const auto& obj : m_chanceObjects) {
        ids->push_back(obj.m_groupID);
    }
}

void ChanceTriggerGameObject::revertChanceRemap() {
    for (auto& obj : m_chanceObjects) {
        obj.m_groupID = obj.m_oldGroupID;
    }
}

ChanceTriggerGameObject::~ChanceTriggerGameObject() {
    m_chanceObjects.clear();
    EffectGameObject::~EffectGameObject();
}