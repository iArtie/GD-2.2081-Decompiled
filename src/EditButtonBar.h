#pragma once

#include "cocos2d.h"
#include "BoomScrollLayer.h"

class EditButtonBar : public cocos2d::CCNode {
public:
    static EditButtonBar* create();
    bool init();
    
    int getPage();
    void goToPage(int page);
    void loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage);
    void onLeft(cocos2d::CCObject* sender);
    void onRight(cocos2d::CCObject* sender);
    void reloadItems(int rowCount, int columnCount);
    
    virtual ~EditButtonBar();

protected:
    BoomScrollLayer* m_scrollLayer;
    cocos2d::CCArray* m_buttonArray;
    cocos2d::CCArray* m_pagesArray;
    bool m_hasCreateItems;
    int m_tabIndex;
    cocos2d::CCPoint m_position;
};