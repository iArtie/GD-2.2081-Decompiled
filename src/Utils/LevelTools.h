#pragma once

#include "Platform/cocos2d.h"
#include <string>

class LevelTools {
public:
    static int artistForAudio(int id);
    static std::string base64DecodeString(std::string str);
    static std::string base64EncodeString(std::string str);
    static cocos2d::CCDictionary * createStarPackDict();
    static std::string fbURLForArtist(int id);
    static int getAudioBPM(int id);
    static std::string getAudioFileName(int id);
    static std::string getAudioString(int id);
    static std::string getAudioTitle(int id);
    static bool getLastGameplayReversed();
    static bool getLastGameplayRotated();
    static float getLastTimewarp();
    static GJGameLevel * getLevel(int id, bool noString);
    static std::unordered_set<int> * getLevelList(std::unordered_set<int> *__return_ptr retstr);
    static SongInfoObject * getSongObject(int id);
    static void moveTriggerObjectsToArray(cocos2d::CCArray *objects, cocos2d::CCDictionary *objectsDict, int index);
    static std::string nameForArtist(int id);
    static std::string ngURLForArtist(int id);
    static float offsetBPMForTrack(int id);
    static void sortSpeedObjects(cocos2d::CCArray *objects, GJBaseGameLayer *layer);
    static void toggleDebugLogging(bool enabled);
    static std::string urlForAudio(int id);
    static float valueForSpeedMod(int speed);
    static bool verifyLevelIntegrity(std::string str, int id);
    static std::string ytURLForArtist(int id);
};
