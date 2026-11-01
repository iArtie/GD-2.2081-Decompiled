#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"

class StatsObject;

class StatsCell : public TableViewCell {
public:
    StatsCell(const char* identifier, float width, float height);
    void draw() override;
    const char* getTitleFromKey(const char* key);
    bool init() override;
    void loadFromObject(StatsObject* object);
    void updateBGColor(int index) override;
    virtual ~StatsCell();

protected:
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
};