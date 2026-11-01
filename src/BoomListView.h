#pragma once

#include "cocos2d.h"

class TableViewCell;
class TableView;
class CCIndexPath;
class BoomListType;

class BoomListView {
public:
    void addObjectToList(cocos2d::CCNode* node);
    TableViewCell* cellForRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView);
    float cellHeightForRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView);
    void didSelectRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView);
    void draw();
    TableViewCell* getListCell(const char* identifier);
    void loadCell(TableViewCell* cell, int index);
    void lockList(bool locked);
    int numberOfRowsInSection(unsigned int section, TableView* tableView);
    unsigned int numberOfSectionsInTableView(TableView* tableView);
    void setupList(float yOffset);
    ~BoomListView();

private:
    float m_itemSeparation;
    cocos2d::CCArray* m_entries;
    TableView* m_tableView;
    int m_type;
    bool m_locked;
    float m_width;
    int m_currentPage;
};