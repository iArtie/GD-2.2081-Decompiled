#pragma once

class SpawnTriggerAction {
public:
    bool isFinished();
    void step(float dt);

protected:
    bool m_finished = false;
    bool m_disabled = false;
    double m_duration = 0.0;
    double m_deltaTime = 0.0;
};