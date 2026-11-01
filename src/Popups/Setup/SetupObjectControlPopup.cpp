#include "Popups/Setup/SetupObjectControlPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/ObjectControlGameObject.h"

SetupObjectControlPopup* SetupObjectControlPopup::create(ObjectControlGameObject* object, cocos2d::CCArray* objects)
{
    SetupObjectControlPopup* instance = new SetupObjectControlPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupObjectControlPopup::~SetupObjectControlPopup()
{
}