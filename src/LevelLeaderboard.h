#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "LevelManagerDelegate.h"
#include "GJEnums.h"

class GJGameLevel;
class LoadingCircle;
class TextArea;
class CCMenuItemSpriteExtra;
class GJCommentListLayer;
class GJLocalScore;

class LevelLeaderboard : public FLAlertLayer, public LevelManagerDelegate {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    static LevelLeaderboard* create(GJGameLevel* level, LevelLeaderboardType type, LevelLeaderboardMode mode);
    bool init(GJGameLevel* level, LevelLeaderboardType type, LevelLeaderboardMode mode);
    CCMenuItemSpriteExtra* getSpriteButton(const char* frame, cocos2d::SEL_MenuHandler callback, cocos2d::CCPoint offset, int buttonType, cocos2d::CCMenu* menu, float scale);
    void deleteLocalScores();
    cocos2d::CCArray* getLocalScores();
    bool isCorrect(const char* key);
    void keyBackClicked();
    void loadLeaderboardFailed(const char* key);
    void loadLeaderboardFinished(cocos2d::CCArray* scores, const char* key);
    void loadScores();
    void onChangeMode(cocos2d::CCObject* sender);
    void onChangeType(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onDeleteLocalScores(cocos2d::CCObject* sender);
    void onUpdate(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher();
    void reloadLeaderboard(LevelLeaderboardType type, LevelLeaderboardMode mode);
    void setupLeaderboard(cocos2d::CCArray* scores);
    void show();
    void updateUserScoreFailed();
    ~LevelLeaderboard();

private:
    LoadingCircle* m_circle = nullptr;
    TextArea* m_noInternet = nullptr;
    GJGameLevel* m_level = nullptr;
    LevelLeaderboardMode m_mode = LevelLeaderboardMode::Time;
    LevelLeaderboardType m_type = LevelLeaderboardType::Local;
    CCMenuItemSpriteExtra* m_timeBtn = nullptr;
    CCMenuItemSpriteExtra* m_pointsBtn = nullptr;
    CCMenuItemSpriteExtra* m_refreshBtn = nullptr;
    GJCommentListLayer* m_list = nullptr;
    cocos2d::CCArray* m_scores = nullptr;
};