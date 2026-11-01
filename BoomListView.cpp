#include "BoomListView.h"
#include "TableView.h"
#include "TableViewCell.h"
#include "GameCell.h"
#include "ListCell.h"
#include "CCIndexPath.h"
#include "cocos2d.h"

void BoomListView::addObjectToList(cocos2d::CCNode* node)
{
    if (m_entries)
    {
        m_entries->addObject(node);
    }
}

TableViewCell* BoomListView::cellForRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView)
{
    int row = indexPath->m_row;
    
    TableViewCell* cell = tableView->dequeueReusableCellWithIdentifier("cell-identifier");
    if (!cell)
    {
        cell = getListCell("cell-identifier");
        cell->autorelease();
        cell->m_listType = m_type;
        cell->m_tableView = m_tableView;
    }
    
    loadCell(cell, row);
    return cell;
}

float BoomListView::cellHeightForRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView)
{
    return m_itemSeparation;
}

void BoomListView::didSelectRowAtIndexPath(CCIndexPath* indexPath, TableView* tableView)
{
    // Empty implementation
}

void BoomListView::draw()
{
    // Empty implementation
}

TableViewCell* BoomListView::getListCell(const char* identifier)
{
    if (m_type == 1) // List|0x1 - GameCell type
    {
        GameCell* cell = new GameCell(identifier, m_width, 80.0f);
        return cell;
    }
    else
    {
        ListCell* cell = new ListCell(identifier, m_width, 40.0f);
        return cell;
    }
}

void BoomListView::loadCell(TableViewCell* cell, int index)
{
    bool isGameCell = (m_type == 1); // List|0x1
    
    if (isGameCell)
    {
        cocos2d::CCString* str = (cocos2d::CCString*)m_entries->objectAtIndex(index);
        std::string text = str->getCString();
        ((GameCell*)cell)->loadFromString(text);
    }
    else
    {
        cocos2d::CCObject* obj = m_entries->objectAtIndex(index);
        ((ListCell*)cell)->loadFromObject(obj, index + 1, m_currentPage, index);
    }
}

void BoomListView::lockList(bool locked)
{
    m_locked = locked;
    bool enabled = !locked;
    
    m_tableView->setTouchEnabled(enabled);
    m_tableView->setMouseEnabled(enabled);
    m_tableView->setKeyboardEnabled(enabled);
}

int BoomListView::numberOfRowsInSection(unsigned int section, TableView* tableView)
{
    return m_entries->count();
}

unsigned int BoomListView::numberOfSectionsInTableView(TableView* tableView)
{
    return 1;
}

void BoomListView::setupList(float yOffset)
{
    bool isGameCell = (m_type == 1); // List|0x1
    
    if (isGameCell)
    {
        m_itemSeparation = 80.0f;
        m_tableView->reloadData();
        m_tableView->m_scrollLimitTop = 5.0f;
        
        if (m_entries->count() != 1)
        {
            m_tableView->moveToTop();
        }
        else
        {
            m_tableView->moveToTopWithOffset(m_itemSeparation);
        }
    }
    else
    {
        m_itemSeparation = 40.0f;
        m_tableView->reloadData();
        m_tableView->m_scrollLimitTop = 30.0f;
        
        if (m_entries->count() != 1)
        {
            m_tableView->moveToTop();
        }
        else
        {
            m_tableView->moveToTopWithOffset(m_itemSeparation);
        }
    }
}

BoomListView::~BoomListView()
{
    // Destructor
}