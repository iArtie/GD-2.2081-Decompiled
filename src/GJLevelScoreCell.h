#pragma once

#include "cocos2d.h"

class GJUserScore;
class SimplePlayer;
class TableViewCell;

class GJLevelScoreCell : public TableViewCell {
public:
    GJLevelScoreCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromScore(GJUserScore* score);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    virtual ~GJLevelScoreCell();

private:
    GJUserScore* m_userScore;
    float m_width;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};