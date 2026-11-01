#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class TransformTriggerGameObject;

class SetupTransformPopup : public SetupTriggerPopup {
public:
    static SetupTransformPopup* create(TransformTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(TransformTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupTransformPopup();
};