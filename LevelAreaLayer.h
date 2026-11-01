#pragma once

#include "cocos2d.h"
#include "CCMenuItemSpriteExtra.h"
#include "DialogDelegate.h"
#include "DialogLayer.h"

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