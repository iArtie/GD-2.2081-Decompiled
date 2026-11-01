#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupAreaTriggerPopup.h"

class EnterEffectObject;

class SetupAreaRotateTriggerPopup : public SetupAreaTriggerPopup {
public:
    static SetupAreaRotateTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);

    ~SetupAreaRotateTriggerPopup();
};