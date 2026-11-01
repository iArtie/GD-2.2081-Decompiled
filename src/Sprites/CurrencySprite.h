#pragma once

#include "Platform/cocos2d.h"
#include <string>

enum CurrencySpriteType {
    Orb = 1,
    Star = 2,
    Diamond = 3,
    FireShard = 4,
    IceShard = 5,
    PoisonShard = 6,
    ShadowShard = 7,
    LavaShard = 8,
    Key = 9,
    Shard2_1 = 10,
    Shard2_2 = 11,
    Shard2_3 = 12,
    Shard2_4 = 13,
    Shard2_5 = 14,
    Moon = 15,
    GoldKey = 16
};

class CurrencySprite : public cocos2d::CCSprite {
public:
    static CurrencySprite* create(CurrencySpriteType type, bool burst);
    static CurrencySprite* createWithSprite(cocos2d::CCSprite* sprite);
    
    bool init(CurrencySpriteType type, bool burst);
    bool initWithSprite(cocos2d::CCSprite* sprite);
    
    static CurrencySpriteType rewardToSpriteType(int rewardType);
    static std::string spriteTypeToStat(CurrencySpriteType type);
    
    virtual ~CurrencySprite();

protected:
    cocos2d::CCSprite* m_burstSprite;
    CurrencySpriteType m_spriteType;
};