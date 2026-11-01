#pragma once

#include "cocos2d.h"

class EditorUI;

class GJTransformControlDelegate {
public:
    virtual void anchorPointMoved(cocos2d::CCPoint anchorPoint);
    virtual cocos2d::CCNode* getTransformNode();
    virtual EditorUI* getUI();
    virtual void transformChangeBegin();
    virtual void transformChangeEnded();
    virtual void transformResetRotation();
    virtual void transformRestoreRotation();
    virtual void transformRotationChanged(float rotation);
    virtual void transformRotationXChanged(float rotationX);
    virtual void transformRotationYChanged(float rotationY);
    virtual void transformScaleXChanged(float scaleX);
    virtual void transformScaleXYChanged(float scaleX, float scaleY);
    virtual void transformScaleYChanged(float scaleY);
    virtual void transformSkewXChanged(float skewX);
    virtual void transformSkewYChanged(float skewY);
    virtual void updateTransformControl();
    virtual ~GJTransformControlDelegate() = default;
};