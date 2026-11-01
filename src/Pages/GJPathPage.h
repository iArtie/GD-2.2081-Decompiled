#pragma once

#include "Platform/cocos2d.h"

class GJPathsLayer;
class GJStoreItem;

class GJPathPage : public FLAlertLayer {
public:
    static GJPathPage* create(int path, GJPathsLayer* layer);
    bool init(int path, GJPathsLayer* layer);
    void keyBackClicked() override;
    void onActivatePath(cocos2d::CCObject* sender);
    void onBack(cocos2d::CCObject* sender);
    void onIconInfo(cocos2d::CCObject* sender);
    void onUnlock(cocos2d::CCObject* sender);
    void playUnlockAnimation();
    void registerWithTouchDispatcher() override;
    void show() override;
    void showCantAffordMessage(GJStoreItem* item);
    void unlockAnimationFinished();
    void unlockAnimationStep2();
    void unlockAnimationStep3();
    virtual ~GJPathPage();

private:
    int m_pathNumber;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    GJPathsLayer* m_pathsLayer;
    cocos2d::CCMenu* m_buttonMenu;
    int m_dialogIndex;
    bool m_animationPlaying;
};