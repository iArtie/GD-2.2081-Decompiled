#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/ConfigureValuePopupDelegate.h"
#include "Delegates/TextInputDelegate.h"

class CameraTriggerGameObject;
class EffectGameObject;
class CCTextInputNode;
class Slider;
class CCMenuItemToggler;
class CCLabelBMFont;

class SetupCameraOffsetTrigger : public SetupTriggerPopup, public ConfigureValuePopupDelegate, public TextInputDelegate {
public:
    static SetupCameraOffsetTrigger* create(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(CameraTriggerGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onEasing(cocos2d::CCObject* sender);
    void onEasingRate(cocos2d::CCObject* sender);
    void onTargetMode(cocos2d::CCObject* sender);
    int posFromSliderValue(float value);
    void sliderChanged(cocos2d::CCObject* sender);
    float sliderValueFromPos(int pos);
    void sliderXChanged(cocos2d::CCObject* sender);
    void sliderYChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void toggleEasingRateVisibility();
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateEasingLabel();
    void updateEasingRateLabel();
    void updateMoveCommandEasing();
    void updateMoveCommandEasingRate();
    void updateMoveCommandPosX();
    void updateMoveCommandPosY();
    void updateValueXLabel();
    void updateValueYLabel();

    ~SetupCameraOffsetTrigger();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    EasingType m_cameraEasingType = EasingType::None;
    int m_offsetX = 0;
    int m_offsetY = 0;
    float m_moveTime = 0.0f;
    float m_cameraEasingRate = 0.0f;
    int m_targetMode = 0;
    CCMenuItemToggler* m_xOnlyToggler = nullptr;
    CCMenuItemToggler* m_yOnlyToggler = nullptr;
    CCTextInputNode* m_moveTimeInput = nullptr;
    CCTextInputNode* m_offsetXInput = nullptr;
    CCTextInputNode* m_offsetYInput = nullptr;
    CCLabelBMFont* m_cameraEasingRateLabel = nullptr;
    CCMenuItemSpriteExtra* m_cameraEasingRateButton = nullptr;
    CCLabelBMFont* m_cameraEasingLabel = nullptr;
    bool m_disableTextDelegate = false;
    Slider* m_offsetYSlider = nullptr;
    Slider* m_moveTimeSlider = nullptr;
    Slider* m_offsetXSlider = nullptr;
};