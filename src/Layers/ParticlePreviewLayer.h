#pragma once

#include "Platform/cocos2d.h"

class ParticlePreviewLayer : public cocos2d::CCLayerColor {
public:
    static ParticlePreviewLayer* create(cocos2d::CCParticleSystemQuad* particleSystem);

    bool init(cocos2d::CCParticleSystemQuad* particleSystem) override;

    virtual void draw() override;
    virtual void visit() override;
    virtual void postVisit() override;

    virtual ~ParticlePreviewLayer();

protected:
    int m_particleMode = 0;
    int m_drawMode = 0;
    int m_gravityMode = 1;
    cocos2d::CCParticleSystemQuad* m_particleSystem = nullptr;
};