#pragma once

#include "cocos2d.h"
#include "EnhancedGameObject.h"
#include <string>

class GJBaseGameLayer;

class ParticleGameObject : public EnhancedGameObject {
public:
    static ParticleGameObject* create();

    bool init() override;
    void customSetup() override;
    void resetObject() override;
    void deactivateObject(bool deactivate) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;

    void addMainSpriteToParent(bool reorder) override;
    void setChildColor(const cocos2d::ccColor3B* color) override;
    void setObjectColor(const cocos2d::ccColor3B* color) override;
    void setRotation(float rotation) override;
    void setRotationX(float rotationX) override;
    void setRotationY(float rotationY) override;
    void setScale(float scale) override;
    void setScaleX(float scaleX) override;
    void setScaleY(float scaleY) override;

    void applyParticleSettings(cocos2d::CCParticleSystemQuad* particle);
    void blendModeChanged();
    void claimParticle();
    void createAndAddCustomParticle();
    void createParticlePreviewArt();
    void particleWasActivated();
    void setParticleString(std::string str);
    void unclaimParticle();
    void updateAnimateOnTrigger(bool animate);
    void updateMainParticleOpacity(unsigned char opacity);
    void updateParticle();
    void updateParticleColor(const cocos2d::ccColor3B* color);
    void updateParticleOpacity(unsigned char opacity);
    void updateParticlePreviewArtOpacity(float opacity);
    void updateParticleScale(float scale);
    void updateParticleStruct();
    void updateSecondaryParticleOpacity(unsigned char opacity);
    void updateSyncedAnimation(float totalTime, int frameIndex);

    virtual ~ParticleGameObject();

protected:
    cocos2d::CCParticleSystemQuad* m_particle = nullptr;
    bool m_particleUseObjectColor = false;
    bool m_hasUniformObjectColor = false;
    bool m_baseOrDetailBlending = false;
    bool m_bUnkBool2 = false;
    bool m_hasCustomAnimation = false;
    cocos2d::CCPoint m_particleOffset;
    bool m_particleLocked = false;
    bool m_isParticleSpriteLocked = false;
    bool m_unk370 = false;
    bool m_ignoreFade = false;
    bool m_ignoreEnter = false;
    bool m_hasParticles = false;
    bool m_blackChildOpacityLocked = false;
    std::string m_particleData;
    char m_animateOnTrigger = 0;
    bool m_animationTriggered = false;
    float m_respawnResult = 0.0f;
    bool m_startingRespawn = false;
    bool m_shouldQuickStart = false;
    bool m_isActivated = false;
    bool m_editorEnabled = false;
    cocos2d::ParticleStruct m_particleStruct;
    bool m_updatedParticleData = false;
    cocos2d::CCSprite* m_colorSprite = nullptr;
    bool m_notPreviewing = false;
    bool m_isUIObject = false;
};