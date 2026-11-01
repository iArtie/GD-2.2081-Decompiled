#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class CameraTriggerGameObject;

class SetupCameraGuidePopup : public SetupTriggerPopup {
public:
    static SetupCameraGuidePopup* create(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(CameraTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupCameraGuidePopup();
};