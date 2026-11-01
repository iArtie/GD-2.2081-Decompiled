// GJCommentListLayer.h
#pragma once

#include "Platform/cocos2d.h"
#include "UI/BoomListView.h"

class GJCommentListLayer : public cocos2d::CCLayerColor {
public:
    static GJCommentListLayer* create(BoomListView* list, const char* title, 
                                      cocos2d::ccColor4B color, float width, 
                                      float height, bool useCustomColor);
    bool init(BoomListView* list, const char* title, cocos2d::ccColor4B color,
              float width, float height, bool useCustomColor);
    virtual ~GJCommentListLayer();

    BoomListView* m_list;
};