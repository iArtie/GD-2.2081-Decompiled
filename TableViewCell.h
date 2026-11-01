#pragma once

#include "CCLayer.h"
#include "CCIndexPath.h"
#include <string>

class TableView;

class TableViewCell : public cocos2d::CCLayer {
public:
    TableViewCell(const char* identifier, float width, float height);
    void updateVisibility();
    ~TableViewCell();

    CCIndexPath m_indexPath;
    std::string m_cellIdentifier;
    float m_width;
    float m_height;
    cocos2d::CCLayerColor* m_backgroundLayer;
    cocos2d::CCLayer* m_mainLayer;
    bool m_unknown2;
    TableView* m_tableView;
};