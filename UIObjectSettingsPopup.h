#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class UISettingsGameObject;

class UIObjectSettingsPopup : public SetupTriggerPopup {
public:
    static UIObjectSettingsPopup* create(UISettingsGameObject* object, cocos2d::CCArray* objects);
    bool init(UISettingsGameObject* object, cocos2d::CCArray* objects) override;
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender) override;
    ~UIObjectSettingsPopup();
};