#pragma once

#include "EffectGameObject.h"
#include <string>
#include <set>

class GJBaseGameLayer;

class EventLinkTrigger : public EffectGameObject {
public:
    static EventLinkTrigger* create();
    
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool init() override;
    
    virtual ~EventLinkTrigger();

private:
    int m_targetGroupID;
    int m_extraID;
    int m_extraID2;
    bool m_resetRemap;
    std::set<int> m_eventIDs;
};