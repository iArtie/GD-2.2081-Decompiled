#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJChallengeItem;
class ChallengesPage;

class ChallengeNode : public cocos2d::CCNode {
public:
    static ChallengeNode* create(GJChallengeItem* challengeItem, ChallengesPage* challengesPage, bool isNew);

    void onClaimReward(cocos2d::CCObject* sender);
    void updateTimeLabel(std::string text);

    ~ChallengeNode();

private:
    cocos2d::CCLabelBMFont* m_countdownLabel;
    GJChallengeItem* m_challengeItem;
    ChallengesPage* m_challengesPage;
    cocos2d::CCPoint m_unkPoint;
    bool m_unloaded;
};