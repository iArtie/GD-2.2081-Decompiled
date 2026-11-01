#pragma once

#include <string>
#include "EffectGameObject.h"

class GJBaseGameLayer;

class PlayerControlGameObject : public EffectGameObject {
public:
    static PlayerControlGameObject* create();

    bool init() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;

    virtual ~PlayerControlGameObject();

protected:
    bool m_targetPlayer1 = false;
    bool m_targetPlayer2 = false;
    bool m_stopJump = false;
    bool m_stopMove = false;
    bool m_stopRotation = false;
    bool m_stopSlide = false;
};