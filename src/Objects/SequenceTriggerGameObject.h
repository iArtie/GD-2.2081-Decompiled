#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Objects/ChanceTriggerGameObject.h"
#include <string>
#include <vector>
#include <unordered_map>

struct ChanceObject {
    int m_groupID = 0;
    int m_oldGroupID = 0;
    int m_chance = 0;
    int m_unk00c = 0;
};

struct SequenceState {
    std::unordered_map<int, int> m_sequenceIndices;
    std::unordered_map<int, float> m_sequenceTimes;
};

class GJBaseGameLayer;

class SequenceTriggerGameObject : public ChanceTriggerGameObject {
public:
    static SequenceTriggerGameObject* create();
    bool init() override;

    void addCount(int index, int count);
    void addTarget(int groupID, int count);
    void deleteTarget(int index);
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool reorderTarget(int index, bool left);
    void resetObject() override;
    void updateSequenceTotalCount();

    ~SequenceTriggerGameObject();

protected:
    SequenceState m_sequenceState;
    std::vector<ChanceObject> m_chanceObjects;
    int m_sequenceTotalCount = 0;
    float m_minInt = 0.0f;
    int m_sequenceMode = 0;
    int m_resetMode = 0;
    float m_reset = 0.0f;
    int m_uniqueRemap = 0;
};