#include "SetupSFXEditPopup.h"
#include "SetupTriggerPopup.h"
#include "SFXTriggerGameObject.h"

SetupSFXEditPopup* SetupSFXEditPopup::create(SFXTriggerGameObject* object, cocos2d::CCArray* objects, bool songTrigger)
{
    SetupSFXEditPopup* instance = new SetupSFXEditPopup();
    if (instance->init(object, objects, songTrigger))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupSFXEditPopup::~SetupSFXEditPopup()
{
}