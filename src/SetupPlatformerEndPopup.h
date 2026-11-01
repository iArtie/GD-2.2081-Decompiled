#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class EndTriggerGameObject;

class SetupPlatformerEndPopup : public SetupTriggerPopup {
public:
    static SetupPlatformerEndPopup* create(EndTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(EndTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupPlatformerEndPopup();
};