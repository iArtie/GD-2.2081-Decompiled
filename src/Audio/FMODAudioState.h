#pragma once

#include <unordered_map>
#include <map>
#include "FMODSoundState.h"
#include "FMODQueuedMusic.h"
#include "FMODSoundTween.h"

class FMODAudioState
{
public:
    FMODAudioState();
    ~FMODAudioState();
    
    float m_elapsed;
    float m_interval;
    unsigned long long m_unkUint64_1;
    
    std::unordered_map<int, FMODSoundState> m_soundStateForChannels;
    std::unordered_map<int, FMODQueuedMusic> m_queuedMusicForChannels1;
    std::unordered_map<int, FMODQueuedMusic> m_queuedMusicForChannels2;
    
    std::unordered_map<int, float> m_volumeForEffectChannels;
    std::unordered_map<int, float> m_volumeModForEffectChannels;
    std::unordered_map<int, float> m_pitchForEffectChannels;
    
    std::unordered_map<int, float> m_volumeForEffectGroups;
    std::unordered_map<int, float> m_volumeModForEffectGroups;
    std::unordered_map<int, float> m_pitchForEffectGroups;
    
    std::unordered_map<int, float> m_volumeForMusicChannels;
    std::unordered_map<int, float> m_volumeModForMusicChannels;
    std::unordered_map<int, float> m_pitchForMusicChannels;
    
    std::unordered_map<int, float> m_intervalForEffects;
    
    std::map<std::pair<int, int>, FMODSoundTween> m_tweensForEffectChannels;
    std::map<std::pair<int, int>, FMODSoundTween> m_tweensForEffectGroups;
    std::map<std::pair<int, int>, FMODSoundTween> m_tweensForMusicChannels;
};