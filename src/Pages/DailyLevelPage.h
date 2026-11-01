#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/GJDailyLevelDelegate.h"
#include "Delegates/LevelDownloadDelegate.h"
#include <string>

class GJGameLevel;
class DailyLevelNode;
class LoadingCircle;

enum GJTimedLevelType {
    Daily = 0,
    Weekly = 1,
    Event = 2
};

enum GJErrorCode {
    UpdateApp = 0
};

class DailyLevelPage : public FLAlertLayer, public GJDailyLevelDelegate, public LevelDownloadDelegate {
public:
    static DailyLevelPage* create(GJTimedLevelType type);
    bool init(GJTimedLevelType type);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void dailyStatusFailed(GJTimedLevelType type, GJErrorCode errorType) override;
    void dailyStatusFinished(GJTimedLevelType type) override;
    void levelDownloadFailed(int response) override;
    void levelDownloadFinished(GJGameLevel* level) override;
    
    void keyBackClicked() override;
    void registerWithTouchDispatcher() override;
    void show() override;
    
    void createDailyNode(GJGameLevel* level, bool instant, float delay, bool isNew);
    void createNodeIfLoaded();
    void downloadAndCreateNode();
    void exitDailyNode(DailyLevelNode* node, float delay);
    int getDailyTime();
    std::string getDailyTimeString(int timeLeft);
    void onClose(cocos2d::CCObject* sender);
    void onTheSafe(cocos2d::CCObject* sender);
    void refreshDailyPage();
    void skipDailyLevel(DailyLevelNode* node, GJGameLevel* level);
    void tryGetDailyStatus();
    void updateTimers(float dt);
    
    virtual ~DailyLevelPage();

    GJTimedLevelType m_type;
    DailyLevelNode* m_dailyNode;

protected:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCNode* m_scene;
    cocos2d::CCLabelBMFont* m_timeLabel;
    LoadingCircle* m_timeCircle;
    LoadingCircle* m_nodeCircle;
    int m_ZOrder;
    int m_joystickConnected;
    bool m_noElasticity;
    bool m_gettingDailyStatus;
    bool m_downloadStarted;
    int m_downloadLevelID;
};