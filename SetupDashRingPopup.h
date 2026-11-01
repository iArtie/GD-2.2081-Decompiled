#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class DashRingObject;

class SetupDashRingPopup : public SetupTriggerPopup {
public:
    static SetupDashRingPopup* create(DashRingObject* object, cocos2d::CCArray* objects);
    bool init(DashRingObject* object, cocos2d::CCArray* objects);
    ~SetupDashRingPopup();
};