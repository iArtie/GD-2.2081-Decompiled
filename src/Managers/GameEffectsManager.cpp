#include "Managers/GameEffectsManager.h"
#include "Layers/PlayLayer.h"

GameEffectsManager* GameEffectsManager::create(PlayLayer* playLayer) {
    GameEffectsManager* ret = new GameEffectsManager();
    if (ret && ret->init(playLayer)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameEffectsManager::init(PlayLayer* playLayer) {
    m_playLayer = playLayer;
    return true;
}

void GameEffectsManager::scaleParticle(cocos2d::CCParticleSystemQuad* particle, float scale) {
    if (!particle) return;
    
    bool isGrouped = (particle->getPositionType() == kCCPositionTypeGrouped);
    
    if (isGrouped) {
        // For grouped particles, just scale the entire system
        float currentScale = particle->getScale();
        particle->setScale(currentScale * scale);
    } else {
        // For non-grouped particles, scale individual properties
        particle->setStartSize(particle->getStartSize() * scale);
        particle->setStartSizeVar(particle->getStartSizeVar() * scale);
        particle->setEndSize(particle->getEndSize() * scale);
        particle->setEndSizeVar(particle->getEndSizeVar() * scale);
        
        // Scale position variance
        cocos2d::CCPoint posVar = particle->getPosVar();
        particle->setPosVar(posVar * scale);
        
        // Scale speed if in gravity mode
        if (particle->getEmitterMode() == 0) { // kCCParticleModeGravity
            particle->setSpeed(particle->getSpeed() * scale);
            particle->setSpeedVar(particle->getSpeedVar() * scale);
        }
    }
}

GameEffectsManager::~GameEffectsManager() {
    // Base class destructor handles cleanup
}