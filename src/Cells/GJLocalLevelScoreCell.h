#pragma once

#include "Platform/cocos2d.h"

class GJLocalScore;
class TableViewCell;

class GJLocalLevelScoreCell : public TableViewCell {
public:
    GJLocalLevelScoreCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromScore(GJLocalScore* score);
    void updateBGColor(int index);
    virtual ~GJLocalLevelScoreCell();

private:
    GJLocalScore* m_localScore;
    float m_width;
    float m_height;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};