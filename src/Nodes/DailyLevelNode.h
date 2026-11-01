#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include <string>

class GJGameLevel;
class DailyLevelPage;
class CCMenuItemSpriteExtra;

class DailyLevelNode : public cocos2d::CCNode, public FLAlertLayerProtocol {
public:
    static DailyLevelNode* create(GJGameLevel* level, DailyLevelPage* page, bool isNew);
    bool init(GJGameLevel* level, DailyLevelPage* page, bool isNew);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void onClaimReward(cocos2d::CCObject* sender);
    void onSkipLevel(cocos2d::CCObject* sender);
    void showSkipButton();
    void updateTimeLabel(std::string text);
    
    virtual ~DailyLevelNode();

protected:
    GJGameLevel* m_level;
    DailyLevelPage* m_page;
    CCMenuItemSpriteExtra* m_skipButton;
    cocos2d::CCLabelBMFont* m_timeLabel;
    cocos2d::CCPoint m_unkPoint;
    bool m_unkBool;
    bool m_needsDownloading;
};