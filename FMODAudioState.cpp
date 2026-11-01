#include "FMODAudioState.h"

FMODAudioState::FMODAudioState()
    : m_elapsed(0.0f)
    , m_interval(0.0f)
    , m_unkUint64_1(0)
{
}

FMODAudioState::~FMODAudioState()
{
    m_soundStateForChannels.clear();
    m_queuedMusicForChannels1.clear();
    m_queuedMusicForChannels2.clear();
    m_intervalForEffects.clear();
    m_pitchForMusicChannels.clear();
    m_volumeModForMusicChannels.clear();
    m_volumeForMusicChannels.clear();
    m_pitchForEffectGroups.clear();
    m_volumeModForEffectGroups.clear();
    m_volumeForEffectGroups.clear();
    m_pitchForEffectChannels.clear();
    m_volumeModForEffectChannels.clear();
    m_volumeForEffectChannels.clear();
    m_tweensForMusicChannels.clear();
    m_tweensForEffectGroups.clear();
    m_tweensForEffectChannels.clear();
}