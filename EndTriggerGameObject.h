#pragma once

#include "EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

class EndTriggerGameObject : public EffectGameObject {
public:
    static EndTriggerGameObject* create();
    
    std::string getSaveString(GJBaseGameLayer* layer);
    bool init() override;
    
    virtual ~EndTriggerGameObject();

private:
    int m_targetGroupID;
    int m_centerGroupID;
    bool m_noEffects;
    bool m_noSFX;
    bool m_instant;
};