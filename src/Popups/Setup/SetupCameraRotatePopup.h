#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/ConfigureValuePopupDelegate.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;
class CCMenuItemSpriteExtra;
class CCLabelBMFont;

class SetupCameraRotatePopup : public SetupTriggerPopup, public ConfigureValuePopupDelegate, public TextInputDelegate {
public:
    static SetupCameraRotatePopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onEasing(cocos2d::CCObject* sender);
    void onEasingRate(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void sliderDegreesChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void toggleEasingRateVisibility();
    void updateCommandDegrees();
    void updateDegreesLabel();
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateEasingLabel();
    void updateEasingRateLabel();
    void updateMoveCommandEasing();
    void updateMoveCommandEasingRate();
    void valuePopupClosed(ConfigureValuePopup* popup, float value);

    ~SetupCameraRotatePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    EasingType m_cameraEasingType = EasingType::None;
    float m_moveTime = 0.0f;
    float m_degrees = 0.0f;
    float m_cameraEasingRate = 0.0f;
    cocos2d::CCArray* m_gameObjects = nullptr;
    CCTextInputNode* m_moveTimeInput = nullptr;
    CCTextInputNode* m_degreesInput = nullptr;
    CCLabelBMFont* m_cameraEasingRateLabel = nullptr;
    CCMenuItemSpriteExtra* m_cameraEasingRateButton = nullptr;
    CCLabelBMFont* m_cameraEasingLabel = nullptr;
    bool m_disableTextDelegate = false;
    Slider* m_moveTimeSlider = nullptr;
    Slider* m_degreesSlider = nullptr;
};