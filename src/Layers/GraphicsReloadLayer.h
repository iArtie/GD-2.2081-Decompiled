#pragma once

#include "Platform/cocos2d.h"

class GraphicsReloadLayer : public cocos2d::CCLayer {
public:
    void performReload();
    virtual ~GraphicsReloadLayer();

    cocos2d::CCSize m_resolution;
    int m_quality;
    bool m_fullscreen;
    bool m_borderless;
    bool m_fix;
    bool m_changedResolution;
};