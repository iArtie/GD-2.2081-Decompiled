#pragma once

#include "Platform/cocos2d.h"

class ListCell : public TableViewCell {
public:
    ListCell(const char* charPtr, float value, float value2);
    void draw();
    bool init();
    void loadFromObject(cocos2d::CCObject* object, int id, int page, int index);
    void updateBGColor(int index);
    ~ListCell();

protected:
    float m_width = 0.0f;
    float m_height = 0.0f;
    cocos2d::ccColor4B m_unk230 = {0, 0, 0, 0};
};