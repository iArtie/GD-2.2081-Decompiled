#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class SetupCameraRotatePopup2 : public SetupTriggerPopup {
public:
    static SetupCameraRotatePopup2* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    ~SetupCameraRotatePopup2();
};