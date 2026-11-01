#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EndTriggerGameObject;

class SetupPlatformerEndPopup : public SetupTriggerPopup {
public:
    static SetupPlatformerEndPopup* create(EndTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(EndTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupPlatformerEndPopup();
};