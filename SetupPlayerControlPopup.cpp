#include "SetupPlayerControlPopup.h"
#include "SetupTriggerPopup.h"
#include "PlayerControlGameObject.h"

SetupPlayerControlPopup* SetupPlayerControlPopup::create(PlayerControlGameObject* object, cocos2d::CCArray* objects)
{
    SetupPlayerControlPopup* instance = new SetupPlayerControlPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupPlayerControlPopup::~SetupPlayerControlPopup()
{
}