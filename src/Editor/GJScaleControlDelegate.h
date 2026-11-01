#pragma once

#include "Platform/cocos2d.h"

class GJScaleControlDelegate {
public:
    virtual void anchorPointMoved(cocos2d::CCPoint newAnchor);
    virtual void scaleChangeBegin();
    virtual void scaleChangeEnded();
    virtual void scaleXChanged(float scaleX, bool lock);
    virtual void scaleXYChanged(float scaleX, float scaleY, bool lock);
    virtual void scaleYChanged(float scaleY, bool lock);
    virtual void updateScaleControl();
    virtual ~GJScaleControlDelegate() = default;
};