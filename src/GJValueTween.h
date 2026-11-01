#pragma once

class GJValueTween {
public:
    void step(float delta);

    bool m_finished;
    bool m_disabled;
    float m_duration;
    float m_deltaTime;
    int m_easingType;
    float m_easingRate;
    float m_fromValue;
    float m_toValue;
    float m_currentValue;
};