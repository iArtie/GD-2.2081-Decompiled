#pragma once

#include <string>
#include <vector>

#include "ChanceTriggerGameObject.h"

class GJBaseGameLayer;

struct ChanceObject {
    int m_groupID = 0;
    int m_chance = 0;
};

class RandTriggerGameObject : public ChanceTriggerGameObject {
public:
    static RandTriggerGameObject* create();

    bool init() override;

    int getRandomGroupID();
    int getTotalChance();
    std::string getSaveString(GJBaseGameLayer* layer) override;

    virtual ~RandTriggerGameObject();

protected:
    std::vector<ChanceObject> m_chanceObjects;
    int m_activateGroup = 0;
};