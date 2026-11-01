#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class SFXTriggerGameObject;

class SetupSFXEditPopup : public SetupTriggerPopup {
public:
    static SetupSFXEditPopup* create(SFXTriggerGameObject* object, cocos2d::CCArray* objects, bool songTrigger);
    bool init(SFXTriggerGameObject* object, cocos2d::CCArray* objects, bool songTrigger);
    ~SetupSFXEditPopup();
};