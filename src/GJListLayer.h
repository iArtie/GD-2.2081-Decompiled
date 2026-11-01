#pragma once

#include "cocos2d.h"

class BoomListView;

class GJListLayer : public cocos2d::CCLayerColor {
public:
    static GJListLayer* create(BoomListView* listView, const char* title, cocos2d::ccColor4B color, float width, float height, int style);
    bool init(BoomListView* listView, const char* title, cocos2d::ccColor4B color, float width, float height, int style);
    virtual ~GJListLayer();

private:
    BoomListView* m_listView;
};