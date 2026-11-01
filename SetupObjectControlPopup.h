#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class ObjectControlGameObject;

class SetupObjectControlPopup : public SetupTriggerPopup {
public:
    static SetupObjectControlPopup* create(ObjectControlGameObject* object, cocos2d::CCArray* objects);
    bool init(ObjectControlGameObject* object, cocos2d::CCArray* objects);
    ~SetupObjectControlPopup();
};