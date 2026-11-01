#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupGravityTriggerPopup : public SetupTriggerPopup {
public:
    static SetupGravityTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void valueDidChange(int tag, float value);

    ~SetupGravityTriggerPopup();
};