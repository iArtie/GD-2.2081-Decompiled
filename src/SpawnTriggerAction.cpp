#include "SpawnTriggerAction.h"

bool SpawnTriggerAction::isFinished() {
    return m_finished;
}

void SpawnTriggerAction::step(float dt) {
    if (!m_finished && !m_disabled) {
        m_deltaTime = dt + m_deltaTime;
        m_finished = m_deltaTime >= m_duration;
    }
}