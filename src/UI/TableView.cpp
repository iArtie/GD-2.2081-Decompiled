#include "UI/TableView.h"
#include "Cells/TableViewCell.h"
#include "Delegates/TableViewDelegate.h"
#include "UI/TableViewDataSource.h"
#include "Delegates/TableViewCellDelegate.h"
#include "Cocos/CCIndexPath.h"
#include "Cocos/CCScrollLayerExt.h"
#include "Platform/cocos2d.h"

TableView::TableView(cocos2d::CCRect rect)
    : CCScrollLayerExt(rect)
{
    m_tableDelegate = nullptr;
    m_dataSource = nullptr;
    m_cellDelegate = nullptr;
    m_unused2 = true;
    setTouchEnabled(true);
    setMouseEnabled(true);
    m_vScrollbarVisible = false;
    m_hScrollbarVisible = false;
    m_disableHorizontal = true;
    m_unused3 = false;
    m_scheduled = false;
    
    m_cellRemovedArray = cocos2d::CCArray::create();
    m_cellRemovedArray->retain();
    
    m_cellArray = cocos2d::CCArray::create();
    m_cellArray->retain();
    
    m_indexPathArray = cocos2d::CCArray::create();
    m_indexPathArray->retain();
    
    m_cancellingTouches = false;
    m_idk2 = false;
    m_touchLastY = 0.0f;
    m_beginTouch = nullptr;
    m_touchedCell = nullptr;
    m_notAtEndOfScroll = false;
    m_touchMoved = false;
}

void TableView::cancelAndStoleTouch(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCSet* set = cocos2d::CCSet::create();
    set->addObject(touch);
    set->autorelease();
    
    m_cancellingTouches = true;
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getTouchDispatcher()->touchesCancelled(set, event);
    m_cancellingTouches = false;
    
    claimTouch(touch);
}

bool TableView::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (!CCScrollLayerExt::ccTouchBegan(touch, event))
        return false;
    
    m_beginTouch = touch;
    m_checkLocation = touch->getLocationInView();
    m_beginLocation = m_checkLocation;
    
    if (!m_scheduled)
    {
        schedule(schedule_selector(TableView::checkBoundaryOfContent), 0.0f);
        m_scheduled = true;
    }
    
    m_touchLastY = m_beginLocation.y;
    return true;
}

void TableView::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_cancellingTouches)
        return;
    
    CCScrollLayerExt::ccTouchCancelled(touch, event);
    touchFinish(touch);
    
    if (m_scheduled)
    {
        unschedule(schedule_selector(TableView::checkBoundaryOfContent));
        m_scheduled = false;
    }
}

void TableView::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    CCScrollLayerExt::ccTouchEnded(touch, event);
    touchFinish(touch);
    
    if (m_scheduled)
    {
        unschedule(schedule_selector(TableView::checkBoundaryOfContent));
        m_scheduled = false;
    }
}

void TableView::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    CCScrollLayerExt::ccTouchMoved(touch, event);
    m_touchMoved = true;
    
    cocos2d::CCPoint pos = touch->getLocationInView();
    pos = cocos2d::CCDirector::sharedDirector()->convertToGL(pos);
    
    if (m_beginTouch == touch)
    {
        m_checkLocation = pos;
    }
    
    if (fabsf(pos.y - m_touchLastY) >= 10.0f)
    {
        m_touchLastY = pos.y;
        cancelAndStoleTouch(touch, event);
    }
}

TableViewCell* TableView::cellForRowAtIndexPath(CCIndexPath* path)
{
    for (unsigned int i = 0; i < m_cellArray->count(); i++)
    {
        TableViewCell* cell = (TableViewCell*)m_cellArray->objectAtIndex(i);
        if (cell && cell->m_indexPath.m_row == path->m_row && cell->m_indexPath.m_section == path->m_section)
        {
            return cell;
        }
    }
    return nullptr;
}

TableViewCell* TableView::cellForTouch(cocos2d::CCTouch* touch)
{
    if (!m_cellArray || m_cellArray->count() == 0)
        return nullptr;
    
    cocos2d::CCPoint location = touch->getLocationInView();
    location = cocos2d::CCDirector::sharedDirector()->convertToGL(location);
    
    for (unsigned int i = 0; i < m_cellArray->count(); i++)
    {
        TableViewCell* cell = (TableViewCell*)m_cellArray->objectAtIndex(i);
        if (!cell)
            continue;
        
        cocos2d::CCPoint local = cell->convertToNodeSpace(location);
        cocos2d::CCRect rect = cell->getBoundingBox();
        if (rect.containsPoint(local))
        {
            return cell;
        }
    }
    return nullptr;
}

int TableView::checkBoundaryOfCell(TableViewCell* cell)
{
    cocos2d::CCPoint pos = cell->getPosition();
    float cellBottom = pos.y + m_contentLayer->getPosition().y;
    float cellTop = cellBottom + cell->getContentSize().height;
    float viewHeight = getContentSize().height;
    
    if (cellBottom <= viewHeight && cellBottom >= 0.0f)
        return 2;
    if (cellTop <= viewHeight && cellTop >= 0.0f)
        return 2;
    return 1;
}

void TableView::checkBoundaryOfContent(float dt)
{
    // Simplified - full implementation would handle cell recycling
    // This is a placeholder for the complex logic in the decompiled code
}

void TableView::checkFirstCell(TableViewCell* cell)
{
    // Simplified - would check if first cell is visible and add cells above if needed
}

