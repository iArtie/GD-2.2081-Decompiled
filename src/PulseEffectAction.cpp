#include "PulseEffectAction.h"

bool PulseEffectAction::isFinished()
{
    return (m_deltaTime + m_startTime) >= (m_fadeInTime + m_holdTime + m_fadeOutTime);
}

void PulseEffectAction::step(float delta)
{
    if (m_disabled)
        return;

    m_deltaTime += delta;
    m_currentValue = valueForDelta(m_deltaTime + m_startTime, m_fadeInTime, m_holdTime, m_fadeOutTime);
}

float PulseEffectAction::valueForDelta(float delta, float fadeInTime, float holdTime, float fadeOutTime)
{
    if (delta < fadeInTime)
        return delta / fadeInTime;

    if (delta < fadeInTime + holdTime)
        return 1.0f;

    float fadeOutElapsed = delta - fadeInTime - holdTime;
    if (fadeOutElapsed < fadeOutTime)
        return 1.0f - fadeOutElapsed / fadeOutTime;

    return 0.0f;
}