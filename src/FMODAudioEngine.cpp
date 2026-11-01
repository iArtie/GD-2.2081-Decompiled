#include "FMODAudioEngine.h"
#include "GameManager.h"
#include "PlatformToolbox.h"
#include "AppDelegate.h"
#include "fmod.hpp"
#include "fmod_errors.h"

FMODAudioEngine* FMODAudioEngine::sharedEngine()
{
    static FMODAudioEngine* instance = nullptr;
    if (!instance)
    {
        instance = new FMODAudioEngine();
        instance->setup();
    }
    return instance;
}

void FMODAudioEngine::setup()
{
    setupAudioEngine();
    m_sfxVolume = 1.0f;
    m_musicVolume = 1.0f;
    m_backgroundMusicFade = 1.0f;
    m_musicFadeStart = -1.0f;
    m_metering = false;
    m_stopped = false;
    m_allAudioPaused = false;
    m_showAudioVisualizer = false;
    m_nextChannelID = 0;
    m_musicOffset = 0;
    m_reverbPreset = 0;
    m_sampleRate = 44100;
    m_reducedQuality = false;
    m_pulse1 = 0.1f;
    m_pulse2 = 0.1f;
    m_pulse3 = 0.0f;
    
    m_audioState.m_elapsed = 0.0f;
    m_audioState.m_interval = 0.0f;
    m_audioState.m_unkUint64_1 = 0;
    
    std::vector<std::string> effects = {
        "achievement_01.ogg",
        "playSound_01.ogg",
        "quitSound_01.ogg",
        "explode_11.ogg",
        "endStart_02.ogg",
        "highscoreGet02.ogg"
    };
    
    for (const auto& effect : effects)
    {
        FMODSound* sound = preloadEffect(effect);
        if (sound)
        {
            sound->m_preloaded = true;
        }
    }
}

void FMODAudioEngine::setupAudioEngine()
{
    FMOD_RESULT result;
    result = FMOD::System_Create(&m_system);
    if (result != FMOD_OK) return;
    
    unsigned int version = 0;
    m_system->getVersion(&version);
    
    unsigned int streamBufferSize = 0;
    int streamBufferType = 1;
    m_system->getStreamBufferSize(&streamBufferSize, &streamBufferType);
    
    GameManager* gameManager = GameManager::sharedState();
    if (!GameManager::getGameVariable(gameManager, "0159"))
    {
        m_system->setDSPBufferSize(512, 4);
    }
    
    unsigned int bufferSize = 0;
    int numBuffers = 0;
    m_system->getDSPBufferSize(&bufferSize, &numBuffers);
    
    int sampleRate = 0;
    FMOD_SPEAKERMODE speakerMode = FMOD_SPEAKERMODE_STEREO;
    int numRawSpeakers = 0;
    m_system->getSoftwareFormat(&sampleRate, &speakerMode, &numRawSpeakers);
    
    bool reducedQuality = GameManager::getGameVariable(gameManager, "0142");
    if (reducedQuality)
    {
        sampleRate = 24000;
    }
    else
    {
        sampleRate = 44100;
    }
    
    m_reducedQuality = reducedQuality;
    m_system->setSoftwareFormat(sampleRate, speakerMode, numRawSpeakers);
    m_system->init(128, FMOD_INIT_NORMAL, nullptr);
    m_system->getSoftwareFormat(&m_sampleRate, &speakerMode, &numRawSpeakers);
    
    m_system->createChannelGroup("BackgroundMusic", &m_backgroundMusicChannel);
    m_backgroundMusicChannel->setVolumeRamp(false);
    m_backgroundMusicChannel->getDSP(-1, &m_mainDSP);
    m_mainDSP->setMeteringEnabled(false, true);
    
    m_system->createChannelGroup("Global", &m_globalChannel);
    m_system->createChannelGroup("Reverb", &m_reverbChannel);
    m_globalChannel->addGroup(m_reverbChannel);
    
    FMOD::DSP* dsp = nullptr;
    m_system->createDSPByType(FMOD_DSP_TYPE_LIMITER, &dsp);
    dsp->setParameterFloat(1, 0.0f);
    dsp->setParameterBool(3, true);
    m_globalChannel->addDSP(1, dsp);
    
    m_system->createDSPByType(FMOD_DSP_TYPE_LIMITER, &dsp);
    dsp->setParameterFloat(1, 0.0f);
    dsp->setParameterBool(3, true);
    m_backgroundMusicChannel->addDSP(1, dsp);
    
    FMOD::DSP* reverbDSP = nullptr;
    m_system->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &reverbDSP);
    m_reverbChannel->addDSP(0, reverbDSP);
    updateReverb((FMODReverbPreset)m_reverbPreset, true);
    
    m_globalChannel->getDSP(-1, &m_globalChannelDSP);
    m_globalChannelDSP->setMeteringEnabled(false, true);
}

void FMODAudioEngine::start()
{
    if (m_stopped)
    {
        m_system->mixerResume();
        m_system->update();
        
        if (PlatformToolbox::shouldResumeSound())
        {
            AppDelegate* appDelegate = AppDelegate::get();
            appDelegate->resumeSound();
        }
        
        update(0.0f);
        resumeSchedulerAndActions();
        m_stopped = false;
    }
}

void FMODAudioEngine::stop()
{
    if (!m_stopped)
    {
        m_stopped = true;
        m_system->mixerSuspend();
        m_system->update();
        pauseSchedulerAndActions();
    }
}

void FMODAudioEngine::update(float dt)
{
    if (m_allAudioPaused)
    {
        return;
    }
    
    m_audioState.m_elapsed += dt;
    m_audioState.m_interval += dt;
    
    m_system->update();
    updateQueuedEffects();
    updateTemporaryEffects();
    releaseRemovedSounds();
    updateQueuedMusic();
    updateBackgroundFade();
    
    if (m_audioState.m_interval >= 0.016667f)
    {
        updateMetering();
        updateChannelTweens(m_audioState.m_interval);
        m_audioState.m_interval = 0.0f;
    }
}

