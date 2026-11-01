#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupRotateCommandPopup : public SetupTriggerPopup {
public:
    static SetupRotateCommandPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void onInfiniteDuration(cocos2d::CCObject* sender);
    float triggerValueFromSliderValue(int tag, float value);
    void updateControlVisibility();
    void valueDidChange(int tag, float value);

    ~SetupRotateCommandPopup();
};