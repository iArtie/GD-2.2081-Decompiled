#include "Objects/OpacityEffectAction.h"

void OpacityEffectAction::step(float delta)
{
    if (!m_finished && !m_disabled)
    {
        float value = delta + m_deltaTime;
        float value2 = value + m_deltaTimeRelated;
        m_deltaTime = value;

        float value3;
        if (value2 < m_duration)
        {
            if (value2 <= 0.0f)
                value3 = 0.0f;
            else
                value3 = value2 / m_duration;
        }
        else
        {
            value3 = 1.0f;
        }

        float value4 = m_toValue - m_fromValue;
        m_finished = value2 >= m_duration;
        m_currentValue = m_fromValue + (value4 * value3);
    }
}