void FMODAudioEngine::updateBackgroundFade()
{
    if (m_musicFadeStart != 0.0f)
    {
        float elapsed = m_audioState.m_elapsed;
        float progress = 0.0f;
        bool flag = false;
        
        if (m_musicFadeStart < 0.0f)
        {
            flag = m_backgroundMusicFade <= 0.0f;
            m_musicFadeStart = elapsed;
            progress = elapsed - m_musicFadeStart;
            if (m_backgroundMusicFade > 0.0f)
            {
                progress = progress / m_backgroundMusicFade;
                if (progress < 1.0f)
                {
                    if (progress <= 0.0f)
                    {
                        progress = 0.0f;
                    }
                    m_backgroundMusicChannel->setVolume(progress * m_musicVolume);
                    if (progress >= m_backgroundMusicFade)
                    {
                        m_musicFadeStart = 0.0f;
                    }
                    return;
                }
            }
        }
        else
        {
            progress = elapsed - m_musicFadeStart;
            flag = m_backgroundMusicFade <= 0.0f;
            if (m_backgroundMusicFade > 0.0f)
            {
                progress = progress / m_backgroundMusicFade;
                if (progress < 1.0f)
                {
                    if (progress <= 0.0f)
                    {
                        progress = 0.0f;
                    }
                    m_backgroundMusicChannel->setVolume(progress * m_musicVolume);
                    if (progress >= m_backgroundMusicFade)
                    {
                        m_musicFadeStart = 0.0f;
                    }
                    return;
                }
            }
        }
        
        if (flag)
        {
            progress = 1.0f;
            m_backgroundMusicChannel->setVolume(progress * m_musicVolume);
            if (progress >= m_backgroundMusicFade)
            {
                m_musicFadeStart = 0.0f;
            }
        }
        else
        {
            progress = progress / m_backgroundMusicFade;
            if (progress >= 1.0f)
            {
                progress = 1.0f;
            }
            m_backgroundMusicChannel->setVolume(progress * m_musicVolume);
            if (progress >= m_backgroundMusicFade)
            {
                m_musicFadeStart = 0.0f;
            }
        }
    }
}

