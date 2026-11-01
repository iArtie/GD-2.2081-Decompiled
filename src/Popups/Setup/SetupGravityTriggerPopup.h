#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupGravityTriggerPopup : public SetupTriggerPopup {
public:
    static SetupGravityTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void valueDidChange(int tag, float value);

    ~SetupGravityTriggerPopup();
};