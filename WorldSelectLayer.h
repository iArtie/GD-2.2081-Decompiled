#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "BoomScrollLayerDelegate.h"

class GJWorldNode;
class CCMenuItemSpriteExtra;
class BoomScrollLayer;

class WorldSelectLayer : public cocos2d::CCLayer, public BoomScrollLayerDelegate {
public:
    static WorldSelectLayer* create(int page);
    bool init(int page);
    static cocos2d::CCScene* scene(int page);

    void animateInActiveIsland();
    cocos2d::ccColor3B colorForPage(int page);
    cocos2d::ccColor3B getColorValue(int page1, int page2, float progress);
    void goToPage(int page, bool instant);
    void keyBackClicked() override;
    void onBack(cocos2d::CCObject* sender);
    void onExit() override;
    void onFreeLevels(cocos2d::CCObject* sender);
    void onGarage(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void scrollLayerMoved(cocos2d::CCPoint position);
    void setupWorlds();
    void showCompleteDialog();
    void tryShowAd();
    void unblockButtons();
    void updateArrows();

    ~WorldSelectLayer();

protected:
    BoomScrollLayer* m_scrollLayer = nullptr;
    bool m_buttonsLocked = false;
    bool m_enteringLevel = false;
    bool m_cleanupSprites = false;
    CCMenuItemSpriteExtra* m_arrowLeft = nullptr;
    CCMenuItemSpriteExtra* m_arrowRight = nullptr;
    GJWorldNode* m_worldNode = nullptr;
    cocos2d::CCSprite* m_background = nullptr;
};