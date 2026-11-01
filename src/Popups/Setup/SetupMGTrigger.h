#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/ConfigureValuePopupDelegate.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;
class CCLabelBMFont;
class CCMenuItemSpriteExtra;

class SetupMGTrigger : public SetupTriggerPopup, public ConfigureValuePopupDelegate, public TextInputDelegate {
public:
    static SetupMGTrigger* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onEasing(cocos2d::CCObject* sender);
    void onEasingRate(cocos2d::CCObject* sender);
    int posFromSliderValue(float value);
    void sliderChanged(cocos2d::CCObject* sender);
    float sliderValueFromPos(int pos);
    void sliderYChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void toggleEasingRateVisibility();
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateEasingLabel();
    void updateEasingRateLabel();
    void updateMoveCommandEasing();
    void updateMoveCommandEasingRate();
    void updateMoveCommandPosY();
    void updateValueYLabel();
    void valuePopupClosed(ConfigureValuePopup* popup, float value);

    ~SetupMGTrigger();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    EasingType m_mgEasingType = EasingType::None;
    int m_offsetY = 0;
    float m_moveTime = 0.0f;
    float m_mgEasingRate = 0.0f;
    CCTextInputNode* m_moveTimeInput = nullptr;
    CCTextInputNode* m_offsetYInput = nullptr;
    CCLabelBMFont* m_mgEasingRateLabel = nullptr;
    CCMenuItemSpriteExtra* m_mgEasingRateButton = nullptr;
    CCLabelBMFont* m_mgEasingLabel = nullptr;
    bool m_disableTextDelegate = false;
    Slider* m_moveTimeSlider = nullptr;
    Slider* m_offsetYSlider = nullptr;
};