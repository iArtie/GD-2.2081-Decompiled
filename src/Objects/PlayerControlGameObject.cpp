#include "Objects/PlayerControlGameObject.h"
#include "Layers/GJBaseGameLayer.h"

PlayerControlGameObject* PlayerControlGameObject::create()
{
    PlayerControlGameObject* returnVal = new PlayerControlGameObject();
    if (returnVal->init())
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool PlayerControlGameObject::init()
{
    return EffectGameObject::init("edit_ePlayerControlBtn_001.png");
}

std::string PlayerControlGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string result = EffectGameObject::getSaveString(layer);

    if (m_targetPlayer1)
    {
        result += ",";
        result += "138";
        result += ",";
        result += std::to_string((int)m_targetPlayer1);
    }

    if (m_targetPlayer2)
    {
        result += ",";
        result += "200";
        result += ",";
        result += std::to_string((int)m_targetPlayer2);
    }

    if (m_stopJump)
    {
        result += ",";
        result += "540";
        result += ",";
        result += std::to_string((int)m_stopJump);
    }

    if (m_stopMove)
    {
        result += ",";
        result += "541";
        result += ",";
        result += std::to_string((int)m_stopMove);
    }

    if (m_stopRotation)
    {
        result += ",";
        result += "542";
        result += ",";
        result += std::to_string((int)m_stopRotation);
    }

    if (m_stopSlide)
    {
        result += ",";
        result += "543";
        result += ",";
        result += std::to_string((int)m_stopSlide);
    }

    return result;
}

PlayerControlGameObject::~PlayerControlGameObject()
{
}