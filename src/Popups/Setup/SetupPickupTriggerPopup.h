#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupPickupTriggerPopup : public SetupTriggerPopup {
public:
    static SetupPickupTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void onPlusButton(cocos2d::CCObject* sender);
    void updateState();

    ~SetupPickupTriggerPopup();
};