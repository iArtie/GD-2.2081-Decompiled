#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupAreaMoveTriggerPopup.h"

class EnterEffectObject;

class SetupAreaTransformTriggerPopup : public SetupAreaMoveTriggerPopup {
public:
    static SetupAreaTransformTriggerPopup* create(EnterEffectObject* object, cocos2d::CCArray* objects);
    bool init(EnterEffectObject* object, cocos2d::CCArray* objects) override;

    ~SetupAreaTransformTriggerPopup();
};