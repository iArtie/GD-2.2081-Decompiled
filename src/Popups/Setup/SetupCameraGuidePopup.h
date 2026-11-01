#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class CameraTriggerGameObject;

class SetupCameraGuidePopup : public SetupTriggerPopup {
public:
    static SetupCameraGuidePopup* create(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupCameraGuidePopup();
};