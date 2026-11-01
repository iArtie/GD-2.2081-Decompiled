#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupBGSpeedTrigger : public SetupTriggerPopup {
public:
    static SetupBGSpeedTrigger* create(EffectGameObject* object, cocos2d::CCArray* objects, int id);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects, int id);
    ~SetupBGSpeedTrigger();
};