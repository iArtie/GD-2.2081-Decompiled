#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class EffectGameObject;
class CCTextInputNode;
class Slider;
class CCLabelBMFont;

class SetupOpacityPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupOpacityPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onClose(cocos2d::CCObject* sender);
    void onTargetIDArrow(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void textInputClosed(CCTextInputNode* node);
    void updateDurLabel(bool decimals);
    void updateDuration();
    void updateOpacity();
    void updateOpacityLabel();
    void updateTargetID();
    void updateTextInputLabel();

    ~SetupOpacityPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    int m_groupID = 0;
    float m_opacity = 1.0f;
    float m_fadeTime = 0.0f;
    CCTextInputNode* m_groupIDInput = nullptr;
    CCTextInputNode* m_fadeTimeInput = nullptr;
    bool m_disableTextDelegate = false;
    CCLabelBMFont* m_opacityLabel = nullptr;
    Slider* m_fadeTimeSlider = nullptr;
    Slider* m_opacitySlider = nullptr;
};