#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include <string>

class BoomScrollLayer;
class GJGroundLayer;
class LevelPage;
class SongsLayer;
class GameManager;
class GameStatsManager;
class AchievementManager;
class GameLevelManager;
class GJGameLevel;
class DynamicScrollDelegate;
class BoomScrollLayerDelegate;
class ExtendedLayer;
class CCMenuItemSpriteExtra;

class LevelSelectLayer : public cocos2d::CCLayer,
                         public DynamicScrollDelegate,
                         public BoomScrollLayerDelegate {
public:
    static LevelSelectLayer* create(int page);
    static cocos2d::CCScene* scene(int page);

    bool init(int page) override;
    void keyBackClicked() override;
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;

    cocos2d::ccColor3B colorForPage(int page);
    cocos2d::ccColor3B getColorValue(int page1, int page2, float progress);
    int getPageCount();
    static float modifyValue(float value);
    void onBack(cocos2d::CCObject* sender);
    void onDownload(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPlay(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void scrollLayerMoved(cocos2d::CCPoint position);
    void tryShowAd();

    virtual ~LevelSelectLayer();

protected:
    BoomScrollLayer* m_scrollLayer = nullptr;
    float m_width = 0.0f;
    int m_scrolls = 0;
    cocos2d::CCSprite* m_backgroundSprite = nullptr;
    GJGroundLayer* m_groundLayer = nullptr;
    bool m_masterDetectiveUnlocked = false;
};