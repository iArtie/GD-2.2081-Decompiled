#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class TimerTriggerGameObject;

class SetupTimerTriggerPopup : public SetupTriggerPopup {
public:
    static SetupTimerTriggerPopup* create(TimerTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(TimerTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupTimerTriggerPopup();
};