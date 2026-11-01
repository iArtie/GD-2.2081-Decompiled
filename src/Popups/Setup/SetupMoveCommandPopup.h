#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupMoveCommandPopup : public SetupTriggerPopup {
public:
    static SetupMoveCommandPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    float triggerSliderValueFromValue(int tag, float value);
    float triggerValueFromSliderValue(int tag, float value);
    void updateControlVisibility();
    void updateInputNode(int tag, float value);
    void updateInputValue(int tag, float* value);
    void valueDidChange(int tag, float value);

    ~SetupMoveCommandPopup();

protected:
    bool m_smallStep = false;
};