#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"

class OptionsObject;
class TableView;

class OptionsCell : public TableViewCell {
public:
    OptionsCell(const char* identifier, float width, float height);

    void loadFromObject(OptionsObject* object);
    void onToggleOption(cocos2d::CCObject* sender);
    void updateBGColor(int index);

    virtual ~OptionsCell();

protected:
    OptionsObject* m_optionsObject = nullptr;
    TableView* m_tableView = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    float m_height = 0.0f;
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
};