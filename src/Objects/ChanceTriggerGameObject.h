#pragma once

#include "Objects/EffectGameObject.h"
#include <vector>

struct ChanceObject {
    int m_groupID;
    int m_oldGroupID;
};

class ChanceTriggerGameObject : public EffectGameObject {
public:
    bool containsTargetID(int id);
    void editChanceObject(int oldID, int newID);
    void getTargetIDs(std::vector<int>* ids);
    void revertChanceRemap();

    ~ChanceTriggerGameObject();

private:
    std::vector<ChanceObject> m_chanceObjects;
};