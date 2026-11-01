#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class PlayerControlGameObject;

class SetupPlayerControlPopup : public SetupTriggerPopup {
public:
    static SetupPlayerControlPopup* create(PlayerControlGameObject* object, cocos2d::CCArray* objects);
    bool init(PlayerControlGameObject* object, cocos2d::CCArray* objects);
    ~SetupPlayerControlPopup();
};