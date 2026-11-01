// GameOptionsLayer.h
#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJBaseGameLayer.h"
#include "Layers/GJOptionsLayer.h"

class GameOptionsLayer : public GJOptionsLayer {
public:
    static GameOptionsLayer* create(GJBaseGameLayer* baseGameLayer);
    void didToggle(int tag);
    bool init(GJBaseGameLayer* baseGameLayer);
    void onPracticeMusicSync(cocos2d::CCObject* sender);
    void onUIOptions(cocos2d::CCObject* sender);
    void onUIPOptions(cocos2d::CCObject* sender);
    void setupOptions();
    void showPracticeMusicSyncUnlockInfo();
    ~GameOptionsLayer();

private:
    GJBaseGameLayer* m_baseGameLayer;
    float m_gap;
    float m_maxLabelScale;
    int m_practiceDialogIndex;
    int m_togglesPerPage;
    float m_offset;
    cocos2d::CCMenu* m_buttonMenu;
    float m_maxLabelWidth;
    cocos2d::CCLayer* m_mainLayer;
    int m_toggleCount;
    float m_height;
};