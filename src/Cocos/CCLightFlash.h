#pragma once

#include "Platform/cocos2d.h"

class CCLightFlash : public cocos2d::CCNode {
public:
    static CCLightFlash* create();

    bool init();
    void cleanupFlash();
    void fadeAndRemove();
    void removeLights();
    void showFlash();

    void setMainLayer(cocos2d::CCNode* layer) { m_mainLayer = layer; }
    void setZOrder(int zOrder) { m_layerColorZOrder = zOrder; }

    ~CCLightFlash();

private:
    cocos2d::CCLayerColor* m_layerColor;
    cocos2d::CCArray* m_lightStripArray;
    bool m_dontFadeOut;
    cocos2d::CCNode* m_mainLayer;
    int m_layerColorZOrder;
};