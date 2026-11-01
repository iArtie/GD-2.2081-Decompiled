#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupBGSpeedTrigger : public SetupTriggerPopup {
public:
    static SetupBGSpeedTrigger* create(EffectGameObject* object, cocos2d::CCArray* objects, int id);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects, int id);
    ~SetupBGSpeedTrigger();
};