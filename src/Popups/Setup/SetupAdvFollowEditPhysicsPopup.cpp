#include "Popups/Setup/SetupAdvFollowEditPhysicsPopup.h"
#include "Objects/AdvancedFollowEditObject.h"

SetupAdvFollowEditPhysicsPopup* SetupAdvFollowEditPhysicsPopup::create(AdvancedFollowEditObject* object, cocos2d::CCArray* objects) {
    SetupAdvFollowEditPhysicsPopup* popup = new SetupAdvFollowEditPhysicsPopup();
    if (popup && popup->init(object, objects)) {
        popup->autorelease();
        return popup;
    }
    CC_SAFE_DELETE(popup);
    return nullptr;
}

bool SetupAdvFollowEditPhysicsPopup::init(AdvancedFollowEditObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 300.0f, 260.0f, 2)) {
        return false;
    }

    return true;
}

void SetupAdvFollowEditPhysicsPopup::valueDidChange(int tag, float value) {
    if (tag == 306) {
        if (value == 0.0f) {
            return;
        }
        SetupTriggerPopup::updateValue(307, 0.0f);
    } else if (tag == 307 && value != 0.0f) {
        SetupTriggerPopup::updateValue(306, 0.0f);
    }
}

SetupAdvFollowEditPhysicsPopup::~SetupAdvFollowEditPhysicsPopup() {
}