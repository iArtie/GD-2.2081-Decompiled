#include "SFXTriggerGameObject.h"
#include "GJBaseGameLayer.h"
#include "FMODAudioEngine.h"

SFXTriggerGameObject* SFXTriggerGameObject::create(const char* frame)
{
    SFXTriggerGameObject* instance = new SFXTriggerGameObject();
    if (instance->init(frame))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

int SFXTriggerGameObject::getSFXRefID()
{
    if (m_sfxUniqueID > 0)
        return m_sfxUniqueID;

    return -m_uniqueID;
}

std::string SFXTriggerGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string baseString = EffectGameObject::getSaveString(layer);

    std::string result = baseString;

    auto appendInt = [&result](const char* key, int value) {
        result += ",";
        result += key;
        result += ",";
        result += std::to_string(value);
    };

    auto appendFloat = [&result](const char* key, float value) {
        result += ",";
        result += key;
        result += ",";
        result += cocos2d::CCString::createWithFormat("%.2f", value)->getCString();
    };

    if (m_soundID)
        appendInt("392", m_soundID);

    if (m_speed)
        appendInt("404", m_speed);

    if (m_pitchIndex)
        appendInt("405", m_pitchIndex);

    if (m_volume != 0.0f)
        appendFloat("406", m_volume);

    if (m_reverb)
        appendInt("407", m_reverb);

    if (m_fastFourierTransform)
        appendInt("412", m_fastFourierTransform);

    if (m_loop)
        appendInt("413", m_loop);

    if (m_stopLoop)
        appendInt("414", m_stopLoop);

    if (m_unique)
        appendInt("415", m_unique);

    if (m_sfxUniqueID)
        appendInt("416", m_sfxUniqueID);

    if (m_override)
        appendInt("420", m_override);

    if (m_start)
        appendInt("408", m_start);

    if (m_fadeIn)
        appendInt("409", m_fadeIn);

    if (m_end)
        appendInt("410", m_end);

    if (m_fadeOut)
        appendInt("411", m_fadeOut);

    if (m_stop)
        appendInt("417", m_stop);

    if (m_changeSpeed)
        appendInt("418", m_changeSpeed);

    if (m_changeVolume)
        appendInt("419", m_changeVolume);

    if (m_targetGroupID)
        appendInt("51", m_targetGroupID);

    if (m_centerGroupID)
        appendInt("71", m_centerGroupID);

    if (m_volumeNear != 0.0f)
        appendFloat("421", m_volumeNear);

    if (m_volumeMedium != 0.0f)
        appendFloat("422", m_volumeMedium);

    if (m_volumeFar != 0.0f)
        appendFloat("423", m_volumeFar);

    if (m_minDistNear)
        appendInt("424", m_minDistNear);

    if (m_minDistMedium)
        appendInt("425", m_minDistMedium);

    if (m_minDistFar)
        appendInt("426", m_minDistFar);

    if (m_proximityMode)
        appendInt("458", m_proximityMode);

    if (m_cameraDistance)
        appendInt("428", m_cameraDistance);

    if (m_targetPlayer1)
        appendInt("138", m_targetPlayer1);

    if (m_targetPlayer2)
        appendInt("200", m_targetPlayer2);

    if (m_ignoreVolumeTest)
        appendInt("489", m_ignoreVolumeTest);

    if (m_duration != 0.0f)
        appendFloat("10", m_duration);

    if (m_preload)
        appendInt("433", m_preload);

    if (m_minInterval != 0.0f)
        appendFloat("434", m_minInterval);

    if (m_sfxGroup)
        appendInt("455", m_sfxGroup);

    if (m_groupID)
        appendInt("457", m_groupID);

    if (m_soundDuration != 0.0f)
        appendFloat("490", m_soundDuration);

    if (m_reverbPreset != FMODReverbPreset::Generic)
        appendInt("502", (int)m_reverbPreset);

    if (m_reverbEnabled)
        appendInt("503", m_reverbEnabled);

    if (m_dontReset)
        appendInt("595", m_dontReset);

    if (m_speedVariance)
        appendInt("596", m_speedVariance);

    if (m_pitchVariance)
        appendInt("597", m_pitchVariance);

    if (m_volumeVariance != 0.0f)
        appendFloat("598", m_volumeVariance);

    if (m_pitchSteps)
        appendInt("599", m_pitchSteps);

    return result;
}

int SFXTriggerGameObject::getUniqueSFXID()
{
    if (m_unique)
        return getSFXRefID();

    return 0;
}

SFXTriggerGameObject::~SFXTriggerGameObject()
{
}