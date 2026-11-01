#pragma once

#include "cocos2d.h"

class BoomListView;

class BoomListLayer : public cocos2d::CCLayerColor {
public:
    static BoomListLayer* create(BoomListView* listView, const char* title);
    bool init(BoomListView* listView, const char* title);
    ~BoomListLayer();
};