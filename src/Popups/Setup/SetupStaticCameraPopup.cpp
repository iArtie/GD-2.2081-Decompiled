#include "Popups/Setup/SetupStaticCameraPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/CameraTriggerGameObject.h"

SetupStaticCameraPopup* SetupStaticCameraPopup::create(CameraTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupStaticCameraPopup* instance = new SetupStaticCameraPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupStaticCameraPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    bool toggle1 = false;
    bool toggle2 = false;

    if (tag == (int)getValue(101))
    {
        tag = 0;
    }
    else
    {
        toggle1 = (tag == 1);
        toggle2 = (tag == 2);
    }

    updateCustomToggleTrigger(1, toggle1);
    updateCustomToggleTrigger(2, toggle2);
    updateValue(101, (float)tag);
}

void SetupStaticCameraPopup::updateState()
{
    float value212 = getValue(212);
    toggleGroup(212, value212 != 0.0f);

    float value453 = getValue(453);
    toggleGroup(453, value453 != 0.0f);
}

void SetupStaticCameraPopup::valueDidChange(int tag, float value)
{
    if (tag == 453 || tag == 212)
        updateState();
}

SetupStaticCameraPopup::~SetupStaticCameraPopup()
{
}