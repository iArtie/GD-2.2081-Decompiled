#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"
#include "UI/GJItemIcon.h"
#include "UI/TextArea.h"
#include "Managers/AchievementManager.h"

class AchievementCell : public TableViewCell
{
public:
    static AchievementCell* create(const char* identifier, float width, float height);
    
    bool init();
    void draw();
    void loadFromDict(cocos2d::CCDictionary* dict);
    void updateBGColor(int index);
    
    virtual ~AchievementCell();

private:
    cocos2d::CCLayerColor* m_backgroundLayer;
    cocos2d::CCLayer* m_mainLayer;
    float m_width;
    float m_height;
};