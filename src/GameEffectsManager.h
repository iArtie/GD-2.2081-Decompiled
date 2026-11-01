#pragma once

#include "cocos2d.h"

class PlayLayer;

class GameEffectsManager : public cocos2d::CCNode {
public:
    static GameEffectsManager* create(PlayLayer* playLayer);
    bool init(PlayLayer* playLayer);
    void scaleParticle(cocos2d::CCParticleSystemQuad* particle, float scale);
    
    virtual ~GameEffectsManager();

private:
    PlayLayer* m_playLayer;
};