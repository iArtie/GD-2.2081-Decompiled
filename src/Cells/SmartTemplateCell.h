#pragma once

#include "Platform/cocos2d.h"
#include "Cells/TableViewCell.h"

class GJSmartTemplate;

class SmartTemplateCell : public TableViewCell {
public:
    SmartTemplateCell(const char* identifier, float width, float height);
    void draw() override;
    bool init() override;
    void loadFromObject(GJSmartTemplate* smartTemplate);
    void onClick(cocos2d::CCObject* sender);
    void updateBGColor(int index) override;
    virtual ~SmartTemplateCell();

protected:
    GJSmartTemplate* m_smartTemplate = nullptr;
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
};