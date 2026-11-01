#pragma once

#include "cocos2d.h"
#include "SetupTriggerPopup.h"

class EffectGameObject;

class MultiTriggerPopup : public SetupTriggerPopup {
public:
    static MultiTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);

    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    virtual ~MultiTriggerPopup();
};