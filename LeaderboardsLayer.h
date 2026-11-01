#pragma once

#include "cocos2d.h"
#include <string>

enum class LeaderboardType {
    Default = 0,
    Top100 = 1,
    Friends = 2,
    Global = 3,
    Creator = 4
};

enum class LeaderboardStat {
    Stars = 0,
    Moons = 1,
    Demons = 2,
    UserCoins = 3
};

class LoadingCircle;
class TextArea;
class GJListLayer;
class CCMenuItemToggler;

class LeaderboardsLayer : public cocos2d::CCLayer {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    static LeaderboardsLayer* create(LeaderboardType type, LeaderboardStat stat);
    bool init(LeaderboardType type, LeaderboardStat stat) override;
    bool isCorrect(const std::string& key);
    void keyBackClicked() override;
    void loadLeaderboardFailed(const char* key);
    void loadLeaderboardFinished(cocos2d::CCArray* scores, const char* key);
    void onBack(cocos2d::CCObject* sender);
    void onCreators(cocos2d::CCObject* sender);
    void onGlobal(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onStat(cocos2d::CCObject* sender);
    void onTop(cocos2d::CCObject* sender);
    void onWeek(cocos2d::CCObject* sender);
    void refreshTabs();
    static cocos2d::CCScene* scene(LeaderboardType type, LeaderboardStat stat);
    void selectLeaderboard(LeaderboardType type, LeaderboardStat stat);
    void setupLevelBrowser(cocos2d::CCArray* scores);
    void setupTabs();
    void toggleTabButtons();
    void updateUserScoreFailed();
    void updateUserScoreFinished();
    virtual ~LeaderboardsLayer();

private:
    LoadingCircle* m_circle;
    TextArea* m_noInternet;
    LeaderboardType m_type;
    LeaderboardStat m_stat;
    CCMenuItemToggler* m_topBtn;
    CCMenuItemToggler* m_friendsBtn;
    CCMenuItemToggler* m_globalBtn;
    CCMenuItemToggler* m_creatorsBtn;
    cocos2d::CCArray* m_modeButtons;
    GJListLayer* m_list;
    cocos2d::CCArray* m_userScores;
    cocos2d::CCArray* m_tabs;
};