#include "Editor/ListButtonBar.h"
#include "Layers/BoomScrollLayer.h"
#include "Editor/ListButtonBarDelegate.h"

int ListButtonBar::getPage()
{
    return BoomScrollLayer::getRelativePageForNum(m_scrollLayer, m_scrollLayer->m_page);
}

void ListButtonBar::goToPage(int page)
{
    BoomScrollLayer::getTotalPages(m_scrollLayer);
    if (m_useMoveAnimation) {
        BoomScrollLayer::moveToPage(m_scrollLayer, page);
    } else {
        BoomScrollLayer::instantMoveToPage(m_scrollLayer, page - 1);
        BoomScrollLayer::instantMoveToPage(m_scrollLayer, page);
    }
    if (m_delegate)
        m_delegate->listButtonBarSwitchedPage(this, page);
}

void ListButtonBar::onLeft(cocos2d::CCObject* sender)
{
    goToPage(m_scrollLayer->m_page - 1);
}

void ListButtonBar::onRight(cocos2d::CCObject* sender)
{
    goToPage(m_scrollLayer->m_page + 1);
}

ListButtonBar::~ListButtonBar()
{
}