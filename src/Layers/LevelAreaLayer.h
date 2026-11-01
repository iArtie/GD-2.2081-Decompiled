#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Delegates/DialogDelegate.h"
#include "Layers/DialogLayer.h"

class LevelAreaLayer : public cocos2d::CCLayer, public DialogDelegate {
public:
    static LevelAreaLayer* create();
    static cocos2d::CCScene* scene();

    virtual bool init() override;
    virtual void keyBackClicked() override;
    virtual void onExit() override;
    virtual ~LevelAreaLayer() override;

    void dialogClosed(DialogLayer* layer) override;
    void fadeInsideTower();
    void onBack(cocos2d::CCObject* sender);
    void onClickDoor(cocos2d::CCObject* sender);
    bool onEnterTower();
    void showDialog();

private:
    cocos2d::CCSprite* m_towerSprite = nullptr;
    cocos2d::CCArray* m_godRays = nullptr;
    bool m_enteringTower = false;
    bool m_exiting = false;
};