#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupAreaTriggerPopup.h"

class EnterEffectObject;

class SetupAreaFadeTriggerPopup : public SetupAreaTriggerPopup {
public:
    static SetupAreaFadeTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);

    ~SetupAreaFadeTriggerPopup();
};