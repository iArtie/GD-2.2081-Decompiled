#include "Popups/Setup/SetupTransformPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/TransformTriggerGameObject.h"

SetupTransformPopup* SetupTransformPopup::create(TransformTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupTransformPopup* instance = new SetupTransformPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupTransformPopup::~SetupTransformPopup()
{
}