#include "SetupAreaRotateTriggerPopup.h"
#include "EnterEffectObject.h"

SetupAreaRotateTriggerPopup* SetupAreaRotateTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects) {
    SetupAreaRotateTriggerPopup* popup = new SetupAreaRotateTriggerPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

SetupAreaRotateTriggerPopup::~SetupAreaRotateTriggerPopup() {
}