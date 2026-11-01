#include "TableViewCell.h"
#include "TableView.h"
#include "cocos2d.h"

TableViewCell::TableViewCell(const char* identifier, float width, float height)
    : m_width(width), m_height(height), m_tableView(nullptr)
{
    m_backgroundLayer = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, 0), width, height);
    this->addChild(m_backgroundLayer, -1);
    
    m_mainLayer = cocos2d::CCLayer::create();
    this->addChild(m_mainLayer);
    
    if (identifier)
    {
        m_cellIdentifier = identifier;
    }
}

void TableViewCell::updateVisibility()
{
    if (!m_tableView)
        return;
    
    float cellHeight = getContentSize().height;
    float viewHeight = m_tableView->getContentSize().height;
    float parentY = getParent()->getPosition().y;
    float cellY = parentY + getPosition().y;
    
    if (cellY <= viewHeight && cellY >= -cellHeight)
    {
        setVisible(true);
    }
    else
    {
        setVisible(false);
    }
}

TableViewCell::~TableViewCell()
{
}