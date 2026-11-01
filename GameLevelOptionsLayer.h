#pragma once

#include "GJOptionsLayer.h"
#include "cocos2d.h"

class GJGameLevel;

class GameLevelOptionsLayer : public GJOptionsLayer {
public:
    static GameLevelOptionsLayer* create(GJGameLevel* level);
    
    void didToggle(int tag) override;
    bool init(GJGameLevel* level) override;
    void setupOptions() override;
    
    virtual ~GameLevelOptionsLayer();

private:
    GJGameLevel* m_level;
    float m_gap;
    float m_maxLabelScale;
};