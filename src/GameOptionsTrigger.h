#pragma once

#include "cocos2d.h"
#include <string>

class GJBaseGameLayer;
class EffectGameObject;

class GameOptionsTrigger : public EffectGameObject {
public:
    static GameOptionsTrigger* create(const char* frame);
    std::string getSaveString(GJBaseGameLayer* layer);
    virtual ~GameOptionsTrigger();

private:
    bool m_streakAdditive;
    int m_unlinkDualGravity;
    int m_hideGround;
    int m_hideP1;
    int m_hideP2;
    int m_disableP1Controls;
    int m_disableP2Controls;
    int m_hideMG;
    int m_hideAttempts;
    int m_editRespawnTime;
    double m_respawnTime;
    int m_audioOnDeath;
    int m_noDeathSFX;
    int m_boostSlide;
};