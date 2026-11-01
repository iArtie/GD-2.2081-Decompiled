#pragma once

#include "cocos2d.h"
#include "CurrencyRewardDelegate.h"

class CurrencyRewardLayer : public cocos2d::CCLayer {
public:
    static CurrencyRewardLayer* create(CurrencyRewardDelegate* delegate, int rewardCount, float time, cocos2d::CCArray* objects);
    bool init(CurrencyRewardDelegate* delegate, int rewardCount, float time, cocos2d::CCArray* objects);
    
    void incrementCount(int count);
    void incrementDiamondsCount(int count);
    void incrementMoonsCount(int count);
    void incrementSpecialCount1(int count);
    void incrementSpecialCount2(int count);
    void incrementStarsCount(int count);
    void pulseSprite(cocos2d::CCSprite* sprite);
    void update(float dt) override;
    
    virtual ~CurrencyRewardLayer();

protected:
    CurrencyRewardDelegate* m_delegate;
    cocos2d::CCNode* m_mainNode;
    cocos2d::CCArray* m_objects;
    float m_time;
    int m_rewardCount;
    float m_elapsed;
    
    // Orbs
    cocos2d::CCLabelBMFont* m_orbsLabel;
    cocos2d::CCSprite* m_orbsSprite;
    int m_orbs;
    
    // Stars
    cocos2d::CCLabelBMFont* m_starsLabel;
    cocos2d::CCSprite* m_starsSprite;
    int m_stars;
    
    // Moons
    cocos2d::CCLabelBMFont* m_moonsLabel;
    cocos2d::CCSprite* m_moonsSprite;
    int m_moons;
    
    // Diamonds
    cocos2d::CCLabelBMFont* m_diamondsLabel;
    cocos2d::CCSprite* m_diamondsSprite;
    int m_diamonds;
    
    // Keys (Special 1)
    cocos2d::CCLabelBMFont* m_keysLabel;
    cocos2d::CCSprite* m_keysSprite;
    int m_keys;
    
    // Shards (Special 2)
    cocos2d::CCLabelBMFont* m_shardsLabel;
    cocos2d::CCSprite* m_shardsSprite;
    int m_shards;
};