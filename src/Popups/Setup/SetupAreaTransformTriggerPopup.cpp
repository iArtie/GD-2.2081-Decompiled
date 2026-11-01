#include "Popups/Setup/SetupAreaTransformTriggerPopup.h"
#include "Objects/EnterEffectObject.h"

SetupAreaTransformTriggerPopup* SetupAreaTransformTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects) {
    SetupAreaTransformTriggerPopup* popup = new SetupAreaTransformTriggerPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAreaTransformTriggerPopup::init(EnterEffectObject* object, cocos2d::CCArray* objects) {
    if (!SetupAreaMoveTriggerPopup::init(object, objects)) {
        return false;
    }

    return true;
}

SetupAreaTransformTriggerPopup::~SetupAreaTransformTriggerPopup() {
}