#pragma once

#include "cocos2d.h"
#include <string>
#include <vector>

class DS_Dictionary;
class GJItemIcon;

enum class UnlockType
{
    Col1 = 0,
    Col2 = 1,
    Cube = 2,
    Ship = 3,
    Ball = 4,
    Ufo = 5,
    Wave = 6,
    Robot = 7,
    Spider = 8,
    Swing = 9,
    Jetpack = 10,
    Death = 11,
    Streak = 12,
    ShipFire = 13,
    Special = 14,
    GJItem = 15
};

class AchievementManager : public cocos2d::CCNode
{
public:
    static AchievementManager* sharedState();

    bool init();
    ~AchievementManager();

    std::string achievementForUnlock(int id, UnlockType type);
    void addManualAchievements();
    bool areAchievementsEarned(cocos2d::CCArray* achievements);
    void checkAchFromUnlock(const char* id);
    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    void firstSetup();
    cocos2d::CCDictionary* getAchievementRewardDict();
    cocos2d::CCDictionary* getAchievementsWithID(const char* id);
    cocos2d::CCArray* getAllAchievements();
    cocos2d::CCArray* getAllAchievementsSorted(bool available);
    bool isAchievementAvailable(std::string id);
    bool isAchievementEarned(const char* ach);
    int limitForAchievement(std::string id);
    void notifyAchievementWithID(const char* id);
    int percentForAchievement(const char* id);
    int percentageForCount(int count, int total);
    void reportPlatformAchievementWithID(const char* id, int percent);
    void resetAchievement(const char* id);
    void resetAchievements();
    void setup();
    void storeAchievementUnlocks();

private:
    void addAchievement(std::string id, std::string title, std::string achievedDesc, std::string unachievedDesc, std::string icon, int limits);
    std::string getPrefixForType(UnlockType type);
    static int achievementCompare(const void* a, const void* b);

public:
    bool m_dontNotify;
    cocos2d::CCDictionary* m_reportedAchievements;
    cocos2d::CCDictionary* m_platformAchievements;
    cocos2d::CCDictionary* m_achievementUnlocks;
    cocos2d::CCDictionary* m_unAchieved;
    cocos2d::CCArray* m_allAchievements;
    cocos2d::CCArray* m_allAchievementsSorted;
    int m_order;
    void* m_unkPtrUnused;
};