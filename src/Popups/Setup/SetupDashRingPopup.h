#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class DashRingObject;

class SetupDashRingPopup : public SetupTriggerPopup {
public:
    static SetupDashRingPopup* create(DashRingObject* object, cocos2d::CCArray* objects);
    bool init(DashRingObject* object, cocos2d::CCArray* objects);
    ~SetupDashRingPopup();
};