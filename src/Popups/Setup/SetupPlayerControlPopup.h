#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class PlayerControlGameObject;

class SetupPlayerControlPopup : public SetupTriggerPopup {
public:
    static SetupPlayerControlPopup* create(PlayerControlGameObject* object, cocos2d::CCArray* objects);
    bool init(PlayerControlGameObject* object, cocos2d::CCArray* objects);
    ~SetupPlayerControlPopup();
};