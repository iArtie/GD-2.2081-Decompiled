#pragma once

#include "cocos2d.h"
#include "AchievementBar.h"

class AchievementNotifier : public cocos2d::CCNode
{
public:
    static AchievementNotifier* sharedState();

    bool init();
    ~AchievementNotifier();

    void achievementDisplayFinished();
    void showNextAchievement();
    void willSwitchToScene(cocos2d::CCScene* scene);

private:
    cocos2d::CCArray* m_achievementBarArray;
    AchievementBar* m_activeAchievementBar;
    cocos2d::CCScene* m_nextScene;
};