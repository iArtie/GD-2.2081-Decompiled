#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;
class CCLabelBMFont;
class Slider;

class SetupTimeWarpPopup : public SetupTriggerPopup {
public:
    static SetupTimeWarpPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void sliderChanged(cocos2d::CCObject* sender);
    void updateTimeWarp();
    void updateTimeWarpLabel();

    ~SetupTimeWarpPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    float m_timeWarpMod = 1.0f;
    CCLabelBMFont* m_timeWarpLabel = nullptr;
    Slider* m_timeWarpSlider = nullptr;
};