#pragma once

#include "cocos2d.h"

class GJUserScore;
class FLAlertLayer;
class TableViewCell;

class GJScoreCell : public TableViewCell {
public:
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    GJScoreCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromScore(GJUserScore* score);
    void onBan(cocos2d::CCObject* sender);
    void onCheck(cocos2d::CCObject* sender);
    void onMoreLevels(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    virtual ~GJScoreCell();

private:
    GJUserScore* m_score;
    float m_width;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};