#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJEffectManager;
class DS_Dictionary;

class LevelSettingsObject : public cocos2d::CCNode {
public:
    static LevelSettingsObject* create();
    std::string getSaveString();
    bool init() override;
    static LevelSettingsObject* objectFromDict(cocos2d::CCDictionary* dict);
    static LevelSettingsObject* objectFromString(const std::string& str);
    void setupColorsFromLegacyMode(cocos2d::CCDictionary* dict);
    bool shouldUseYSection();
    ~LevelSettingsObject() override;

private:
    bool m_platformerMode = false;
    bool m_dynamicLevelHeight = false;
    GJEffectManager* m_effectManager = nullptr;
    bool m_startsWithStartPos = false;
    double m_songOffset = 0.0;
    int m_fadeIn = 0;
    int m_fadeOut = 0;
    std::string m_guidelineString;
    int m_backgroundIndex = 0;
    int m_groundIndex = 0;
    int m_middleGroundIndex = 0;
    int m_groundLineIndex = 0;
    int m_fontIndex = 0;
    int m_colorPage = 0;
    int m_startMode = 0;
    bool m_startMini = false;
    bool m_startDual = false;
    int m_startSpeed = 0;
    bool m_twoPlayerMode = false;
    bool m_propertykA23 = false;
    bool m_propertykA24 = false;
    bool m_allowMultiRotation = false;
    bool m_enable22Changes = false;
    bool m_enableImpulseFix = false;
    bool m_allowStaticRotate = false;
    bool m_reverseSync = false;
    bool m_mirrorMode = false;
    bool m_rotateGameplay = false;
    bool m_enablePlayerSqueeze = false;
    bool m_fixGravityBug = false;
    int m_spawnGroup = 0;
    bool m_noTimePenalty = false;
    int m_propertykA44 = 0;
    bool m_decreaseBoostSlide = false;
    bool m_dontReset = false;
    int m_nextFreeID = 0;
    bool m_fixNegativeScale = false;
    bool m_fixRobotJump = false;
    bool m_resetCamera = false;
    bool m_sortGroups = false;
    bool m_fixRadiusCollision = false;
    int m_targetOrder = 0;
    int m_targetChannel = 0;
    bool m_reverseGameplay = false;
    bool m_disableStartPos = false;
    bool m_isFlipped = false;
};