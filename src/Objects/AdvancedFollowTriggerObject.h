#pragma once

#include <string>

class GJBaseGameLayer;

class AdvancedFollowTriggerObject {
public:
    static AdvancedFollowTriggerObject* create(const char* frame);
    int getAdvancedFollowID();
    std::string getSaveString(GJBaseGameLayer* layer);
    ~AdvancedFollowTriggerObject();

private:
    int m_centerGroupID;
    int m_targetPlayer1;
    int m_targetPlayer2;
    int m_followCPP;
    int m_targetGroupID;
    float m_delay;
    float m_delayVariance;
    float m_maxSpeed;
    float m_maxSpeedVariance;
    float m_friction;
    float m_frictionVariance;
    int m_xOnly;
    int m_yOnly;
    float m_maxRange;
    float m_maxRangeVariance;
    float m_property310;
    float m_property311;
    float m_acceleration;
    float m_accelerationVariance;
    float m_property312;
    float m_property313;
    float m_property314;
    float m_property315;
    float m_steerForce;
    float m_steerForceVariance;
    int m_steerForceLowEnabled;
    float m_steerForceLow;
    float m_steerForceLowVariance;
    int m_steerForceHighEnabled;
    float m_steerForceHigh;
    float m_steerForceHighVariance;
    float m_speedRangeLow;
    float m_speedRangeLowVariance;
    float m_speedRangeHigh;
    float m_speedRangeHighVariance;
    float m_startSpeed;
    float m_startSpeedVariance;
    int m_startSpeedReference;
    float m_startDirection;
    float m_startDirectionVariance;
    int m_startDirectionReference;
    float m_breakForce;
    float m_breakForceVariance;
    float m_breakAngle;
    float m_breakAngleVariance;
    float m_breakSteerForce;
    float m_breakSteerForceVariance;
    float m_breakSteerSpeedLimit;
    float m_breakSteerSpeedLimitVariance;
    int m_targetDirection;
    int m_ignoreDisabled;
    int m_rotateDirection;
    float m_rotationOffset;
    float m_nearAcceleration;
    float m_nearAccelerationVariance;
    float m_nearDistance;
    float m_nearDistanceVariance;
    float m_nearFriction;
    float m_nearFrictionVariance;
    float m_easing;
    float m_easingVariance;
    float m_rotateEasing;
    float m_rotateDeadZ;
    int m_priority;
    int m_maxRangeReference;
    int m_followMode;
    int m_exclusive;
    int m_startMode;
};