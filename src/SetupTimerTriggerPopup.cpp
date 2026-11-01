#include "SetupTimerTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "TimerTriggerGameObject.h"

SetupTimerTriggerPopup* SetupTimerTriggerPopup::create(TimerTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupTimerTriggerPopup* instance = new SetupTimerTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupTimerTriggerPopup::~SetupTimerTriggerPopup()
{
}