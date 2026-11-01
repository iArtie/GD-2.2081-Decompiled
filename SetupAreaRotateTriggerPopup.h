#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupAreaTriggerPopup.h"

class EnterEffectObject;

class SetupAreaRotateTriggerPopup : public SetupAreaTriggerPopup {
public:
    static SetupAreaRotateTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);

    ~SetupAreaRotateTriggerPopup();
};