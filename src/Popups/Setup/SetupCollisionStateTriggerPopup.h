#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupCollisionStateTriggerPopup : public SetupTriggerPopup {
public:
    static SetupCollisionStateTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    ~SetupCollisionStateTriggerPopup();
};