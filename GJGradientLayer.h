// GJGradientLayer.h
#pragma once

#include "cocos2d.h"

class GJGradientLayer : public cocos2d::CCLayerGradient {
public:
    static GJGradientLayer* create();
    bool init();
    virtual ~GJGradientLayer();

    cocos2d::ccColor3B m_baseColor;
    cocos2d::ccColor3B m_detailColor;
    cocos2d::CCPoint m_gradientPosition;
    cocos2d::CCPoint m_AlongVector;
    int m_triggerObject;
    bool m_noScale;
    int m_blendingMode;
    int m_blendingLayer;
    int m_gradientID;
};