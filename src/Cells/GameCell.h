#pragma once

#include "Cells/TableViewCell.h"
#include "Platform/cocos2d.h"
#include <string>

class GameCell : public TableViewCell {
public:
    GameCell(const char* charPtr, float value, float value2);
    
    void draw() override;
    bool init() override;
    void loadFromString(std::string str);
    void onTouch(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    
    virtual ~GameCell();

private:
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
    std::string m_gameLink;
    float m_width;
    float m_height;
    int m_unk230;
};