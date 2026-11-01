// GJChallengeItem.cpp
#include "Models/GJChallengeItem.h"
#include "Utils/DS_Dictionary.h"
#include <cstdlib>

bool GJChallengeItem::canEncode() {
    return true;
}

GJChallengeItem* GJChallengeItem::create() {
    return GJChallengeItem::create(0, GJChallengeType::Unknown, 0, 0, "");
}

GJChallengeItem* GJChallengeItem::createFromString(std::string string) {
    cocos2d::CCArray* array = cocos2d::CCString::split(string, ",");
    
    if (array->count() <= 4) {
        return nullptr;
    }
    
    int timeLeft = static_cast<cocos2d::CCString*>(array->objectAtIndex(0))->intValue();
    int challengeType = static_cast<cocos2d::CCString*>(array->objectAtIndex(1))->intValue();
    int position = static_cast<cocos2d::CCString*>(array->objectAtIndex(2))->intValue();
    int reward = static_cast<cocos2d::CCString*>(array->objectAtIndex(3))->intValue();
    std::string name = static_cast<cocos2d::CCString*>(array->objectAtIndex(4))->getCString();
    
    return GJChallengeItem::create(timeLeft, static_cast<GJChallengeType>(challengeType), 
                                   position, reward, name);
}

GJChallengeItem* GJChallengeItem::createWithCoder(DS_Dictionary* dsdict) {
    GJChallengeItem* item = GJChallengeItem::create();
    item->dataLoaded(dsdict);
    return item;
}

void GJChallengeItem::dataLoaded(DS_Dictionary* dsdict) {
    m_challengeType = static_cast<GJChallengeType>(dsdict->getIntegerForKey("1"));
    m_timeLeft = dsdict->getIntegerForKey("5");
    m_canClaim = dsdict->getBoolForKey("6");
    m_name = dsdict->getStringForKey("7");
    
    // Initialize count value
    int countValue = dsdict->getIntegerForKey("F78BA8");
    m_count.value = countValue;
    m_count.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
    m_count.random = countValue + m_count.seed;
    
    // Initialize goal value
    int goalValue = dsdict->getIntegerForKey("3");
    m_goal.value = goalValue;
    m_goal.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
    m_goal.random = goalValue + m_goal.seed;
    
    // Initialize reward value
    int rewardValue = dsdict->getIntegerForKey("F2CA20");
    m_reward.value = rewardValue;
    m_reward.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
    m_reward.random = rewardValue + m_reward.seed;
    
    m_position = dsdict->getIntegerForKey("8");
}

void GJChallengeItem::encodeWithCoder(DS_Dictionary* dsdict) {
    dsdict->setIntegerForKey("kCEK", 7);
    dsdict->setIntegerForKey("1", static_cast<int>(m_challengeType));
    dsdict->setIntegerForKey("5", m_timeLeft);
    dsdict->setBoolForKey("6", m_canClaim);
    dsdict->setStringForKey("7", m_name);
    dsdict->setIntegerForKey("F78BA8", m_count.random - m_count.seed);
    dsdict->setIntegerForKey("3", m_goal.random - m_goal.seed);
    dsdict->setIntegerForKey("F2CA20", m_reward.random - m_reward.seed);
    dsdict->setIntegerForKey("8", m_position);
}

void GJChallengeItem::incrementCount(int add) {
    if (!m_canClaim) {
        int newCount = add + m_count.random - m_count.seed;
        int goal = m_goal.random - m_goal.seed;
        
        if (newCount >= goal) {
            newCount = goal;
            m_canClaim = true;
        }
        
        m_count.value = newCount;
        m_count.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
        m_count.random = newCount + m_count.seed;
    }
}

GJChallengeItem::~GJChallengeItem() {
    // m_name destructor handles string cleanup
    cocos2d::CCObject::~CCObject();
}