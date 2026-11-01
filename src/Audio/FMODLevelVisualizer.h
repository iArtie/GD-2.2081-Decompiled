#pragma once

#include "Platform/cocos2d.h"

class FMODLevelVisualizer : public cocos2d::CCNode
{
public:
    static FMODLevelVisualizer* create();
    
    bool init();
    void updateVisualizer(float volume, float peak, float time);
    
    ~FMODLevelVisualizer();

private:
    float m_width;
};