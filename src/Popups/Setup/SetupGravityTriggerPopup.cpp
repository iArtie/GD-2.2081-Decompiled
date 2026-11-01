#include "Popups/Setup/SetupGravityTriggerPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"

SetupGravityTriggerPopup* SetupGravityTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupGravityTriggerPopup* instance = new SetupGravityTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupGravityTriggerPopup::valueDidChange(int tag, float value)
{
    if ((tag == 138 || (tag >= 138 && (unsigned int)(tag - 200) <= 1)) && value != 0.0f)
    {
        if (tag == 138)
        {
            updateValue(200, 0.0f);
            updateValue(201, 0.0f);
        }
        else
        {
            updateValue(138, 0.0f);
            if (tag == 200)
                return;
            updateValue(200, 0.0f);
            if (tag == 201)
                return;
            updateValue(201, 0.0f);
        }
    }
}

SetupGravityTriggerPopup::~SetupGravityTriggerPopup()
{
}