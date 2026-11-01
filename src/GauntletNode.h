#pragma once

#include "cocos2d.h"
#include <string>

class GJMapPack;
class GJChestSprite;

enum GauntletType {
    Fire = 1,
    Ice = 2,
    Poison = 3,
    Shadow = 4,
    Lava = 5,
    Bonus = 6,
    Chaos = 7,
    Demon = 8,
    Time = 9,
    Crystal = 10,
    Magic = 11,
    Spike = 12,
    Monster = 13,
    Doom = 14,
    Death = 15,
    Forest = 16,
    Rune = 17,
    Force = 18,
    Spooky = 19,
    Dragon = 20,
    Water = 21,
    Haunted = 22,
    Acid = 23,
    Witch = 24,
    Power = 25,
    Potion = 26,
    Snake = 27,
    Toxic = 28,
    Halloween = 29,
    Treasure = 30,
    Ghost = 31,
    Spider = 32,
    Gem = 33,
    Inferno = 34,
    Portal = 35,
    Strange = 36,
    Fantasy = 37,
    Christmas = 38,
    Surprise = 39,
    Mystery = 40,
    Cursed = 41,
    Cyborg = 42,
    Castle = 43,
    Grave = 44,
    Temple = 45,
    World = 46,
    Galaxy = 47,
    Universe = 48,
    Discord = 49,
    Split = 50,
    NCS_I = 51,
    NCS_II = 52,
    Space = 53,
    Cosmos = 54,
    Random = 55,
    Chance = 56,
    Future = 57,
    Utopia = 58,
    Cinema = 59,
    Love = 60
};

class GauntletNode : public cocos2d::CCNode {
public:
    static GauntletNode* create(GJMapPack* gauntlet);
    static std::string frameForType(GauntletType type);
    static std::string nameForType(GauntletType type);
    
    bool init(GJMapPack* gauntlet);
    void generateNode();
    void onClaimReward();
    void onUnlock();
    void showUnlockAnimation();
    virtual ~GauntletNode();

private:
    cocos2d::CCNode* m_gauntletInfoNode;
    cocos2d::CCNode* m_rewardNode;
    GJMapPack* m_gauntlet;
    cocos2d::ccColor3B m_labelColor;
    cocos2d::ccColor3B m_backgroundColor;
    bool m_locked;
};