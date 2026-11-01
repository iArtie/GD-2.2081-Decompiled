#pragma once

#include <string>

#include "EffectGameObject.h"

class RotateGameplayGameObject : public EffectGameObject {
public:
    static RotateGameplayGameObject* create();

    bool init() override;

    std::string getSaveString(GJBaseGameLayer* layer) override;

    void updateGameplayRotation();
    void updateStartValues() override;

    virtual ~RotateGameplayGameObject();

protected:
    int m_slopeDirection = 0;
    int m_moveDirection = 0;
    int m_groundDirection = 0;
    int m_editVelocity = 0;
    int m_overrideVelocity = 0;
    double m_velocityModX = 0.0;
    double m_velocityModY = 0.0;
    int m_changeChannel = 0;
    int m_channelOnly = 0;
    int m_targetChannelID = 0;
    int m_instantOffset = 0;
    int m_dontSlide = 0;
};