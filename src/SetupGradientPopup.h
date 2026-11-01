#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class GradientTriggerObject;
class EffectGameObject;
class CCLabelBMFont;

class SetupGradientPopup : public SetupTriggerPopup {
public:
    static SetupGradientPopup* create(GradientTriggerObject* object, cocos2d::CCArray* objects);
    bool init(GradientTriggerObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onBlending(cocos2d::CCObject* sender);
    void onPlusButton(cocos2d::CCObject* sender);
    void onZLayer(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void updateBlending();
    void updateBlendingLabel();
    void updateGradientLabels(bool vertex);
    void updateToggleItem(int value, bool toggled);
    void updateZLayerButtons();
    void valueDidChange(int tag, float value);

    ~SetupGradientPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_blendingMode = 0;
    int m_blendingLayer = 0;
    cocos2d::CCArray* m_gradientLabels = nullptr;
    cocos2d::CCArray* m_layerButtons = nullptr;
    CCLabelBMFont* m_blendingLabel = nullptr;
    bool m_disableTextDelegate = false;
};