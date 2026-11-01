#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"

class GJMapPack;
class CCMenuItemSpriteExtra;
class TableViewCell;

class MapPackCell : public TableViewCell {
public:
    MapPackCell(char* charPtr, float value, float value2);
    void draw();
    bool init();
    void loadFromMapPack(GJMapPack* pack);
    void onClaimReward(cocos2d::CCObject* sender);
    void onClick(cocos2d::CCObject* sender);
    void playCompleteEffect();
    void reloadCell();
    void updateBGColor(int index);
    ~MapPackCell();

protected:
    float m_width = 0.0f;
    float m_height = 0.0f;
    cocos2d::CCArray* m_rewardSprites = nullptr;
    cocos2d::CCArray* m_rewardLabels = nullptr;
    GJMapPack* m_mapPack = nullptr;
    CCMenuItemSpriteExtra* m_viewButton = nullptr;
    CCMenuItemSpriteExtra* m_rewardButton = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
};