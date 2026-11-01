#pragma once

#include "Platform/cocos2d.h"
#include <string>

class DS_Dictionary;
class GJRewardObject;

enum class GJRewardType {
    // Reward types
    Unknown = 0
};

enum class SpecialRewardItem {
    FireShard = 1,
    IceShard = 2,
    PoisonShard = 3,
    ShadowShard = 4,
    LavaShard = 5,
    EarthShard = 6,
    BloodShard = 7,
    MetalShard = 8,
    LightShard = 9,
    SoulShard = 10,
    BonusKey = 11,
    GoldKey = 12
};

class GJRewardItem : public cocos2d::CCObject {
public:
    bool canEncode();
    static GJRewardItem* create();
    static GJRewardItem* createWithCoder(DS_Dictionary* dict);
    static GJRewardItem* createWithObject(GJRewardType type, GJRewardObject* object);
    static GJRewardItem* createWithObjects(GJRewardType type, cocos2d::CCArray* objects);
    void dataLoaded(DS_Dictionary* dict);
    void encodeWithCoder(DS_Dictionary* dict);
    static SpecialRewardItem getNextShardType(SpecialRewardItem type);
    static SpecialRewardItem getRandomNonMaxShardType();
    static SpecialRewardItem getRandomShardType();
    int getRewardCount(SpecialRewardItem type);
    GJRewardObject* getRewardObjectForType(SpecialRewardItem type);
    bool init(int chestID, int timeRemaining, const std::string& rewardStr);
    static bool isShardType(SpecialRewardItem rewardItem);
    static std::string rewardItemToStat(SpecialRewardItem rewardItem);
    virtual ~GJRewardItem();

    int m_chestID;
    GJRewardType m_rewardType;
    cocos2d::CCArray* m_rewardObjects;
    int m_timeRemaining;
};