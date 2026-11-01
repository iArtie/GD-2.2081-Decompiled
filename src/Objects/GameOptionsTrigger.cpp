#include "Objects/GameOptionsTrigger.h"
#include "Layers/GJBaseGameLayer.h"
#include "Objects/EffectGameObject.h"

GameOptionsTrigger* GameOptionsTrigger::create(const char* frame)
{
    GameOptionsTrigger* instance = new GameOptionsTrigger();
    if (instance && instance->init(frame))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

std::string GameOptionsTrigger::getSaveString(GJBaseGameLayer* layer)
{
    std::string result;

    // Get base save string from EffectGameObject
    std::string baseString = EffectGameObject::getSaveString(this, layer);
    result += baseString;

    // Streak additive
    if (m_streakAdditive)
    {
        result += ",91," + std::to_string(m_streakAdditive);
    }

    // Unlink dual gravity
    if (m_unlinkDualGravity != Disabled)
    {
        result += ",92," + std::to_string(m_unlinkDualGravity);
    }

    // Hide ground
    if (m_hideGround != Disabled)
    {
        result += ",93," + std::to_string(m_hideGround);
    }

    // Hide player 1
    if (m_hideP1 != Disabled)
    {
        result += ",94," + std::to_string(m_hideP1);
    }

    // Hide player 2
    if (m_hideP2 != Disabled)
    {
        result += ",95," + std::to_string(m_hideP2);
    }

    // Disable player 1 controls
    if (m_disableP1Controls != Disabled)
    {
        result += ",97," + std::to_string(m_disableP1Controls);
    }

    // Disable player 2 controls
    if (m_disableP2Controls != Disabled)
    {
        result += ",99," + std::to_string(m_disableP2Controls);
    }

    // Hide MG
    if (m_hideMG != Disabled)
    {
        result += ",103," + std::to_string(m_hideMG);
    }

    // Hide attempts
    if (m_hideAttempts != Disabled)
    {
        result += ",106," + std::to_string(m_hideAttempts);
    }

    // Edit respawn time
    if (m_editRespawnTime != Disabled)
    {
        result += ",109," + std::to_string(m_editRespawnTime);
    }

    // Respawn time
    if (m_respawnTime != 0.0f)
    {
        result += ",110," + std::to_string(m_respawnTime);
    }

    // Audio on death
    if (m_audioOnDeath != Disabled)
    {
        result += ",111," + std::to_string(m_audioOnDeath);
    }

    // No death SFX
    if (m_noDeathSFX != Disabled)
    {
        result += ",112," + std::to_string(m_noDeathSFX);
    }

    // Boost slide
    if (m_boostSlide != Disabled)
    {
        result += ",113," + std::to_string(m_boostSlide);
    }

    return result;
}

GameOptionsTrigger::~GameOptionsTrigger()
{
    // Destructor
    // The base EffectGameObject destructor will handle cleanup
}