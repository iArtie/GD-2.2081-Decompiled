#pragma once

#include "cocos2d.h"
#include "TableViewCell.h"

class SongObject;

class SongCell : public TableViewCell {
public:
    SongCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromObject(SongObject* object);
    void onClick(cocos2d::CCObject* sender);
    void updateBGColor(int index) override;
    virtual ~SongCell();

protected:
    SongObject* m_songObject = nullptr;
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
};