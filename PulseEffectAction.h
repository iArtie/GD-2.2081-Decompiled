#pragma once

class PulseEffectAction {
public:
    bool isFinished();
    void step(float delta);

private:
    static float valueForDelta(float delta, float fadeInTime, float holdTime, float fadeOutTime);

    float m_deltaTime = 0.0f;
    float m_startTime = 0.0f;
    float m_fadeInTime = 0.0f;
    float m_holdTime = 0.0f;
    float m_fadeOutTime = 0.0f;
    bool m_disabled = false;
    float m_currentValue = 0.0f;
};