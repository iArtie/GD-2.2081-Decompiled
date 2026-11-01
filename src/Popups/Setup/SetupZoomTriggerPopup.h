#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;

class SetupZoomTriggerPopup : public SetupTriggerPopup {
public:
    static SetupZoomTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    ~SetupZoomTriggerPopup();

protected:
    bool m_disableTextDelegate = false;
};