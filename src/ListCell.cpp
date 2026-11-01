#include "ListCell.h"

ListCell::ListCell(const char* charPtr, float value, float value2)
    : TableViewCell(charPtr, value, value2)
{
    init();
}

void ListCell::draw()
{
    cocos2d::CCSize size(m_width, m_height);
    cocos2d::ccDrawColor4B(m_unk230.r, m_unk230.g, m_unk230.b, m_unk230.a);
    glLineWidth(1.0f);
    cocos2d::CCPoint point(1.0f, size.height);
    cocos2d::CCPoint point2(size.width - 1.0f, size.height);
    cocos2d::ccDrawLine(point, point2);
}

bool ListCell::init()
{
    return true;
}

void ListCell::loadFromObject(cocos2d::CCObject* object, int id, int page, int index)
{
}

void ListCell::updateBGColor(int index)
{
}

ListCell::~ListCell()
{
}