#pragma once

#include "Platform/cocos2d.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupAdvFollowEditPhysicsPopup : public SetupTriggerPopup {
public:
    static SetupAdvFollowEditPhysicsPopup* create(AdvancedFollowEditObject* object, cocos2d::CCArray* objects);
    bool init(AdvancedFollowEditObject* object, cocos2d::CCArray* objects);

    void valueDidChange(int tag, float value) override;

    ~SetupAdvFollowEditPhysicsPopup();
};