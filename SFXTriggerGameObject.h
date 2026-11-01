#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "EffectGameObject.h"
#include <string>

class SFXTriggerGameObject : public EffectGameObject {
public:
    static SFXTriggerGameObject* create(const char* frame);
    bool init(const char* frame);

    int getSFXRefID();
    std::string getSaveString(GJBaseGameLayer* layer) override;
    int getUniqueSFXID();

    ~SFXTriggerGameObject();

    int m_sfxUniqueID = 0;
    int m_uniqueID = 0;
    bool m_unique = false;
    int m_soundID = 0;
    int m_speed = 0;
    int m_pitchIndex = 0;
    float m_volume = 1.0f;
    int m_reverb = 0;
    bool m_fastFourierTransform = false;
    int m_loop = 0;
    int m_stopLoop = 0;
    int m_override = 0;
    int m_start = 0;
    int m_fadeIn = 0;
    int m_end = 0;
    int m_fadeOut = 0;
    int m_stop = 0;
    int m_changeSpeed = 0;
    int m_changeVolume = 0;
    int m_targetGroupID = 0;
    int m_centerGroupID = 0;
    float m_volumeNear = 0.0f;
    float m_volumeMedium = 0.0f;
    float m_volumeFar = 0.0f;
    int m_minDistNear = 0;
    int m_minDistMedium = 0;
    int m_minDistFar = 0;
    int m_proximityMode = 0;
    int m_cameraDistance = 0;
    int m_targetPlayer1 = 0;
    int m_targetPlayer2 = 0;
    bool m_ignoreVolumeTest = false;
    float m_duration = 0.0f;
    int m_preload = 0;
    float m_minInterval = 0.0f;
    int m_sfxGroup = 0;
    int m_groupID = 0;
    float m_soundDuration = 0.0f;
    FMODReverbPreset m_reverbPreset = FMODReverbPreset::Generic;
    bool m_reverbEnabled = false;
    bool m_dontReset = false;
    int m_speedVariance = 0;
    int m_pitchVariance = 0;
    float m_volumeVariance = 0.0f;
    int m_pitchSteps = 0;
};