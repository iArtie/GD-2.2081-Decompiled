#include "SetupCameraGuidePopup.h"
#include "SetupTriggerPopup.h"
#include "CameraTriggerGameObject.h"

SetupCameraGuidePopup* SetupCameraGuidePopup::create(CameraTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupCameraGuidePopup* instance = new SetupCameraGuidePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

SetupCameraGuidePopup::~SetupCameraGuidePopup()
{
}