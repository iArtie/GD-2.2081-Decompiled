#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupAreaTriggerPopup.h"

class EnterEffectObject;

class SetupAreaFadeTriggerPopup : public SetupAreaTriggerPopup {
public:
    static SetupAreaFadeTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);

    ~SetupAreaFadeTriggerPopup();
};