#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class TransformTriggerGameObject;

class SetupTransformPopup : public SetupTriggerPopup {
public:
    static SetupTransformPopup* create(TransformTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(TransformTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupTransformPopup();
};