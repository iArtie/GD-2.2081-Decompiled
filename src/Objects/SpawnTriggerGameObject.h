#pragma once

#include "Platform/cocos2d.h"
#include "Objects/EffectGameObject.h"
#include "ChanceObject.h"
#include <string>
#include <vector>

class GJBaseGameLayer;

class SpawnTriggerGameObject : public EffectGameObject {
public:
    static SpawnTriggerGameObject* create();
    bool init() override;
    void addRemap(int oldID, int newID);
    void changeRemap(int oldID, int newID, bool reverse);
    ChanceObject* getRemapWithSource(int id);
    ChanceObject* getRemapWithTarget(int id);
    std::string getSaveString(GJBaseGameLayer* layer) override;
    void removeRemap(int oldID, int newID);
    void updateRemapKeys(const std::vector<int>* remapKeys);
    virtual ~SpawnTriggerGameObject();

protected:
    std::vector<ChanceObject> m_remapObjects;
    int m_remapKey = 0;
    std::vector<int> m_remapKeys;
    bool m_resetRemap = false;
    int m_uniqueID = 0;
    int m_targetGroupID = 0;
    float m_spawnDelay = 0.0f;
    float m_delayRange = 0.0f;
    bool m_previewDisable = false;
    bool m_spawnOrdered = false;
};