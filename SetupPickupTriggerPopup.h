#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

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