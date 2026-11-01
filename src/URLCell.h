#pragma once

#include "cocos2d.h"
#include "TableViewCell.h"

class CCURLObject;

class URLCell : public TableViewCell {
public:
    URLCell(char* charPtr, float width, float height);

    void draw() override;
    bool init() override;
    void loadFromObject(CCURLObject* object);
    void onURL(cocos2d::CCObject* sender);
    void updateBGColor(int index);

    ~URLCell();

protected:
    CCURLObject* m_urlObject = nullptr;
    float m_width = 0.0f;
    float m_height = 0.0f;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
};