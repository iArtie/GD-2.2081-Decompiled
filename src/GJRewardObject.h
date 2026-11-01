#pragma once

#include "cocos2d.h"

class DS_Dictionary;

enum class SpecialRewardItem {
    FireShard = 0,
    IceShard = 1,
    PoisonShard = 2,
    ShadowShard = 3,
    LavaShard = 4,
    BonusKey = 5,
    EarthShard = 6,
    BloodShard = 7,
    MetalShard = 8,
    LightShard = 9,
    SoulShard = 10,
    GoldKey = 11
};

enum class UnlockType {
    Unknown = 0
};

class GJRewardObject : public cocos2d::CCObject {
public:
    bool canEncode();
    static GJRewardObject* create();
    static GJRewardObject* createItemUnlock(UnlockType type, int id);
    static GJRewardObject* createWithCoder(DS_Dictionary* dict);
    void dataLoaded(DS_Dictionary* dict);
    void encodeWithCoder(DS_Dictionary* dict);
    bool init(SpecialRewardItem specialRewardItem, int total, int itemID);
    bool isSpecialType();
    virtual ~GJRewardObject();

    SpecialRewardItem m_specialRewardItem;
    int m_itemID;
    int m_total;
    UnlockType m_unlockType;
};