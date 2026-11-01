#include "Objects/AdvancedFollowTriggerObject.h"
#include "Layers/GJBaseGameLayer.h"
#include <string>

AdvancedFollowTriggerObject* AdvancedFollowTriggerObject::create(const char* frame)
{
    AdvancedFollowTriggerObject* pRet = new AdvancedFollowTriggerObject();
    if (pRet && pRet->init(frame))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

int AdvancedFollowTriggerObject::getAdvancedFollowID()
{
    if (m_targetPlayer1)
        return -1;
    if (m_targetPlayer2)
        return -2;
    if (m_followCPP)
        return -3;
    return 0;
}

std::string AdvancedFollowTriggerObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string saveString = "";
    
    // Get base save string from EffectGameObject
    // This would call the parent class's getSaveString
    
    if (m_targetGroupID)
    {
        saveString += ",51," + std::to_string(m_targetGroupID);
        if (m_centerGroupID)
        {
            saveString += ",71," + std::to_string(m_centerGroupID);
        }
    }
    else if (m_centerGroupID)
    {
        saveString += ",71," + std::to_string(m_centerGroupID);
    }
    
    if (m_targetPlayer1)
    {
        saveString += ",138," + std::to_string(m_targetPlayer1);
    }
    
    if (m_targetPlayer2)
    {
        saveString += ",200," + std::to_string(m_targetPlayer2);
    }
    
    if (m_followCPP)
    {
        saveString += ",201," + std::to_string(m_followCPP);
    }
    
    if (m_delay != 0.0f)
    {
        saveString += ",292," + std::to_string(m_delay);
    }
    
    if (m_delayVariance != 0.0f)
    {
        saveString += ",293," + std::to_string(m_delayVariance);
    }
    
    if (m_maxSpeed != 0.0f)
    {
        saveString += ",298," + std::to_string(m_maxSpeed);
    }
    
    if (m_maxSpeedVariance != 0.0f)
    {
        saveString += ",299," + std::to_string(m_maxSpeedVariance);
    }
    
    if (m_friction != 0.0f)
    {
        saveString += ",558," + std::to_string(m_friction);
    }
    
    if (m_frictionVariance != 0.0f)
    {
        saveString += ",559," + std::to_string(m_frictionVariance);
    }
    
    if (m_xOnly)
    {
        saveString += ",306," + std::to_string(m_xOnly);
    }
    
    if (m_yOnly)
    {
        saveString += ",307," + std::to_string(m_yOnly);
    }
    
    if (m_maxRange != 0.0f)
    {
        saveString += ",308," + std::to_string(m_maxRange);
    }
    
    if (m_maxRangeVariance != 0.0f)
    {
        saveString += ",309," + std::to_string(m_maxRangeVariance);
    }
    
    if (m_property310 != 0.0f)
    {
        saveString += ",310," + std::to_string(m_property310);
    }
    
    if (m_property311 != 0.0f)
    {
        saveString += ",311," + std::to_string(m_property311);
    }
    
    if (m_acceleration != 0.0f)
    {
        saveString += ",334," + std::to_string(m_acceleration);
    }
    
    if (m_accelerationVariance != 0.0f)
    {
        saveString += ",335," + std::to_string(m_accelerationVariance);
    }
    
    if (m_property312 != 0.0f)
    {
        saveString += ",312," + std::to_string(m_property312);
    }
    
    if (m_property313 != 0.0f)
    {
        saveString += ",313," + std::to_string(m_property313);
    }
    
    if (m_property314 != 0.0f)
    {
        saveString += ",314," + std::to_string(m_property314);
    }
    
    if (m_property315 != 0.0f)
    {
        saveString += ",315," + std::to_string(m_property315);
    }
    
    if (m_steerForce != 0.0f)
    {
        saveString += ",316," + std::to_string(m_steerForce);
    }
    
    if (m_steerForceVariance != 0.0f)
    {
        saveString += ",317," + std::to_string(m_steerForceVariance);
    }
    
    if (m_steerForceLowEnabled)
    {
        saveString += ",337," + std::to_string(m_steerForceLowEnabled);
    }
    
    if (m_steerForceLow != 0.0f)
    {
        saveString += ",318," + std::to_string(m_steerForceLow);
    }
    
    if (m_steerForceLowVariance != 0.0f)
    {
        saveString += ",319," + std::to_string(m_steerForceLowVariance);
    }
    
    if (m_steerForceHighEnabled)
    {
        saveString += ",338," + std::to_string(m_steerForceHighEnabled);
    }
    
    if (m_steerForceHigh != 0.0f)
    {
        saveString += ",320," + std::to_string(m_steerForceHigh);
    }
    
    if (m_steerForceHighVariance != 0.0f)
    {
        saveString += ",321," + std::to_string(m_steerForceHighVariance);
    }
    
    if (m_speedRangeLow != 0.0f)
    {
        saveString += ",322," + std::to_string(m_speedRangeLow);
    }
    
    if (m_speedRangeLowVariance != 0.0f)
    {
        saveString += ",323," + std::to_string(m_speedRangeLowVariance);
    }
    
    if (m_speedRangeHigh != 0.0f)
    {
        saveString += ",324," + std::to_string(m_speedRangeHigh);
    }
    
    if (m_speedRangeHighVariance != 0.0f)
    {
        saveString += ",325," + std::to_string(m_speedRangeHighVariance);
    }
    
    if (m_startSpeed != 0.0f)
    {
        saveString += ",300," + std::to_string(m_startSpeed);
    }
    
    if (m_startSpeedVariance != 0.0f)
    {
        saveString += ",301," + std::to_string(m_startSpeedVariance);
    }
    
    if (m_startSpeedReference)
    {
        saveString += ",560," + std::to_string(m_startSpeedReference);
    }
    
    if (m_startDirection != 0.0f)
    {
        saveString += ",563," + std::to_string(m_startDirection);
    }
    
    if (m_startDirectionVariance != 0.0f)
    {
        saveString += ",564," + std::to_string(m_startDirectionVariance);
    }
    
    if (m_startDirectionReference)
    {
        saveString += ",565," + std::to_string(m_startDirectionReference);
    }
    
    if (m_breakForce != 0.0f)
    {
        saveString += ",326," + std::to_string(m_breakForce);
    }
    
    if (m_breakForceVariance != 0.0f)
    {
        saveString += ",327," + std::to_string(m_breakForceVariance);
    }
    
    if (m_breakAngle != 0.0f)
    {
        saveString += ",328," + std::to_string(m_breakAngle);
    }
    
    if (m_breakAngleVariance != 0.0f)
    {
        saveString += ",329," + std::to_string(m_breakAngleVariance);
    }
    
    if (m_breakSteerForce != 0.0f)
    {
        saveString += ",330," + std::to_string(m_breakSteerForce);
    }
    
    if (m_breakSteerForceVariance != 0.0f)
    {
        saveString += ",331," + std::to_string(m_breakSteerForceVariance);
    }
    
    if (m_breakSteerSpeedLimit != 0.0f)
    {
        saveString += ",332," + std::to_string(m_breakSteerSpeedLimit);
    }
    
    if (m_breakSteerSpeedLimitVariance != 0.0f)
    {
        saveString += ",333," + std::to_string(m_breakSteerSpeedLimitVariance);
    }
    
    if (m_targetDirection)
    {
        saveString += ",305," + std::to_string(m_targetDirection);
    }
    
    if (m_ignoreDisabled)
    {
        saveString += ",336," + std::to_string(m_ignoreDisabled);
    }
    
    if (m_rotateDirection)
    {
        saveString += ",339," + std::to_string(m_rotateDirection);
    }
    
    if (m_rotationOffset != 0.0f)
    {
        saveString += ",340," + std::to_string(m_rotationOffset);
    }
    
    if (m_nearAcceleration != 0.0f)
    {
        saveString += ",357," + std::to_string(m_nearAcceleration);
    }
    
    if (m_nearAccelerationVariance != 0.0f)
    {
        saveString += ",358," + std::to_string(m_nearAccelerationVariance);
    }
    
    if (m_nearDistance != 0.0f)
    {
        saveString += ",359," + std::to_string(m_nearDistance);
    }
    
    if (m_nearDistanceVariance != 0.0f)
    {
        saveString += ",360," + std::to_string(m_nearDistanceVariance);
    }
    
    if (m_nearFriction != 0.0f)
    {
        saveString += ",561," + std::to_string(m_nearFriction);
    }
    
    if (m_nearFrictionVariance != 0.0f)
    {
        saveString += ",562," + std::to_string(m_nearFrictionVariance);
    }
    
    if (m_easing != 0.0f)
    {
        saveString += ",361," + std::to_string(m_easing);
    }
    
    if (m_easingVariance != 0.0f)
    {
        saveString += ",362," + std::to_string(m_easingVariance);
    }
    
    if (m_rotateEasing != 0.0f)
    {
        saveString += ",363," + std::to_string(m_rotateEasing);
    }
    
    if (m_rotateDeadZ != 0.0f)
    {
        saveString += ",364," + std::to_string(m_rotateDeadZ);
    }
    
    if (m_priority)
    {
        saveString += ",365," + std::to_string(m_priority);
    }
    
    if (m_maxRangeReference)
    {
        saveString += ",366," + std::to_string(m_maxRangeReference);
    }
    
    if (m_followMode)
    {
        saveString += ",367," + std::to_string(m_followMode);
    }
    
    if (m_exclusive)
    {
        saveString += ",571," + std::to_string(m_exclusive);
    }
    
    if (m_startMode)
    {
        saveString += ",572," + std::to_string(m_startMode);
    }
    
    return saveString;
}

AdvancedFollowTriggerObject::~AdvancedFollowTriggerObject()
{
    // Destructor would call parent destructors
}