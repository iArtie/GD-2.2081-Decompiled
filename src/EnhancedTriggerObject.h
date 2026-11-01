#pragma once

#include "EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

class EnhancedTriggerObject : public EffectGameObject {
public:
    static EnhancedTriggerObject* create(const char* frame);
    
    std::string getSaveString(GJBaseGameLayer* layer) override;
    
    virtual ~EnhancedTriggerObject();

private:
    int m_minXID;
    int m_minYID;
    int m_maxXID;
    int m_maxYID;
};