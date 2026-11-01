#pragma once

#include "Platform/cocos2d.h"
#include "Delegates/LevelManagerDelegate.h"
#include <string>

enum GauntletType {
    // Gauntlet types
};

class LoadingCircle;
class TextArea;
class CCMenuItemSpriteExtra;
class GJGameLevel;
class GameLevelManager;

class GauntletLayer : public cocos2d::CCLayer, public LevelManagerDelegate {
public:
    static GauntletLayer* create(GauntletType type);
    bool init(GauntletType type);
    void keyBackClicked();
    void loadLevelsFailed(const char* key, int type);
    void loadLevelsFinished(cocos2d::CCArray* levels, const char* key, int type);
    void onBack(cocos2d::CCObject* sender);
    void onLevel(cocos2d::CCObject* sender);
    static cocos2d::CCScene* scene(GauntletType type);
    void setupGauntlet(cocos2d::CCArray* levels);
    void unlockActiveItem();
    virtual ~GauntletLayer();

private:
    cocos2d::CCArray* m_levels;
    GauntletType m_gauntletType;
    LoadingCircle* m_loadingCircle;
    TextArea* m_tryAgainText;
    CCMenuItemSpriteExtra* m_activeItemButton;
    cocos2d::CCArray* m_activeObjects;
    cocos2d::CCSprite* m_backgroundSprite;
    void* m_unkPtr;
};