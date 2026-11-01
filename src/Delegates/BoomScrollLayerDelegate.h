#pragma once

#include "Platform/cocos2d.h"

class BoomScrollLayer;

class BoomScrollLayerDelegate {
public:
    void scrollLayerMoved(cocos2d::CCPoint position);
    void scrollLayerScrollingStarted(BoomScrollLayer *layer);
};