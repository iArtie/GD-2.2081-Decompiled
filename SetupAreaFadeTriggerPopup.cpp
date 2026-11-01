#include "SetupAreaFadeTriggerPopup.h"
#include "EnterEffectObject.h"

SetupAreaFadeTriggerPopup* SetupAreaFadeTriggerPopup::create(EnterEffectObject* object, cocos2d::CCArray* objects) {
    SetupAreaFadeTriggerPopup* popup = new SetupAreaFadeTriggerPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

SetupAreaFadeTriggerPopup::~SetupAreaFadeTriggerPopup() {
}