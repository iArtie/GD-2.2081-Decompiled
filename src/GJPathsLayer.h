#pragma once

#include "cocos2d.h"
#include <string>

class GJPathsLayer : public FLAlertLayer {
public:
    static GJPathsLayer* create();
    void darkenButtons(bool darken);
    bool init() override;
    void keyBackClicked() override;
    static std::string nameForPath(int path);
    void onClose(cocos2d::CCObject* sender);
    void onExit() override;
    void onPath(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher() override;
    void show() override;
    virtual ~GJPathsLayer();

    bool m_exiting;

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCNode* m_scene;
    int m_ZOrder;
    cocos2d::CCSprite* m_closeSprite;
    cocos2d::CCMenu* m_buttonMenu;
};