#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"

class CCMenuItemSpriteExtra;
class GJLevelList;

class LevelListCell : public TableViewCell {
public:
    LevelListCell(const char* identifier, float width, float height);
    static LevelListCell* create(float width, float height);
    void draw() override;
    bool init() override;
    void loadFromList(GJLevelList* list);
    void onClick(cocos2d::CCObject* sender);
    void onListInfo(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    virtual ~LevelListCell();

private:
    float m_width;
    float m_height;
    GJLevelList* m_levelList;
    bool m_addingLevel;
};