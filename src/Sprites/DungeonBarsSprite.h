#pragma once

#include "Platform/cocos2d.h"

class DungeonBarsSprite : public cocos2d::CCNode {
public:
    static DungeonBarsSprite* create();
    bool init();
    
    void animateOutBars();
    void visit() override;
    
    virtual ~DungeonBarsSprite();

protected:
    cocos2d::CCSprite* m_barsSprite;
};