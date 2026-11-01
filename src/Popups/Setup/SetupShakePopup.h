#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;

class SetupShakePopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupShakePopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateInterval();
    void updateIntervalLabel(bool decimals);
    void updateShake();
    void updateShakeLabel(bool decimals);

    ~SetupShakePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    float m_duration = 0.0f;
    float m_strength = 0.0f;
    float m_interval = 0.0f;
    CCTextInputNode* m_strengthInput = nullptr;
    CCTextInputNode* m_durationInput = nullptr;
    CCTextInputNode* m_intervalInput = nullptr;
    bool m_disableTextDelegate = false;
    Slider* m_intervalSlider = nullptr;
    Slider* m_durationSlider = nullptr;
    Slider* m_strengthSlider = nullptr;
};