void FMODAudioEngine::updateChannelTweens(float dt)
{
    std::map<std::pair<int, int>, FMODSoundTween>* containers[3] = {
        &m_audioState.m_tweensForEffectChannels,
        &m_audioState.m_tweensForMusicChannels,
        &m_audioState.m_tweensForEffectGroups
    };
    
    AudioTargetType types[3] = { SFXChannel, MusicChannel, SFXGroup };
    
    for (int i = 0; i < 3; i++)
    {
        auto& container = *containers[i];
        AudioTargetType type = types[i];
        
        auto it = container.begin();
        while (it != container.end())
        {
            FMODSoundTween& tween = it->second;
            tween.m_elapsed += m_audioState.m_interval;
            
            float progress = tween.m_elapsed / tween.m_duration;
            if (progress >= 1.0f)
            {
                progress = 1.0f;
                tween.m_finished = true;
            }
            
            float currentValue = tween.m_start + (tween.m_end - tween.m_start) * progress;
            tween.m_current = currentValue;
            
            if (tween.m_type == Volume)
            {
                setChannelVolume(it->first.first, type, currentValue);
            }
            else if (tween.m_type == Pitch)
            {
                setChannelPitch(it->first.first, type, currentValue);
            }
            
            if (tween.m_finished)
            {
                it = container.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void FMODAudioEngine::updateMetering()
{
    if (m_metering)
    {
        // Update metering values
    }
}

void FMODAudioEngine::updateQueuedEffects()
{
    auto it = m_soundPathToSound.begin();
    while (it != m_soundPathToSound.end())
    {
        if (isSoundReady(it->second))
        {
            FMODSound* sound = storeEffect(it->second, it->first);
            queuedEffectFinishedLoading(it->first);
            it = m_soundPathToSound.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void FMODAudioEngine::updateQueuedMusic()
{
    auto it = m_audioState.m_queuedMusicForChannels1.begin();
    while (it != m_audioState.m_queuedMusicForChannels1.end())
    {
        FMODQueuedMusic& music = it->second;
        
        if (music.m_sound && !isSoundReady(music.m_sound))
        {
            ++it;
            continue;
        }
        
        if (music.m_queuedStatus == 0)
        {
            loadMusic(music.m_filePath, music.m_pitch, music.m_unkFloat2, music.m_volume, 
                     music.m_loop, music.m_musicID, 0, music.m_dontReset);
            setMusicTimeMS(music.m_start, true, music.m_musicID);
            music.m_queuedStatus = 1;
        }
        else if (music.m_queuedStatus == 1)
        {
            if (!music.m_noPrepare || !music.m_stopMusic)
            {
                triggerQueuedMusic(&music);
            }
            else
            {
                m_audioState.m_queuedMusicForChannels2[it->first] = music;
            }
            it = m_audioState.m_queuedMusicForChannels1.erase(it);
            continue;
        }
        
        ++it;
    }
}

void FMODAudioEngine::updateTemporaryEffects()
{
    auto it = m_temporarySoundPaths.begin();
    while (it != m_temporarySoundPaths.end())
    {
        auto soundIt = m_fmodSounds.find(*it);
        if (soundIt != m_fmodSounds.end() && soundIt->second.m_playCount == 0)
        {
            FMOD_OPENSTATE state;
            soundIt->second.m_sound->getOpenState(&state, nullptr, nullptr, nullptr);
            if (state == FMOD_OPENSTATE_READY || state == FMOD_OPENSTATE_PLAYING)
            {
                m_removedSounds.push_back(soundIt->second.m_sound);
                m_fmodSounds.erase(soundIt);
                it = m_temporarySoundPaths.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void FMODAudioEngine::releaseRemovedSounds()
{
    for (auto it = m_removedSounds.begin(); it != m_removedSounds.end();)
    {
        FMOD_OPENSTATE state;
        (*it)->getOpenState(&state, nullptr, nullptr, nullptr);
        if (state == FMOD_OPENSTATE_READY || state == FMOD_OPENSTATE_PLAYING)
        {
            (*it)->release();
            it = m_removedSounds.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void FMODAudioEngine::updateReverb(FMODReverbPreset preset, bool force)
{
    if (m_reverbPreset != preset || force)
    {
        FMOD::DSP* dsp = nullptr;
        m_reverbChannel->getDSP(0, &dsp);
        if (!dsp) return;
        
        float values[12] = {
            100.0f, 7.0f, 11.0f, 5000.0f, 83.0f, 100.0f, 100.0f, 250.0f, 0.0f, 14500.0f, 96.0f, -8.0f
        };
        
        if (preset != 0)
        {
            int index = preset - 1;
            if (index >= 0 && index <= 21)
            {
                // Valores predefinidos para diferentes presets
            }
        }
        
        dsp->setParameterFloat(0, values[0]);
        dsp->setParameterFloat(1, values[1]);
        dsp->setParameterFloat(2, values[2]);
        dsp->setParameterFloat(3, values[3]);
        dsp->setParameterFloat(4, values[4]);
        dsp->setParameterFloat(5, values[5]);
        dsp->setParameterFloat(6, values[6]);
        dsp->setParameterFloat(7, values[7]);
        dsp->setParameterFloat(8, values[8]);
        dsp->setParameterFloat(9, values[9]);
        dsp->setParameterFloat(10, values[10]);
        dsp->setParameterFloat(11, values[11]);
        
        m_reverbPreset = preset;
    }
}

FMOD_OPENSTATE FMODAudioEngine::waitUntilSoundReady(FMOD::Sound* sound)
{
    if (!sound) return FMOD_OPENSTATE_ERROR;
    
    FMOD_OPENSTATE state;
    do
    {
        sound->getOpenState(&state, nullptr, nullptr, nullptr);
    }
    while ((state & 0xFFFFFFFD) != 0 && state != FMOD_OPENSTATE_PLAYING);
    
    return state;
}

bool FMODAudioEngine::isSoundReady(FMOD::Sound* sound)
{
    if (!sound) return false;
    
    FMOD_OPENSTATE state;
    sound->getOpenState(&state, nullptr, nullptr, nullptr);
    return (state & 0xFFFFFFFD) == 0 || state == FMOD_OPENSTATE_PLAYING;
}

int FMODAudioEngine::playEffect(const std::string& path)
{
    return playEffect(path, 1.0f, 0.0f, 1.0f);
}

int FMODAudioEngine::playEffect(const std::string& path, float volume, float pitch, float pan)
{
    FMOD::Sound* sound = nullptr;
    FMOD::Channel* channel = nullptr;
    
    auto it = m_fmodSounds.find(path);
    if (it != m_fmodSounds.end() && it->second.m_sound)
    {
        sound = it->second.m_sound;
        it->second.m_playCount++;
    }
    else
    {
        m_system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
        if (!sound) return 0;
    }
    
    m_system->playSound(sound, m_globalChannel, false, &channel);
    if (!channel) return 0;
    
    channel->setVolume(volume * m_sfxVolume);
    channel->setPitch(pitch);
    if (pan != 0.0f)
    {
        channel->setPan(pan);
    }
    
    int channelID = ++m_nextChannelID;
    registerChannel(channel, channelID, 0);
    
    return channelID;
}

int FMODAudioEngine::playEffectAsync(const std::string& path)
{
    return playEffectAsync(path, 1.0f, 0.0f, 1.0f);
}

int FMODAudioEngine::playEffectAsync(const std::string& path, float volume, float pitch, float pan)
{
    return playEffect(path, volume, pitch, pan);
}

int FMODAudioEngine::playEffectAdvanced(const std::string& path, float speed, float unk024, float volume, float pitch, 
                                        bool fft, bool reverb, int start, int end, int fadeIn, int fadeOut, 
                                        bool loop, int effectID, bool override, bool unk04d, int channelID, 
                                        int uniqueID, float minInterval, int group)
{
    // Implementación avanzada para efectos
    return playEffect(path, volume, pitch, 0.0f);
}

FMODSound* FMODAudioEngine::preloadEffect(const std::string& path)
{
    FMOD::Sound* sound = nullptr;
    m_system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
    if (!sound) return nullptr;
    
    return storeEffect(sound, path);
}

FMOD::Sound* FMODAudioEngine::preloadEffectAsync(const std::string& path)
{
    FMOD::Sound* sound = nullptr;
    m_system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
    if (sound)
    {
        m_soundPathToSound[path] = sound;
    }
    return sound;
}

FMODSound* FMODAudioEngine::storeEffect(FMOD::Sound* sound, const std::string& path)
{
    FMODSound fmodSound;
    fmodSound.m_sound = sound;
    fmodSound.m_filePath = path;
    fmodSound.m_playCount = 0;
    fmodSound.m_playIndex = 0;
    fmodSound.m_preloaded = false;
    fmodSound.m_loaded = true;
    
    sound->getLength(&fmodSound.m_length, FMOD_TIMEUNIT_MS);
    
    m_fmodSounds[path] = fmodSound;
    return &m_fmodSounds[path];
}

void FMODAudioEngine::queuedEffectFinishedLoading(const std::string& path)
{
    for (auto it = m_queuedEffects.begin(); it != m_queuedEffects.end(); ++it)
    {
        if (it->m_filePath == path)
        {
            FMODQueuedEffect effect = *it;
            m_queuedEffects.erase(it);
            
            playEffectAdvanced(
                effect.m_filePath,
                effect.m_speed,
                effect.m_unk024,
                effect.m_volume,
                effect.m_pitch,
                effect.m_fastFourierTransform,
                effect.m_reverb,
                effect.m_start,
                effect.m_end,
                effect.m_fadeIn,
                effect.m_fadeOut,
                effect.m_loop,
                effect.m_effectID,
                effect.m_override,
                effect.m_unk04d,
                effect.m_channelID,
                effect.m_uniqueID,
                effect.m_minInterval,
                effect.m_group
            );
            break;
        }
    }
}

void FMODAudioEngine::unloadAllEffects()
{
    stopAllEffects();
    
    for (auto& pair : m_fmodSounds)
    {
        if (!pair.second.m_preloaded)
        {
            m_removedSounds.push_back(pair.second.m_sound);
        }
    }
    m_fmodSounds.clear();
    m_nextChannelID = 0;
}

void FMODAudioEngine::unloadEffect(const std::string& path)
{
    auto it = m_fmodSounds.find(path);
    if (it != m_fmodSounds.end())
    {
        if (it->second.m_sound)
        {
            m_removedSounds.push_back(it->second.m_sound);
        }
        m_fmodSounds.erase(it);
    }
}

bool FMODAudioEngine::isEffectLoaded(const std::string& path)
{
    return m_fmodSounds.find(path) != m_fmodSounds.end();
}

void FMODAudioEngine::stopAllEffects()
{
    m_globalChannel->stop();
}

void FMODAudioEngine::pauseEffect(unsigned int effect)
{
    auto it = m_channelIDToChannel.find(effect);
    if (it != m_channelIDToChannel.end())
    {
        it->second->setPaused(true);
    }
}

void FMODAudioEngine::resumeEffect(unsigned int effect)
{
    auto it = m_channelIDToChannel.find(effect);
    if (it != m_channelIDToChannel.end())
    {
        it->second->setPaused(false);
    }
}

void FMODAudioEngine::pauseAllEffects()
{
    m_globalChannel->setPaused(true);
}

void FMODAudioEngine::resumeAllEffects()
{
    m_globalChannel->setPaused(false);
}

int FMODAudioEngine::countActiveEffects()
{
    return (int)m_channelIDToChannel.size() - countActiveMusic();
}

void FMODAudioEngine::loadMusic(const std::string& path)
{
    loadMusic(path, 1.0f, 0.0f, 1.0f, false, 0, false, false);
}

void FMODAudioEngine::loadMusic(const std::string& path, float pitch, float unknown, float volume, 
                                bool loop, int musicID, bool noPrepare, bool dontReset)
{
    FMOD::Sound* sound = nullptr;
    
    auto it = m_fmodMusic.find(musicID);
    if (it != m_fmodMusic.end() && it->second.m_filePath == path)
    {
        return;
    }
    
    sound = createStream(path);
    if (!sound) return;
    
    FMODMusic music;
    music.m_sound = sound;
    music.m_filePath = path;
    music.m_pitch = pitch;
    music.m_volume = volume;
    music.m_loop = loop;
    music.m_musicID = musicID;
    music.m_noPrepare = noPrepare;
    music.m_dontReset = dontReset;
    music.m_channelID = 0;
    music.m_isOgg = (path.find(".ogg") != std::string::npos);
    music.m_isPersistent = false;
    music.m_start = 0;
    music.m_end = 0;
    music.m_fadeIn = 0;
    music.m_fadeOut = 0;
    
    m_fmodMusic[musicID] = music;
}

FMOD::Sound* FMODAudioEngine::createStream(const std::string& path)
{
    FMOD::Sound* sound = nullptr;
    std::string fullPath = path;
    
    if (!fullPath.empty() && fullPath[0] != '/')
    {
        fullPath = "file://" + fullPath;
    }
    
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    
    bool isOgg = (path.find(".ogg") != std::string::npos);
    exinfo.suggestedsoundtype = isOgg ? FMOD_SOUND_TYPE_OGGVORBIS : FMOD_SOUND_TYPE_MEDIACODEC;
    
    FMOD_RESULT result = m_system->createStream(fullPath.c_str(), FMOD_CREATESTREAM | FMOD_NONBLOCKING, &exinfo, &sound);
    if (result != FMOD_OK)
    {
        return nullptr;
    }
    
    sound->setLoopCount(0);
    return sound;
}

void FMODAudioEngine::loadAndPlayMusic(const std::string& path, unsigned int time, int musicID)
{
    loadMusic(path);
    setMusicTimeMS(time, false, musicID);
    resumeMusic(musicID);
}

FMOD::Sound* FMODAudioEngine::preloadMusic(const std::string& path, bool noRelease, int musicID)
{
    FMOD::Sound* sound = createStream(path);
    if (!sound) return nullptr;
    
    if (noRelease || waitUntilSoundReady(sound) != FMOD_OPENSTATE_ERROR)
    {
        FMODMusic music;
        music.m_sound = sound;
        music.m_filePath = path;
        music.m_isOgg = (path.find(".ogg") != std::string::npos);
        music.m_isPersistent = false;
        music.m_channelID = 0;
        music.m_start = 0;
        music.m_end = 0;
        music.m_fadeIn = 0;
        music.m_fadeOut = 0;
        m_fmodMusic[musicID] = music;
        return sound;
    }
    else
    {
        sound->release();
        return nullptr;
    }
}

void FMODAudioEngine::stopAllMusic(bool clear)
{
    for (auto& pair : m_fmodMusic)
    {
        FMODMusic& music = pair.second;
        if (music.m_channelID > 0)
        {
            auto channelIt = m_channelIDToChannel.find(music.m_channelID);
            if (channelIt != m_channelIDToChannel.end())
            {
                channelIt->second->stop();
            }
        }
        if (clear && music.m_sound)
        {
            m_removedSounds.push_back(music.m_sound);
        }
    }
    if (clear)
    {
        m_fmodMusic.clear();
    }
}

void FMODAudioEngine::stopAndRemoveMusic(int id)
{
    auto it = m_fmodMusic.find(id);
    if (it != m_fmodMusic.end())
    {
        if (it->second.m_channelID > 0)
        {
            auto channelIt = m_channelIDToChannel.find(it->second.m_channelID);
            if (channelIt != m_channelIDToChannel.end())
            {
                channelIt->second->stop();
            }
        }
        if (it->second.m_sound)
        {
            m_removedSounds.push_back(it->second.m_sound);
        }
        m_fmodMusic.erase(it);
    }
}

void FMODAudioEngine::pauseMusic(int musicChannel)
{
    FMOD::Channel* channel = getActiveMusicChannel(musicChannel);
    if (channel)
    {
        channel->setPaused(true);
    }
}

void FMODAudioEngine::resumeMusic(int musicChannel)
{
    FMOD::Channel* channel = getActiveMusicChannel(musicChannel);
    if (channel)
    {
        channel->setPaused(false);
    }
}

void FMODAudioEngine::pauseAllMusic(bool force)
{
    for (auto& pair : m_fmodMusic)
    {
        if (force || !pair.second.m_isPersistent)
        {
            FMOD::Channel* channel = getActiveMusicChannel(pair.first);
            if (channel)
            {
                channel->setPaused(true);
            }
        }
    }
}

void FMODAudioEngine::resumeAllMusic()
{
    for (auto& pair : m_fmodMusic)
    {
        FMOD::Channel* channel = getActiveMusicChannel(pair.first);
        if (channel)
        {
            channel->setPaused(false);
        }
    }
}

int FMODAudioEngine::countActiveMusic()
{
    int count = 0;
    for (auto& pair : m_fmodMusic)
    {
        if (pair.second.m_channelID > 0)
        {
            count++;
        }
    }
    return count;
}

unsigned int FMODAudioEngine::getMusicLengthMS(int channel)
{
    auto it = m_fmodMusic.find(channel);
    if (it != m_fmodMusic.end() && it->second.m_sound)
    {
        unsigned int length = 0;
        it->second.m_sound->getLength(&length, FMOD_TIMEUNIT_MS);
        return length;
    }
    return 0;
}

unsigned int FMODAudioEngine::getMusicTimeMS(int channel)
{
    FMOD::Channel* ch = getActiveMusicChannel(channel);
    if (ch)
    {
        unsigned int pos = 0;
        ch->getPosition(&pos, FMOD_TIMEUNIT_MS);
        return pos;
    }
    return 0;
}

float FMODAudioEngine::getMusicTime(int channel)
{
    return (float)getMusicTimeMS(channel) / 1000.0f;
}

void FMODAudioEngine::setMusicTimeMS(unsigned int time, bool dontWait, int musicID)
{
    FMOD::Channel* channel = getActiveMusicChannel(musicID);
    if (channel)
    {
        int offset = 0;
        auto it = m_fmodMusic.find(musicID);
        if (it != m_fmodMusic.end() && it->second.m_isOgg)
        {
            offset = 0;
        }
        else
        {
            offset = 40;
        }
        channel->setPosition(time + m_musicOffset + offset, FMOD_TIMEUNIT_MS);
        
        if (!dontWait)
        {
            auto musicIt = m_fmodMusic.find(musicID);
            if (musicIt != m_fmodMusic.end() && musicIt->second.m_sound)
            {
                waitUntilSoundReady(musicIt->second.m_sound);
            }
        }
    }
}

FMOD::Channel* FMODAudioEngine::getActiveMusicChannel(int musicID)
{
    auto it = m_fmodMusic.find(musicID);
    if (it != m_fmodMusic.end() && it->second.m_channelID > 0)
    {
        auto channelIt = m_channelIDToChannel.find(it->second.m_channelID);
        if (channelIt != m_channelIDToChannel.end())
        {
            return channelIt->second;
        }
    }
    return nullptr;
}

bool FMODAudioEngine::isAnyPersistentPlaying()
{
    for (auto& pair : m_fmodMusic)
    {
        if (pair.second.m_isPersistent)
        {
            return true;
        }
    }
    return false;
}

bool FMODAudioEngine::isPersistentMatchPlaying(const std::string& path, int musicID)
{
    if (isMusicPlaying(musicID))
    {
        std::string activePath = getActiveMusic(musicID);
        if (activePath == path)
        {
            return m_fmodMusic[musicID].m_isPersistent;
        }
    }
    return false;
}

std::string FMODAudioEngine::getActiveMusic(int id)
{
    auto it = m_fmodMusic.find(id);
    if (it != m_fmodMusic.end())
    {
        return it->second.m_filePath;
    }
    return "";
}

int FMODAudioEngine::getMusicChannelID(int musicID)
{
    auto it = m_fmodMusic.find(musicID);
    if (it != m_fmodMusic.end())
    {
        return it->second.m_channelID;
    }
    return 0;
}

void FMODAudioEngine::swapMusicIndex(int musicID, int channelID)
{
    auto it1 = m_fmodMusic.find(musicID);
    auto it2 = m_fmodMusic.find(channelID);
    
    if (it1 != m_fmodMusic.end() && it2 != m_fmodMusic.end())
    {
        std::swap(it1->second, it2->second);
    }
}

void FMODAudioEngine::triggerQueuedMusic(FMODQueuedMusic* music)
{
    if (music->m_stopMusic)
    {
        if (isMusicPlaying(music->m_musicID))
        {
            int channelID = getMusicChannelID(music->m_musicID);
            if (channelID > 0)
            {
                auto stateIt = m_audioState.m_soundStateForChannels.find(channelID);
                if (stateIt != m_audioState.m_soundStateForChannels.end())
                {
                    stateIt->second.m_newChannelID = music->m_channelID;
                }
            }
        }
        
        if (isMusicPlaying(music->m_channelID))
        {
            int channelID = getMusicChannelID(music->m_channelID);
            if (channelID > 0)
            {
                auto stateIt = m_audioState.m_soundStateForChannels.find(channelID);
                if (stateIt != m_audioState.m_soundStateForChannels.end())
                {
                    stateIt->second.m_newChannelID = music->m_musicID;
                }
            }
        }
        
        stopMusic(music->m_channelID);
        swapMusicIndex(music->m_musicID, music->m_channelID);
        music->m_musicID = music->m_channelID;
    }
    
    startMusic(music->m_start, music->m_end, music->m_fadeIn, music->m_fadeOut,
               music->m_loop, music->m_musicID, false, music->m_dontReset);
}

void FMODAudioEngine::activateQueuedMusic(int channel)
{
    auto it1 = m_audioState.m_queuedMusicForChannels2.find(channel);
    if (it1 != m_audioState.m_queuedMusicForChannels2.end())
    {
        FMODQueuedMusic music = it1->second;
        triggerQueuedMusic(&music);
        m_audioState.m_queuedMusicForChannels2.erase(it1);
    }
    else
    {
        auto it2 = m_audioState.m_queuedMusicForChannels1.find(channel);
        if (it2 != m_audioState.m_queuedMusicForChannels1.end())
        {
            it2->second.m_queuedStatus = 1;
        }
    }
}

bool FMODAudioEngine::isMusicPlaying(int musicID)
{
    auto it = m_fmodMusic.find(musicID);
    if (it != m_fmodMusic.end() && it->second.m_channelID > 0)
    {
        auto channelIt = m_channelIDToChannel.find(it->second.m_channelID);
        if (channelIt != m_channelIDToChannel.end())
        {
            bool isPlaying = false;
            channelIt->second->isPlaying(&isPlaying);
            return isPlaying;
        }
    }
    return false;
}

void FMODAudioEngine::stopMusic(int musicID)
{
    auto it = m_fmodMusic.find(musicID);
    if (it != m_fmodMusic.end() && it->second.m_channelID > 0)
    {
        auto channelIt = m_channelIDToChannel.find(it->second.m_channelID);
        if (channelIt != m_channelIDToChannel.end())
        {
            channelIt->second->stop();
        }
    }
}

void FMODAudioEngine::startMusic(int start, int end, int fadeIn, int fadeOut, bool loop, int musicID, bool unknown, bool dontReset)
{
    FMOD::Channel* channel = getActiveMusicChannel(musicID);
    if (channel)
    {
        // Configurar loop points, fade, etc.
    }
}

void FMODAudioEngine::setBackgroundMusicVolume(float volume)
{
    m_musicVolume = volume;
    m_musicFadeStart = 0.0f;
    if (m_backgroundMusicChannel)
    {
        m_backgroundMusicChannel->setVolume(volume);
    }
}

void FMODAudioEngine::setEffectsVolume(float volume)
{
    m_sfxVolume = volume;
    if (m_globalChannel)
    {
        m_globalChannel->setVolume(volume);
    }
}

float FMODAudioEngine::getBackgroundMusicVolume()
{
    return m_musicVolume;
}

float FMODAudioEngine::getEffectsVolume()
{
    return m_sfxVolume;
}

void FMODAudioEngine::fadeInBackgroundMusic(float value)
{
    m_backgroundMusicChannel->setVolume(0.0f);
    m_backgroundMusicFade = value;
    m_musicFadeStart = -1.0f;
}

void FMODAudioEngine::fadeInMusic(float duration, int channel)
{
    FMOD::Channel* ch = getActiveMusicChannel(channel);
    if (ch)
    {
        stopAndGetFade(ch);
        
        unsigned long long dspClock = 0;
        ch->getDSPClock(0, &dspClock);
        
        ch->addFadePoint(dspClock, 0.0f);
        ch->addFadePoint((unsigned long long)((float)dspClock + (float)(m_sampleRate * duration)), 1.0f);
    }
}

void FMODAudioEngine::fadeOutMusic(float duration, int channel)
{
    FMOD::Channel* ch = getActiveMusicChannel(channel);
    if (ch)
    {
        float currentVolume = stopAndGetFade(ch);
        
        unsigned long long dspClock = 0;
        ch->getDSPClock(0, &dspClock);
        
        ch->addFadePoint(dspClock, currentVolume);
        ch->addFadePoint((unsigned long long)((float)dspClock + (float)(m_sampleRate * duration)), 0.0f);
    }
}

void FMODAudioEngine::fadeMusic(float duration, int channel, float startVolume, float endVolume)
{
    FMOD::Channel* ch = getActiveMusicChannel(channel);
    if (ch)
    {
        stopAndGetFade(ch);
        
        unsigned long long dspClock = 0;
        ch->getDSPClock(0, &dspClock);
        
        ch->addFadePoint(dspClock, startVolume);
        ch->addFadePoint((unsigned long long)((float)dspClock + (float)(m_sampleRate * duration)), endVolume);
    }
}

float FMODAudioEngine::stopAndGetFade(FMOD::Channel* channel)
{
    if (channel)
    {
        float volume = 1.0f;
        channel->getVolume(&volume);
        return volume;
    }
    return 1.0f;
}

void FMODAudioEngine::pauseAllAudio()
{
    if (!m_allAudioPaused)
    {
        m_allAudioPaused = true;
        m_backgroundMusicChannel->setPaused(true);
        m_globalChannel->setPaused(true);
    }
}

void FMODAudioEngine::resumeAllAudio()
{
    if (m_allAudioPaused)
    {
        m_allAudioPaused = false;
        m_backgroundMusicChannel->setPaused(false);
        m_globalChannel->setPaused(false);
    }
}

void FMODAudioEngine::clearAllAudio()
{
    stopAllEffects();
    stopAllMusic(true);
    resumeAllAudio();
}

void FMODAudioEngine::enableMetering()
{
    m_metering = true;
    m_pulse1 = 0.1f;
    m_pulse2 = 0.1f;
    m_pulse3 = 0.0f;
}

void FMODAudioEngine::disableMetering()
{
    m_metering = false;
}

float FMODAudioEngine::getMeteringValue()
{
    return 0.0f;
}

float FMODAudioEngine::pitchForIdx(int index)
{
    if (index == 0) return 1.0f;
    return 1.0f + (index * 0.0833f);
}

int FMODAudioEngine::getNextChannelID()
{
    return ++m_nextChannelID;
}

void FMODAudioEngine::registerChannel(FMOD::Channel* channel, int channelID, int effectID)
{
    channel->setUserData((void*)(intptr_t)channelID);
    m_channelIDToChannel[channelID] = channel;
    
    float volume = 0.0f;
    channel->getVolume(&volume);
    m_audioState.m_volumeForEffectChannels[channelID] = volume;
    m_audioState.m_volumeModForEffectChannels[channelID] = 1.0f;
    
    float pitch = 0.0f;
    channel->getPitch(&pitch);
    m_audioState.m_pitchForEffectChannels[channelID] = pitch;
    
    if (effectID != 0)
    {
        m_channelIDToEffectID[channelID] = effectID;
        m_effectIDToChannelID[effectID] = channelID;
    }
}

void FMODAudioEngine::unregisterChannel(int id)
{
    m_channelIDToChannel.erase(id);
    m_audioState.m_soundStateForChannels.erase(id);
    m_channelIDToEffectID.erase(id);
    m_effectIDToChannelID.erase(id);
    m_stoppedChannels.erase(id);
    channelUnlinkSound(id);
}

FMOD::Channel* FMODAudioEngine::channelForChannelID(int id)
{
    auto it = m_channelIDToChannel.find(id);
    if (it != m_channelIDToChannel.end())
    {
        return it->second;
    }
    return nullptr;
}

FMOD::Channel* FMODAudioEngine::channelForUniqueID(int id)
{
    int channelID = channelIDForUniqueID(id);
    return channelForChannelID(channelID);
}

int FMODAudioEngine::channelIDForUniqueID(int id)
{
    auto it = m_effectIDToChannelID.find(id);
    if (it != m_effectIDToChannelID.end())
    {
        return it->second;
    }
    return 0;
}

void FMODAudioEngine::channelLinkSound(int id, FMODSound* sound)
{
    sound->m_playIndex = ++m_nextChannelID;
    sound->m_playCount++;
    m_channelIDToSoundPath[id] = sound->m_filePath;
}

void FMODAudioEngine::channelUnlinkSound(int id)
{
    auto it = m_channelIDToSoundPath.find(id);
    if (it != m_channelIDToSoundPath.end())
    {
        auto soundIt = m_fmodSounds.find(it->second);
        if (soundIt != m_fmodSounds.end())
        {
            soundIt->second.m_playCount--;
        }
        m_channelIDToSoundPath.erase(it);
    }
}

void FMODAudioEngine::channelStopped(FMOD::Channel* channel, bool remove)
{
    int channelID = 0;
    channel->getUserData((void**)&channelID);
    
    unregisterChannel(channelID);
    
    if (remove)
    {
        auto musicIt = m_fmodMusic.find(channelID);
        if (musicIt != m_fmodMusic.end())
        {
            if (musicIt->second.m_sound)
            {
                m_removedSounds.push_back(musicIt->second.m_sound);
            }
            m_fmodMusic.erase(musicIt);
        }
    }
}

void FMODAudioEngine::setChannelVolume(int id, AudioTargetType type, float volume)
{
    if (type == SFXGroup)
    {
        m_audioState.m_volumeForEffectGroups[id] = volume;
        m_audioState.m_volumeModForEffectGroups[id] = 1.0f;
        FMOD::ChannelGroup* group = getChannelGroup(id, false);
        if (group)
        {
            group->setVolume(volume);
        }
        group = getChannelGroup(id, true);
        if (group)
        {
            group->setVolume(volume);
        }
        return;
    }
    
    if (type == MusicChannel)
    {
        m_audioState.m_volumeForMusicChannels[id] = volume;
        int channelID = getMusicChannelID(id);
        FMOD::Channel* ch = channelForChannelID(channelID);
        if (ch)
        {
            ch->setVolume(volume);
        }
        return;
    }
    
    m_audioState.m_volumeForEffectChannels[id] = volume;
    FMOD::Channel* ch = channelForChannelID(id);
    if (ch)
    {
        ch->setVolume(volume);
    }
}

void FMODAudioEngine::setChannelVolumeMod(int id, AudioTargetType type, float volumeMod)
{
    if (type == SFXGroup)
    {
        m_audioState.m_volumeModForEffectGroups[id] = volumeMod;
        m_audioState.m_volumeForEffectGroups[id] = 1.0f;
        FMOD::ChannelGroup* group = getChannelGroup(id, false);
        if (group)
        {
            group->setVolume(volumeMod);
        }
        group = getChannelGroup(id, true);
        if (group)
        {
            group->setVolume(volumeMod);
        }
        return;
    }
    
    if (type == MusicChannel)
    {
        m_audioState.m_volumeModForMusicChannels[id] = volumeMod;
        int channelID = getMusicChannelID(id);
        FMOD::Channel* ch = channelForChannelID(channelID);
        if (ch)
        {
            ch->setVolume(volumeMod);
        }
        return;
    }
    
    m_audioState.m_volumeModForEffectChannels[id] = volumeMod;
    FMOD::Channel* ch = channelForChannelID(id);
    if (ch)
    {
        ch->setVolume(volumeMod);
    }
}

void FMODAudioEngine::setChannelPitch(int id, AudioTargetType type, float pitch)
{
    if (type == SFXGroup)
    {
        m_audioState.m_pitchForEffectGroups[id] = pitch;
        FMOD::ChannelGroup* group = getChannelGroup(id, false);
        if (group)
        {
            group->setPitch(pitch);
        }
        group = getChannelGroup(id, true);
        if (group)
        {
            group->setPitch(pitch);
        }
        return;
    }
    
    if (type == MusicChannel)
    {
        m_audioState.m_pitchForMusicChannels[id] = pitch;
        int channelID = getMusicChannelID(id);
        FMOD::Channel* ch = channelForChannelID(channelID);
        if (ch)
        {
            ch->setPitch(pitch);
        }
        return;
    }
    
    m_audioState.m_pitchForEffectChannels[id] = pitch;
    FMOD::Channel* ch = channelForChannelID(id);
    if (ch)
    {
        ch->setPitch(pitch);
    }
}

FMOD::ChannelGroup* FMODAudioEngine::getChannelGroup(int id, bool reverb)
{
    FMOD::ChannelGroup* group = nullptr;
    
    if (reverb)
    {
        auto it = m_reverbChannelGroups.find(id);
        if (it != m_reverbChannelGroups.end())
        {
            return it->second;
        }
    }
    else
    {
        auto it = m_globalChannelGroups.find(id);
        if (it != m_globalChannelGroups.end())
        {
            return it->second;
        }
    }
    
    m_system->createChannelGroup(nullptr, &group);
    if (!group) return nullptr;
    
    if (!m_audioState.m_volumeForEffectGroups.count(id))
    {
        m_audioState.m_volumeForEffectGroups[id] = 1.0f;
    }
    if (!m_audioState.m_volumeModForEffectGroups.count(id))
    {
        m_audioState.m_volumeModForEffectGroups[id] = 1.0f;
    }
    if (!m_audioState.m_pitchForEffectGroups.count(id))
    {
        m_audioState.m_pitchForEffectGroups[id] = 1.0f;
    }
    
    if (reverb)
    {
        m_reverbChannel->addGroup(group);
        m_reverbChannelGroups[id] = group;
    }
    else
    {
        m_globalChannel->addGroup(group);
        m_globalChannelGroups[id] = group;
    }
    
    group->setVolume(m_audioState.m_volumeForEffectGroups[id] * m_audioState.m_volumeModForEffectGroups[id]);
    group->setPitch(m_audioState.m_pitchForEffectGroups[id]);
    
    return group;
}

void FMODAudioEngine::stopChannel(int id)
{
    FMOD::Channel* ch = channelForChannelID(id);
    if (ch)
    {
        ch->stop();
    }
    m_stoppedChannels.insert(id);
}

bool FMODAudioEngine::isChannelStopping(int channel)
{
    return m_stoppedChannels.find(channel) != m_stoppedChannels.end();
}

void FMODAudioEngine::stopChannelTween(int id, AudioTargetType target, AudioModType mod)
{
    std::map<std::pair<int, int>, FMODSoundTween>* container = nullptr;
    
    if (target == SFXChannel)
    {
        container = &m_audioState.m_tweensForEffectChannels;
    }
    else if (target == MusicChannel)
    {
        container = &m_audioState.m_tweensForMusicChannels;
    }
    else if (target == SFXGroup)
    {
        container = &m_audioState.m_tweensForEffectGroups;
    }
    
    if (!container) return;
    
    for (auto it = container->begin(); it != container->end();)
    {
        if (it->first.first == id && it->second.m_type == mod)
        {
            it = container->erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void FMODAudioEngine::stopChannelTweens(int id, AudioTargetType target)
{
    stopChannelTween(id, target, Volume);
    stopChannelTween(id, target, Pitch);
}

std::map<std::pair<int, int>, FMODSoundTween>* FMODAudioEngine::getTweenContainer(AudioTargetType type)
{
    if (type == SFXChannel)
    {
        return &m_audioState.m_tweensForEffectChannels;
    }
    else if (type == MusicChannel)
    {
        return &m_audioState.m_tweensForMusicChannels;
    }
    else if (type == SFXGroup)
    {
        return &m_audioState.m_tweensForEffectGroups;
    }
    return nullptr;
}

void FMODAudioEngine::saveAudioState(FMODAudioState* state)
{
    state->m_elapsed = m_audioState.m_elapsed;
    state->m_interval = m_audioState.m_interval;
    state->m_unkUint64_1 = m_audioState.m_unkUint64_1;
    
    state->m_tweensForEffectChannels = m_audioState.m_tweensForEffectChannels;
    state->m_tweensForEffectGroups = m_audioState.m_tweensForEffectGroups;
    state->m_tweensForMusicChannels = m_audioState.m_tweensForMusicChannels;
    
    state->m_volumeForEffectChannels = m_audioState.m_volumeForEffectChannels;
    state->m_volumeModForEffectChannels = m_audioState.m_volumeModForEffectChannels;
    state->m_pitchForEffectChannels = m_audioState.m_pitchForEffectChannels;
    
    state->m_volumeForEffectGroups = m_audioState.m_volumeForEffectGroups;
    state->m_volumeModForEffectGroups = m_audioState.m_volumeModForEffectGroups;
    state->m_pitchForEffectGroups = m_audioState.m_pitchForEffectGroups;
    
    state->m_volumeForMusicChannels = m_audioState.m_volumeForMusicChannels;
    state->m_volumeModForMusicChannels = m_audioState.m_volumeModForMusicChannels;
    state->m_pitchForMusicChannels = m_audioState.m_pitchForMusicChannels;
    
    state->m_intervalForEffects = m_audioState.m_intervalForEffects;
    state->m_queuedMusicForChannels1 = m_audioState.m_queuedMusicForChannels1;
    state->m_queuedMusicForChannels2 = m_audioState.m_queuedMusicForChannels2;
    state->m_soundStateForChannels = m_audioState.m_soundStateForChannels;
}

void FMODAudioEngine::loadAudioState(FMODAudioState* state)
{
    m_audioState.m_elapsed = state->m_elapsed;
    m_audioState.m_interval = state->m_interval;
    m_audioState.m_unkUint64_1 = state->m_unkUint64_1;
    
    m_audioState.m_tweensForEffectChannels = state->m_tweensForEffectChannels;
    m_audioState.m_tweensForEffectGroups = state->m_tweensForEffectGroups;
    m_audioState.m_tweensForMusicChannels = state->m_tweensForMusicChannels;
    
    m_audioState.m_volumeForEffectChannels = state->m_volumeForEffectChannels;
    m_audioState.m_volumeModForEffectChannels = state->m_volumeModForEffectChannels;
    m_audioState.m_pitchForEffectChannels = state->m_pitchForEffectChannels;
    
    m_audioState.m_volumeForEffectGroups = state->m_volumeForEffectGroups;
    m_audioState.m_volumeModForEffectGroups = state->m_volumeModForEffectGroups;
    m_audioState.m_pitchForEffectGroups = state->m_pitchForEffectGroups;
    
    m_audioState.m_volumeForMusicChannels = state->m_volumeForMusicChannels;
    m_audioState.m_volumeModForMusicChannels = state->m_volumeModForMusicChannels;
    m_audioState.m_pitchForMusicChannels = state->m_pitchForMusicChannels;
    
    m_audioState.m_intervalForEffects = state->m_intervalForEffects;
    m_audioState.m_queuedMusicForChannels1 = state->m_queuedMusicForChannels1;
    m_audioState.m_queuedMusicForChannels2 = state->m_queuedMusicForChannels2;
    m_audioState.m_soundStateForChannels = state->m_soundStateForChannels;
}

FMODAudioEngine::~FMODAudioEngine()
{
    if (m_system)
    {
        m_system->release();
        m_system = nullptr;
    }
}