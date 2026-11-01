#pragma once

#include <string>

class GJBaseGameLayer;

class KeyframeAnimTriggerObject : public EffectGameObject {
public:
    static KeyframeAnimTriggerObject* create();
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool init() override;
    virtual ~KeyframeAnimTriggerObject();

    float m_timeMod;
    float m_positionXMod;
    float m_positionYMod;
    float m_rotationMod;
    float m_scaleXMod;
    float m_scaleYMod;
};