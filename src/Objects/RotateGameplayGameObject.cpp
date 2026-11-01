#include "Objects/RotateGameplayGameObject.h"

RotateGameplayGameObject* RotateGameplayGameObject::create()
{
    RotateGameplayGameObject* returnVal = new RotateGameplayGameObject();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool RotateGameplayGameObject::init()
{
    return EffectGameObject::init("edit_eGameRotBtn_001.png");
}

std::string RotateGameplayGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string result = EffectGameObject::getSaveString(layer);

    if (m_editVelocity)
    {
        result += ",";
        result += std::to_string(169);
        result += ",";
        result += std::to_string(m_editVelocity);
    }

    if (m_overrideVelocity)
    {
        result += ",";
        result += std::to_string(584);
        result += ",";
        result += std::to_string(m_overrideVelocity);
    }

    if (m_velocityModX != 0.0)
    {
        result += ",";
        result += std::to_string(582);
        result += ",";
        result += std::to_string(m_velocityModX);
    }

    if (m_velocityModY != 0.0)
    {
        result += ",";
        result += std::to_string(583);
        result += ",";
        result += std::to_string(m_velocityModY);
    }

    if (m_moveDirection)
    {
        result += ",";
        result += std::to_string(166);
        result += ",";
        result += std::to_string(m_moveDirection);
    }

    if (m_groundDirection)
    {
        result += ",";
        result += std::to_string(167);
        result += ",";
        result += std::to_string(m_groundDirection);
    }

    if (m_changeChannel)
    {
        result += ",";
        result += std::to_string(171);
        result += ",";
        result += std::to_string(m_changeChannel);
    }

    if (m_channelOnly)
    {
        result += ",";
        result += std::to_string(172);
        result += ",";
        result += std::to_string(m_channelOnly);
    }

    if (m_targetChannelID)
    {
        result += ",";
        result += std::to_string(173);
        result += ",";
        result += std::to_string(m_targetChannelID);
    }

    if (m_instantOffset)
    {
        result += ",";
        result += std::to_string(368);
        result += ",";
        result += std::to_string(m_instantOffset);
    }

    if (m_dontSlide)
    {
        result += ",";
        result += std::to_string(585);
        result += ",";
        result += std::to_string(m_dontSlide);
    }

    return result;
}

void RotateGameplayGameObject::updateGameplayRotation()
{
    determineSlopeDirection();

    switch (m_slopeDirection)
    {
        case 1:
            m_moveDirection = 1;
            m_groundDirection = 4;
            break;
        case 2:
            m_moveDirection = 2;
            m_groundDirection = 3;
            break;
        case 3:
            m_moveDirection = 1;
            m_groundDirection = 3;
            break;
        case 4:
            m_moveDirection = 3;
            m_groundDirection = 2;
            break;
        case 5:
            m_moveDirection = 4;
            m_groundDirection = 1;
            break;
        case 6:
            m_moveDirection = 3;
            m_groundDirection = 1;
            break;
        case 7:
            m_moveDirection = 4;
            m_groundDirection = 2;
            break;
        default:
            m_moveDirection = 2;
            m_groundDirection = 4;
            break;
    }
}

void RotateGameplayGameObject::updateStartValues()
{
    GameObject::updateStartValues();
    updateGameplayRotation();
}

RotateGameplayGameObject::~RotateGameplayGameObject()
{
}