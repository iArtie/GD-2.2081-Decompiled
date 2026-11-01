#pragma once

#include "Platform/cocos2d.h"
#include "Objects/EffectGameObject.h"
#include "Utils/GJEnums.h"
#include <string>

class GJBaseGameLayer;

class TriggerControlGameObject : public EffectGameObject {
public:
    static TriggerControlGameObject* create(const char* frame);
    bool init(const char* frame) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    void updateTriggerControlFrame();
    virtual ~TriggerControlGameObject();

protected:
    std::string m_triggerControlFrame;
    GJActionCommand m_customTriggerValue = GJActionCommand::Stop;
    int m_targetGroupID = 0;
    int m_targetControlID = 0;
};