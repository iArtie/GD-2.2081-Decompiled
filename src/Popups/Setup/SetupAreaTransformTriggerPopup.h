#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupAreaMoveTriggerPopup.h"

class EnterEffectObject;

class SetupAreaTransformTriggerPopup : public SetupAreaMoveTriggerPopup {
public:
    static SetupAreaTransformTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects) override;

    ~SetupAreaTransformTriggerPopup();
};