#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class TimerTriggerGameObject;

class SetupTimerTriggerPopup : public SetupTriggerPopup {
public:
    static SetupTimerTriggerPopup* create(TimerTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(TimerTriggerGameObject* object, cocos2d::CCArray* objects);
    ~SetupTimerTriggerPopup();
};