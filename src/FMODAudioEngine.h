#pragma once

#include "cocos2d.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include "FMODAudioState.h"
#include "FMODSound.h"
#include "FMODMusic.h"
#include "FMODQueuedEffect.h"

enum AudioTargetType
{
    SFXChannel = 0,
    MusicChannel = 1,
    SFXGroup = 2
};

class FMODAudioEngine : public cocos2d::CCNode
{
public:
    static FMODAudioEngine* sharedEngine();
    
    void setup();
    void setupAudioEngine();
    void start();
    void stop();
    void update(float dt);
    
    int playEffect(const std::string& path);
    int playEffect(const std::string& path, float volume, float pitch, float pan);
    int playEffectAsync(const std::string& path);
    int playEffectAsync(const std::string& path, float volume, float pitch, float pan);
    int playEffectAdvanced(const std::string& path, float speed, float unk024, float volume, float pitch, 
                          bool fft, bool reverb, int start, int end, int fadeIn, int fadeOut, 
                          bool loop, int effectID, bool override, bool unk04d, int channelID, 
                          int uniqueID, float minInterval, int group);
    FMODSound* preloadEffect(const std::string& path);
    FMOD::Sound* preloadEffectAsync(const std::string& path);
    void queuedEffectFinishedLoading(const std::string& path);
    void unloadAllEffects();
    void unloadEffect(const std::string& path);
    bool isEffectLoaded(const std::string& path);
    void stopAllEffects();
    void pauseEffect(unsigned int effect);
    void resumeEffect(unsigned int effect);
    void pauseAllEffects();
    void resumeAllEffects();
    int countActiveEffects();
    
    void loadMusic(const std::string& path);
    void loadMusic(const std::string& path, float pitch, float unknown, float volume, 
                   bool loop, int musicID, bool noPrepare, bool dontReset);
    void loadAndPlayMusic(const std::string& path, unsigned int time, int musicID);
    FMOD::Sound* preloadMusic(const std::string& path, bool noRelease, int musicID);
    void stopAllMusic(bool clear);
    void stopAndRemoveMusic(int id);
    void pauseMusic(int musicChannel);
    void resumeMusic(int musicChannel);
    void pauseAllMusic(bool force);
    void resumeAllMusic();
    int countActiveMusic();
    unsigned int getMusicLengthMS(int channel);
    unsigned int getMusicTimeMS(int channel);
    float getMusicTime(int channel);
    void setMusicTimeMS(unsigned int time, bool dontWait, int musicID);
    FMOD::Channel* getActiveMusicChannel(int musicID);
    bool isAnyPersistentPlaying();
    bool isPersistentMatchPlaying(const std::string& path, int musicID);
    std::string getActiveMusic(int id);
    int getMusicChannelID(int musicID);
    void swapMusicIndex(int musicID, int channelID);
    void triggerQueuedMusic(FMODQueuedMusic* music);
    void activateQueuedMusic(int channel);
    void startMusic(int start, int end, int fadeIn, int fadeOut, bool loop, int musicID, bool unknown, bool dontReset);
    bool isMusicPlaying(int musicID);
    void stopMusic(int musicID);
    
    void setBackgroundMusicVolume(float volume);
    void setEffectsVolume(float volume);
    float getBackgroundMusicVolume();
    float getEffectsVolume();
    void fadeInBackgroundMusic(float value);
    void fadeInMusic(float duration, int channel);
    void fadeOutMusic(float duration, int channel);
    void fadeMusic(float duration, int channel, float startVolume, float endVolume);
    float stopAndGetFade(FMOD::Channel* channel);
    
    void pauseAllAudio();
    void resumeAllAudio();
    void clearAllAudio();
    
    void enableMetering();
    void disableMetering();
    float getMeteringValue();
    
    static float pitchForIdx(int index);
    
    FMOD::Sound* createStream(const std::string& path);
    FMODSound* storeEffect(FMOD::Sound* sound, const std::string& path);
    bool isSoundReady(FMOD::Sound* sound);
    FMOD_OPENSTATE waitUntilSoundReady(FMOD::Sound* sound);
    
    int getNextChannelID();
    void registerChannel(FMOD::Channel* channel, int channelID, int effectID);
    void unregisterChannel(int id);
    FMOD::Channel* channelForChannelID(int id);
    FMOD::Channel* channelForUniqueID(int id);
    int channelIDForUniqueID(int id);
    void channelLinkSound(int id, FMODSound* sound);
    void channelUnlinkSound(int id);
    void channelStopped(FMOD::Channel* channel, bool remove);
    
    void setChannelVolume(int id, AudioTargetType type, float volume);
    void setChannelVolumeMod(int id, AudioTargetType type, float volumeMod);
    void setChannelPitch(int id, AudioTargetType type, float pitch);
    void stopChannel(int id);
    bool isChannelStopping(int channel);
    void stopChannelTween(int id, AudioTargetType target, AudioModType mod);
    void stopChannelTweens(int id, AudioTargetType target);
    FMOD::ChannelGroup* getChannelGroup(int id, bool reverb);
    std::map<std::pair<int, int>, FMODSoundTween>* getTweenContainer(AudioTargetType type);
    
    void saveAudioState(FMODAudioState* state);
    void loadAudioState(FMODAudioState* state);
    
    void updateBackgroundFade();
    void updateChannelTweens(float dt);
    void updateMetering();
    void updateQueuedEffects();
    void updateQueuedMusic();
    void updateTemporaryEffects();
    void releaseRemovedSounds();
    void updateReverb(FMODReverbPreset preset, bool force);
    
    ~FMODAudioEngine();

private:
    FMOD::System* m_system;
    FMOD::ChannelGroup* m_backgroundMusicChannel;
    FMOD::ChannelGroup* m_globalChannel;
    FMOD::ChannelGroup* m_reverbChannel;
    FMOD::DSP* m_mainDSP;
    FMOD::DSP* m_globalChannelDSP;
    int m_sampleRate;
    bool m_reducedQuality;
    bool m_stopped;
    bool m_allAudioPaused;
    bool m_metering;
    bool m_showAudioVisualizer;
    float m_backgroundMusicFade;
    float m_musicFadeStart;
    float m_musicVolume;
    float m_sfxVolume;
    float m_pulse1;
    float m_pulse2;
    float m_pulse3;
    int m_lastResult;
    int m_reverbPreset;
    int m_musicOffset;
    int m_nextChannelID;
    
    std::unordered_map<int, FMOD::Channel*> m_channelIDToChannel;
    std::unordered_map<int, FMODMusic> m_fmodMusic;
    std::unordered_map<std::string, FMODSound> m_fmodSounds;
    std::unordered_map<int, std::string> m_channelIDToSoundPath;
    std::unordered_map<int, FMOD::ChannelGroup*> m_globalChannelGroups;
    std::unordered_map<int, FMOD::ChannelGroup*> m_reverbChannelGroups;
    std::unordered_map<int, int> m_channelIDToEffectID;
    std::unordered_map<int, int> m_effectIDToChannelID;
    std::unordered_map<std::string, FMOD::Sound*> m_soundPathToSound;
    std::vector<FMOD::Sound*> m_removedSounds;
    std::unordered_set<int> m_stoppedChannels;
    std::vector<FMODQueuedEffect> m_queuedEffects;
    std::unordered_set<std::string> m_temporarySoundPaths;
    
    FMODAudioState m_audioState;
};