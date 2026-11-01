#pragma once

#include "Platform/cocos2d.h"
#include "Objects/EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

class SpawnParticleGameObject : public EffectGameObject {
public:
    static SpawnParticleGameObject* create();
    bool init() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~SpawnParticleGameObject();

protected:
    int m_targetGroupID = 0;
    int m_centerGroupID = 0;
    cocos2d::CCPoint m_offset;
    cocos2d::CCPoint m_offsetVariance;
    bool m_matchRotation = false;
    double m_rotation = 0.0;
    double m_rotationVariance = 0.0;
    double m_scale = 0.0;
    double m_scaleVariance = 0.0;
};