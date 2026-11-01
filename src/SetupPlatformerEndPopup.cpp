#include "SetupPlatformerEndPopup.h"
#include "SetupTriggerPopup.h"
#include "EndTriggerGameObject.h"

SetupPlatformerEndPopup* SetupPlatformerEndPopup::create(EndTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupPlatformerEndPopup* instance = new SetupPlatformerEndPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupPlatformerEndPopup::~SetupPlatformerEndPopup()
{
}