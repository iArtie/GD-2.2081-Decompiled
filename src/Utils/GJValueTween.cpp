#include "Utils/GJValueTween.h"
#include "Utils/GameToolbox.h"

void GJValueTween::step(float delta) {
    if (m_finished || m_disabled) {
        return;
    }
    
    m_deltaTime += delta;
    float progress = (m_deltaTime < m_duration) ? (m_deltaTime / m_duration) : 1.0f;
    
    if (m_easingType > 0) {
        progress = GameToolbox::getEasedValue(progress, m_easingType, m_easingRate);
    }
    
    float range = m_toValue - m_fromValue;
    m_currentValue = m_fromValue + range * progress;
    m_finished = (m_deltaTime >= m_duration);
}