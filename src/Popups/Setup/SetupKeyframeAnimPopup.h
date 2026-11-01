#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupKeyframeAnimPopup : public SetupTriggerPopup {
public:
    static SetupKeyframeAnimPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    ~SetupKeyframeAnimPopup();
};