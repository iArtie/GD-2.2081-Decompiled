// EffectManagerState.cpp
#include "Utils/EffectManagerState.h"

EffectManagerState::~EffectManagerState() {
    for (auto& groupCmd : m_groupCommandObjects) {
        for (auto& keyframe : groupCmd.m_keyframes) {
            delete[] keyframe.m_spline1.m_x.m_start;
            delete[] keyframe.m_spline1.m_y.m_start;
            delete[] keyframe.m_spline1.m_b.m_start;
            delete[] keyframe.m_spline1.m_c.m_start;
            delete[] keyframe.m_spline1.m_d.m_start;
            delete[] keyframe.m_spline2.m_x.m_start;
            delete[] keyframe.m_spline2.m_y.m_start;
            delete[] keyframe.m_spline2.m_b.m_start;
            delete[] keyframe.m_spline2.m_c.m_start;
            delete[] keyframe.m_spline2.m_d.m_start;
        }
        delete[] groupCmd.m_keyframes.m_start;
    }

    for (auto& action : m_spawnTriggerActions) {
        delete[] action.m_remapKeys.m_start;
    }

    for (auto& action : m_toggleTriggerActions) {
        delete[] action.m_remapKeys.m_start;
    }

    for (auto& action : m_collisionTriggerActions) {
        delete[] action.m_remapKeys.m_start;
    }

    for (auto& action : m_touchToggleActions) {
        delete[] action.m_remapKeys.m_start;
    }
}