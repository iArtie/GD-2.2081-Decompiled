#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class ObjectControlGameObject;

class SetupObjectControlPopup : public SetupTriggerPopup {
public:
    static SetupObjectControlPopup* create(ObjectControlGameObject* object, cocos2d::CCArray* objects);
    bool init(ObjectControlGameObject* object, cocos2d::CCArray* objects);
    ~SetupObjectControlPopup();
};