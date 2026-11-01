#pragma once

#include "cocos2d.h"

class GJSmartBlockPreviewSprite : public cocos2d::CCNode {
public:
    static GJSmartBlockPreviewSprite* create();
    void visit() override;
    virtual ~GJSmartBlockPreviewSprite();
};