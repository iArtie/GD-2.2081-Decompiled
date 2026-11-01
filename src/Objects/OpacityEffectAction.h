#pragma once

class OpacityEffectAction {
public:
    void step(float delta);

private:
    bool m_finished = false;
    bool m_disabled = false;
    float m_deltaTime = 0.0f;
    float m_duration = 0.0f;
    float m_deltaTimeRelated = 0.0f;
    float m_fromValue = 0.0f;
    float m_toValue = 0.0f;
    float m_currentValue = 0.0f;
};