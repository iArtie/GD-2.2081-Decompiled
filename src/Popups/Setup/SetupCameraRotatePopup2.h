#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupCameraRotatePopup2 : public SetupTriggerPopup {
public:
    static SetupCameraRotatePopup2* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    ~SetupCameraRotatePopup2();
};