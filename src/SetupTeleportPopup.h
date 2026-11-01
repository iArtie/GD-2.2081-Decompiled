#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class CCMenuItemToggler;

class SetupTeleportPopup : public SetupTriggerPopup {
public:
    void onTeleportGravity(cocos2d::CCObject* sender);
    void updateDefaultTriggerValues() override;
    void updateTeleportGravityState(int state);
    void valueDidChange(int tag, float value) override;

    ~SetupTeleportPopup();

protected:
    cocos2d::CCArray* m_teleportGravityToggles = nullptr;
    cocos2d::CCDictionary* m_triggerValues = nullptr;
};