void TableView::checkLastCell(TableViewCell* cell)
{
    // Simplified - would check if last cell is visible and add cells below if needed
}

void TableView::claimTouch(cocos2d::CCTouch* touch)
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCTargetedTouchHandler* handler = (cocos2d::CCTargetedTouchHandler*)director->getTouchDispatcher()->findHandler(this);
    if (!handler->getClaimedTouches()->containsObject(touch))
    {
        handler->getClaimedTouches()->addObject(touch);
    }
}

void TableView::deleteTableViewCell(TableViewCell* cell)
{
    if (!cell)
        return;
    
    removeIndexPathFromPathAddedArray(&cell->m_indexPath);
    m_contentLayer->removeChild(cell, true);
    m_cellArray->removeObject(cell);
}

bool TableView::dequeueReusableCellWithIdentifier(const char* identifier)
{
    return false;
}

void TableView::initTableViewCells()
{
    if (!m_tableDelegate || !m_dataSource)
        return;
    
    int sections = m_dataSource->numberOfSectionsInTableView(this);
    if (sections <= 0)
        return;
    
    for (unsigned int section = 0; section < (unsigned int)sections; section++)
    {
        int rows = m_dataSource->numberOfRowsInSection(section, this);
        if (rows <= 0)
            continue;
        
        for (int row = 0; row < rows; row++)
        {
            CCIndexPath* path = new CCIndexPath(row, section);
            path->autorelease();
            
            float height = m_tableDelegate->cellHeightForRowAtIndexPath(path, this);
            cocos2d::CCPoint pos(0.0f, m_contentLayer->getContentSize().height - (height * row) - height);
            
            if (pos.y < 0.0f)
                return;
            
            TableViewCell* cell = m_dataSource->cellForRowAtIndexPath(path, this);
            if (cell)
            {
                cell->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
                cell->setContentSize(cocos2d::CCSize(getContentSize().width, height));
                cell->setPosition(pos);
                
                m_cellArray->addObject(cell);
                m_indexPathArray->addObject(path);
                
                if (m_tableDelegate)
                    m_tableDelegate->TableViewWillDisplayCellForRowAtIndexPath(path, cell, this);
                
                m_contentLayer->addChild(cell);
                
                if (m_tableDelegate)
                    m_tableDelegate->TableViewDidDisplayCellForRowAtIndexPath(path, cell, this);
            }
        }
    }
}

bool TableView::isDuplicateInVisibleCellArray(CCIndexPath* path)
{
    for (unsigned int i = 0; i < m_cellArray->count(); i++)
    {
        TableViewCell* cell = (TableViewCell*)m_cellArray->objectAtIndex(i);
        if (cell && cell->m_indexPath.m_row == path->m_row && cell->m_indexPath.m_section == path->m_section)
        {
            return true;
        }
    }
    return false;
}

bool TableView::isDuplicateIndexPath(CCIndexPath* path)
{
    for (unsigned int i = 0; i < m_indexPathArray->count(); i++)
    {
        CCIndexPath* existing = (CCIndexPath*)m_indexPathArray->objectAtIndex(i);
        if (existing && existing->m_row == path->m_row && existing->m_section == path->m_section)
        {
            return true;
        }
    }
    return false;
}

void TableView::registerWithTouchDispatcher()
{
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getTouchDispatcher()->addTargetedDelegate(this, -138, false);
}

void TableView::reloadData()
{
    // Simplified - full implementation would recreate all cells
    m_cellArray->removeAllObjects();
    m_indexPathArray->removeAllObjects();
    initTableViewCells();
}

bool TableView::removeIndexPathFromPathAddedArray(CCIndexPath* path)
{
    for (unsigned int i = 0; i < m_indexPathArray->count(); i++)
    {
        CCIndexPath* existing = (CCIndexPath*)m_indexPathArray->objectAtIndex(i);
        if (existing && existing->m_row == path->m_row && existing->m_section == path->m_section)
        {
            m_indexPathArray->removeObject(existing);
            return true;
        }
    }
    return false;
}

void TableView::scrllViewWillBeginDecelerating(CCScrollLayerExt* layer)
{
}

void TableView::scrollToIndexPath(CCIndexPath* path)
{
}

void TableView::scrollViewDidEndDecelerating(CCScrollLayerExt* layer)
{
}

void TableView::scrollViewDidEndMoving(CCScrollLayerExt* layer)
{
}

void TableView::scrollViewTouchMoving(CCScrollLayerExt* layer)
{
}

void TableView::scrollWheel(float y, float x)
{
    if (y != 0.0f)
    {
        CCScrollLayerExt::scrollLayer(this, y);
    }
}

void TableView::touchFinish(cocos2d::CCTouch* touch)
{
    if (m_beginTouch != touch)
        return;
    
    m_touchedCell = cellForTouch(touch);
    
    if (m_touchedCell && m_tableDelegate && !m_notAtEndOfScroll)
    {
        m_tableDelegate->didSelectRowAtIndexPath(&m_touchedCell->m_indexPath, this);
    }
    
    m_checkLocation = cocos2d::CCPoint(0, 0);
    m_beginLocation = cocos2d::CCPoint(0, 0);
    m_touchMoved = false;
}

TableView::~TableView()
{
    CC_SAFE_RELEASE_NULL(m_cellRemovedArray);
    CC_SAFE_RELEASE_NULL(m_cellArray);
    CC_SAFE_RELEASE_NULL(m_indexPathArray);
}