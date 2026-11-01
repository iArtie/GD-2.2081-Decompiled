#pragma once

#include "GameObject.h"
#include <string>

class PlayerObject;
class GJBaseGameLayer;

class EnhancedGameObject : public GameObject {
public:
    void activatedByPlayer(PlayerObject* player);
    void animationTriggered();
    bool canAllowMultiActivate();
    bool canMultiActivate(bool multiActivate);
    static EnhancedGameObject* create(const char* frame);
    void createRotateAction(float angle, int clockwiseDirection);
    void customSetup() override;
    void deactivateObject(bool deactivate) override;
    bool getHasRotateAction();
    bool getHasSyncedAnimation();
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool hasBeenActivated();
    bool hasBeenActivatedByPlayer(PlayerObject* player);
    bool init(const char* frame) override;
    void powerOffObject();
    void powerOnObject(int state);
    void previewAnimateOnTrigger();
    void refreshRotateAction();
    void resetObject() override;
    void resetSyncedAnimation();
    void restoreObject() override;
    void saveActiveColors() override;
    void setupAnimationVariables();
    void stateSensitiveOff(GJBaseGameLayer* layer);
    void triggerActivated(float xPosition);
    void triggerAnimation();
    void updateAnimateOnTrigger(bool animate);
    void updateRotateAction(float dt);
    void updateState(int state);
    void updateUserCoin();
    void waitForAnimationTrigger();
    
    virtual ~EnhancedGameObject();

private:
    int m_objectID;
    int m_state;
    int m_classType;
    
    bool m_hasCustomAnimation;
    bool m_hasCustomRotation;
    bool m_isRingPoweredOn;
    bool m_isNoMultiActivate;
    bool m_isMultiActivate;
    bool m_animateOnTrigger;
    bool m_isActivated;
    bool m_activated;
    bool m_activatedByPlayer1;
    bool m_activatedByPlayer2;
    bool m_poweredOn;
    bool m_unk548;
    bool m_bUnkBool2;
    bool m_editorEnabled;
    bool m_hasUniqueCoin;
    bool m_disableRotation;
    bool m_usesAudioScale;
    bool m_customAudioScale;
    bool m_colorZLayerRelated;
    bool m_isInvisibleBlock;
    bool m_updateCustomContentSize;
    bool m_animationShouldUseSpeed;
    bool m_customGlowColor;
    bool m_glowColorIsLBG;
    bool m_unk4F8;
    bool m_hasNoEffects;
    bool m_particleUseObjectColor;
    bool m_particleLocked;
    bool m_colorSpriteLocked;
    bool m_animateOnlyWhenActive;
    bool m_disableDelayedLoop;
    bool m_singleFrame;
    bool m_isDisabled2;
    bool m_animationTriggered;
    bool m_visible;
    bool m_shouldNotHideAnimFreeze;
    bool m_usesSpecialAnimation;
    
    float m_rotationSpeed;
    float m_rotationAngle;
    float m_rotationDelta;
    float m_height;
    float m_width;
    float m_randomFrameTime;
    float m_animationSpeed;
    float m_frameTime;
    float m_minAudioScale;
    float m_maxAudioScale;
    float m_spriteWidthScale;
    float m_spriteHeightScale;
    float m_objectRadius;
    float m_rotationAnimationSpeed;
    float m_unk544;
    float m_animationStart;
    float m_unk540;
    float m_animationRandomizedStartValue;
    
    int m_animationOffset;
    int m_frames;
    int m_animationRandomizedStart;
    int m_unk4C0;
    int m_unk4C4;
    int m_unk4C8;
    int m_unk4CC;
    int m_unkAnimationInt;
    int m_maybeAnimationVariableXInt;
    int m_defaultZOrder;
    int m_defaultZLayer;
    int m_objectType;
    
    EnhancedGameObject* m_colorSprite;
    cocos2d::CCSprite* m_glowSprite;
    cocos2d::CCPoint m_particleOffset;
};