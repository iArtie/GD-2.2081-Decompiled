// GJChallengeItem.h
#pragma once

#include "Platform/cocos2d.h"
#include <string>

class DS_Dictionary;

enum class GJChallengeType {
    Unknown = 0,
    // Other challenge types based on usage
};

struct GJChallengeValue {
    int value;
    int seed;
    int random;
};

class GJChallengeItem : public cocos2d::CCObject {
public:
    bool canEncode() override;
    
    static GJChallengeItem* create();
    static GJChallengeItem* createFromString(std::string string);
    static GJChallengeItem* createWithCoder(DS_Dictionary* dsdict);
    
    void dataLoaded(DS_Dictionary* dsdict);
    void encodeWithCoder(DS_Dictionary* dsdict);
    void incrementCount(int add);
    
    virtual ~GJChallengeItem();

private:
    GJChallengeType m_challengeType;
    int m_timeLeft;
    bool m_canClaim;
    std::string m_name;
    GJChallengeValue m_count;
    GJChallengeValue m_goal;
    GJChallengeValue m_reward;
    int m_position;
};