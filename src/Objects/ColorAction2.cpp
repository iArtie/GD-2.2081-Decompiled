#include "Objects/ColorAction2.h"
#include "Utils/GameToolbox.h"

void ColorAction2::resetAction() {
    m_stepFinished = false;
    m_paused = false;
    m_deltaTime = 0.0f;
}

void ColorAction2::step(float dt) {
    if (m_stepFinished || m_paused) {
        return;
    }

    float newTime = dt + m_deltaTime;
    float intervalTime = newTime + m_stepInterval;
    m_deltaTime = newTime;

    if (intervalTime < m_duration) {
        if (intervalTime <= 0.0f) {
            m_color = m_fromColor;
            m_currentOpacity = m_fromOpacity;
        } else {
            float progress = intervalTime / m_duration;
            if (progress < 1.0f && progress > 0.0f) {
                m_color = GameToolbox::multipliedColorValue(
                    m_fromColor,
                    m_toColor,
                    progress
                );
                m_currentOpacity = m_fromOpacity + (progress * (m_toOpacity - m_fromOpacity));
            } else {
                m_color = m_fromColor;
                m_currentOpacity = m_fromOpacity;
            }
        }
        m_stepFinished = (intervalTime >= m_duration);
    } else {
        m_color = m_toColor;
        m_currentOpacity = m_toOpacity;
        m_stepFinished = true;
    }
}