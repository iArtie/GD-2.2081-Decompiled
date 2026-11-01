#pragma once

#include "cocos2d.h"
#include "CCScrollLayerExt.h"

class TableViewCell;
class TableViewDelegate;
class TableViewDataSource;
class TableViewCellDelegate;
class CCIndexPath;
class CCContentLayer;

class TableView : public CCScrollLayerExt {
public:
    TableView(cocos2d::CCRect rect);
    virtual ~TableView();

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    virtual void registerWithTouchDispatcher() override;

    void cancelAndStoleTouch(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    TableViewCell* cellForRowAtIndexPath(CCIndexPath* path);
    TableViewCell* cellForTouch(cocos2d::CCTouch* touch);
    int checkBoundaryOfCell(TableViewCell* cell);
    void checkBoundaryOfContent(float dt);
    void checkFirstCell(TableViewCell* cell);
    void checkLastCell(TableViewCell* cell);
    void claimTouch(cocos2d::CCTouch* touch);
    void deleteTableViewCell(TableViewCell* cell);
    bool dequeueReusableCellWithIdentifier(const char* identifier);
    void initTableViewCells();
    bool isDuplicateInVisibleCellArray(CCIndexPath* path);
    bool isDuplicateIndexPath(CCIndexPath* path);
    void reloadData();
    bool removeIndexPathFromPathAddedArray(CCIndexPath* path);
    void scrllViewWillBeginDecelerating(CCScrollLayerExt* layer);
    void scrollToIndexPath(CCIndexPath* path);
    void scrollViewDidEndDecelerating(CCScrollLayerExt* layer);
    void scrollViewDidEndMoving(CCScrollLayerExt* layer);
    void scrollViewTouchMoving(CCScrollLayerExt* layer);
    void scrollWheel(float y, float x);
    void touchFinish(cocos2d::CCTouch* touch);

public:
    cocos2d::CCPoint m_beginLocation;
    cocos2d::CCPoint m_lastCellPos;
    cocos2d::CCPoint m_checkLocation;
    TableViewDelegate* m_tableDelegate;
    TableViewDataSource* m_dataSource;
    TableViewCellDelegate* m_cellDelegate;
    bool m_unused2;
    bool m_vScrollbarVisible;
    bool m_hScrollbarVisible;
    bool m_disableHorizontal;
    bool m_unused3;
    bool m_scheduled;
    cocos2d::CCArray* m_cellRemovedArray;
    cocos2d::CCArray* m_cellArray;
    cocos2d::CCArray* m_indexPathArray;
    bool m_cancellingTouches;
    bool m_idk2;
    float m_touchLastY;
    cocos2d::CCTouch* m_beginTouch;
    TableViewCell* m_touchedCell;
    bool m_notAtEndOfScroll;
    bool m_touchMoved;
};