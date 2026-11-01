#pragma once

#include "cocos2d.h"

class TableViewCell;

class CCContentLayer : public cocos2d::CCLayerColor {
public:
    static CCContentLayer* create(const cocos2d::ccColor4B* color, float width, float height);
    virtual void setPosition(const cocos2d::CCPoint* position) override;
    ~CCContentLayer();
};